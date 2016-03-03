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

unsigned short port_number;
string server_address;
string filename;
size_t bytes_read;
size_t bytes_sent;

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
				break;
		}
	}
	return retval;

}

void ReadAndWrite (string filename){

    FILE* fd = NULL;
    //memset(buff,0,sizeof(buff));

    //PERRYS TRANSFER LOOP
    const int BUFFER_SIZE = 1024;
	unsigned char buffer[BUFFER_SIZE]; 


	int socket_fd = socket(AF_INET, SOCK_STREAM, 0);


    fd = fopen(filename.c_str(),"rw+");
	fseek (fd, 0, SEEK_END);
	long size = ftell(fd);

	cout << "The files size is: " << size << endl;

    if(NULL == fd)
    {
        printf("\n File could not be opened, due to fopen() error..\n");
    }
    else {
    	cout << filename << endl;
    }

	while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, fd)) > 0)
	{
		 bytes_sent = send(socket_fd, buffer, bytes_read, 0);
	} 

}


int main(int argc, char * argv[])
{
	//Ensure that the -f option has been given
	if (!ProcessOptions(argc, argv, string("hp:s:f::")))
		exit(0);

	cout << "Client targeting server at " << server_address << " on port " << port_number << endl;

	ReadAndWrite(filename);
	//Ensure the file specified can be opened for reading. 

	return 0;
}