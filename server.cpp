
#include <iostream>
#include <cstring> 	// used for memset.
#include <arpa/inet.h> 	// for inet_ntop function
#include <netdb.h>
#include <sys/socket.h>
#include <pthread.h>
#include <vector>
#include <list>
#include <iterator>
#include <sstream>

#include <errno.h>

using namespace std;

unsigned short port_number = 5077;




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


int main(int argc, char * argv[])
{
	if (!ProcessOptions(argc, argv, string("hp:")))
		exit(0);


	cout << "Server started."  << endl ; // don not forgfet endl, or it won't display.


}