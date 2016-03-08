//Grant Johnson, Scott Foltz, Liz Valentine
#include <iostream>
#include <iomanip>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <memory.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <assert.h>
#include <unordered_set>

using namespace std;

unsigned short port_number = 5077;
string server_address = "127.0.0.1";
string filename;
bool fileEntered = false;
size_t bytes_read;
size_t bytes_sent;


struct sockaddr_in server_sockaddr;
struct hostent * server_hostent;

bool ProcessOptions(int argc, char * argv[], string format_string)
{
	char c;
	bool retval = true;
	
	while ((c = getopt(argc, argv, format_string.c_str())) != -1)
	{
		switch (c)
		{
			case 'h':
				cerr << argv[0] << " options:" << endl;
				cerr << "	-h displays help" << endl;
				cerr << "	-s server_address ... defaults to 127.0.0.1" << endl;
				cerr << "	-p port_number ... defaults to 4095" << endl;
				cerr << "   -f file name ... include .txt" << endl;
					retval = false;
			case 's':
				server_address = string(optarg);
				break;
			case 'p':
				port_number = atoi(optarg);
				break;
			case 'f':
				filename = string(optarg);
				fileEntered = true;
				break;
		}
	}
	
	if(!fileEntered)
	{
		cout << "No filename was passed in." << endl;
		retval = false;
	}
	
	return retval;

}

void SendToServer (string filename){

    FILE* fd = NULL;	
    const int BUFFER_SIZE = 1024;
	unsigned char buffer[BUFFER_SIZE];

	unsigned int filenameLength = filename.length();
	
    fd = fopen(filename.c_str(),"rw+");
    if(fd == NULL)
    {
        perror("ERROR opening file");
        exit(1);
    }
    
    
	cout << "Client targeting server at " << server_address << " on port " << port_number << endl;
	int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_fd < 0)
	{
		perror("ERROR making socket");
		exit(1);
	}
    //Connect to server
    server_hostent = gethostbyname(server_address.c_str());
	if (server_hostent == nullptr)
	{
		cerr << "ERROR, no such host: " << server_address << endl;
		exit(1);
	}

	// Zero out the entire server_sockaddr. Note that many sample programs use the
	// less portable bzero(). memset() is preferred.
	memset(&server_sockaddr, 0, sizeof(server_sockaddr));
	
	// The family specifies an INTERNET socket rather than a file system socket.
	server_sockaddr.sin_family = AF_INET;

	// Initialize the server address by copying the results of gethostbyname. Note
	// many samples online use bcopy. This is not portable. memmove is more powerful
	// than memcopy so is used here (even though, by construction, the benefits of
	// memmove are not being used).
	memmove(&server_sockaddr.sin_addr.s_addr, server_hostent->h_addr, server_hostent->h_length);
	
	// To assign the port number, use host-to-network-short.
	server_sockaddr.sin_port = htons(port_number);
	cout << "Connecting on port "<< port_number << endl;

	//Connect to server
	if (connect(socket_fd, (struct sockaddr *)&server_sockaddr , sizeof(server_sockaddr)) < 0)
    {
        perror("ERROR on connect");
        exit(1);
    }
     	
    //Send the length of the filename
    if(send(socket_fd, (void*) &filenameLength, 4, 0) < 0)
    {
    	perror("ERROR on send");
    	exit(1);
    }
    
    //Send the filename
	if(send(socket_fd, (void*) filename.c_str(), filenameLength, 0) < 0)
	{
		perror("Error on send");
		exit(1);
	}
	
	while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, fd)) > 0)
	{
		bytes_sent = send(socket_fd, buffer, bytes_read, 0);
		if(bytes_sent < 0)
		{
			perror("Error on send");
			exit(1);
		}
	} 
	
	cout << filename << " sent" << endl;

}


int main(int argc, char * argv[])
{
	//Ensure that the -f option has been given
	if (!ProcessOptions(argc, argv, string("hp:s:f:")))
		exit(0);

	//Ensure the file specified can be opened for reading and send to server
	SendToServer(filename);

	return 0;
}
