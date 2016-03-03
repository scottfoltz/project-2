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


int main(int argc, char * argv[])
{
	
	if (!ProcessOptions(argc, argv, string("hp:s:f::")))
		exit(0);

	cout << "Client targeting server at " << server_address << " on port " << port_number << endl;

	cout << filename << endl;

	return 0;
}