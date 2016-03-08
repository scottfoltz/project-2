//Grant Johnson, Scott Foltz, Liz Valentine
#include <iostream>
#include <cstring> 	// used for memset.
#include <arpa/inet.h> 	// for inet_ntop function
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <vector>
#include <list>
#include <iterator>
#include <sstream>
#include <vector>
#include <getopt.h>
#include <thread>
#include <errno.h>
#include <stdio.h>

using namespace std;

unsigned short port_number = 5077;
	

bool ProcessOptions(int argc, char * argv[], string format_string)
{
	char c;
	bool retval = true;
	
	while ((c = getopt(argc, argv, "hp:")) != -1)
	{
		switch (c)
		{
			case 'h':
				cerr << argv[0] << " options:" << endl;
				cerr << "	-h displays help" << endl;
				cerr << "	-p port_number ... defaults to 5077" << endl;
		                retval = false;
				break;
				
			case 'p':
				port_number = atoi(optarg);
				break;

		}
	}
	return retval;
}


void HandleClient(int clientSocket)
{
	unsigned char* buffer[1024];
		
	//First, receive 4 bytes for the length of the name of the file
	unsigned int nameLength;
	
	if(recv(clientSocket, (void*) &nameLength, 4, 0) == -1)
	{
		perror("Error receiving from client");
		exit(1);
	}
	
	cout << "Length of filename: " << nameLength << endl;
	
	//Next, make a buffer the size of nameLength, this will be the filename
	unsigned char* filename = new unsigned char [nameLength];
	if(recv(clientSocket, (void*) filename, nameLength, 0) == -1)
	{
		perror("Error receiving from client");
		exit(1);
	}
	
	cout << "Filename: " << filename << endl;
	
	//Opening the file for writing
	const char* flag = "w";
	
	//Open the file for writing (If no file exists, it will create one, if a file exists, it will rewrite over the contents)
	FILE* file = fopen((const char*)filename, flag);
	
	//Check for errors
	if(file == nullptr)
	{
		perror("Error opening file");
		exit(1);
	}
	
	ssize_t bytes_received;
	ssize_t bytes_written;
	
	while((bytes_received = recv(clientSocket, buffer, 1024, 0)) > 0)
	{
		//Write to the file using buffer, only write as many bytes as we received.
		bytes_written = fwrite(buffer, sizeof(char), bytes_received, file);
		//Check that nothing went wrong.
		if(bytes_written != bytes_received)
		{
			perror("Error writing to file");
			exit(1);
		}
	}
	
	//Close the file when we're done with it.
	fclose(file);
		
	//Close the socket
	close(clientSocket);
}

int main(int argc, char * argv[])
{
	int tcp_socket;
	socklen_t client_length;
	struct sockaddr_in server_sockaddr;
	struct sockaddr_in client_sockaddr;
	
	if (!ProcessOptions(argc, argv, string("hp:")))
		exit(0);
	
	//Make a tcp socket. SOCK_STREAM indicates it's a streaming socket rather than a Datagram socket
	tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (tcp_socket < 0)
	{
		perror("ERROR opening socket");
		exit(1);
	}

	memset(&server_sockaddr, 0, sizeof(server_sockaddr));

	server_sockaddr.sin_family = AF_INET;
	server_sockaddr.sin_addr.s_addr = INADDR_ANY;
	server_sockaddr.sin_port = htons(port_number);
	
	if (bind(tcp_socket, (const struct sockaddr *) &server_sockaddr, socklen_t(sizeof(server_sockaddr))) < 0)
	{
		close(tcp_socket);
		perror("ERROR on binding");
		exit(1);
	}

	cout << "Socket bound on port: " << port_number<< endl;

	client_length = sizeof(client_sockaddr);
	if(listen(tcp_socket, 5) < 0)
	{
		perror("ERROR on listening");
		exit(1);
	}
	
	while(1)
	{
		auto clientSocket = accept(tcp_socket, (struct sockaddr*) &client_sockaddr, &client_length);
		
		if(clientSocket < 0)
		{
			perror("ERROR on accepting");
			exit(1);
		}
		
		cout << "Connection established from client at: " << inet_ntoa(client_sockaddr.sin_addr) << endl;
		cout << "Port: " << client_sockaddr.sin_port << endl;
		
		//Make a thread to handle this client
		thread clientHandler(HandleClient, clientSocket);
		//Detach
		clientHandler.detach();		
	}
}
