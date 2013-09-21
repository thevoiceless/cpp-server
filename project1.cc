#include "includes.h"
#include "utils.h"

#define MAXLINE 1024
#define PORT 8504
#define DEBUG 1

// Read the entire request from the socket and return it
string readRequest(const int sockfd)
{
	const int BUFSIZE = 256;
	int available = 0;
	int charsRead = 0;
	bool started = false;
	bool done = false;
	char c[BUFSIZE];
	stringstream request;

	// Code may reach this part before there is data to read
	// Wait until ready to start reading the request
	while (!started)
	{
		// http://stackoverflow.com/questions/3053757/read-from-socket/3054519#3054519
		// http://man7.org/linux/man-pages/man2/ioctl.2.html
		ioctl(sockfd, FIONREAD, &available);
		// Data is available to read
		if (available > 0)
		{
			started = true;
		}
	}
	// Read the whole request
	while (!done)
	{
		// Determine the number of available bytes before each read
		ioctl(sockfd, FIONREAD, &available);
		if (available > 0)
		{
			// Read up to BUFSIZE bytes
			if ((charsRead = read(sockfd, c, min(BUFSIZE, available))) > 0)
			{
				request.write(c, charsRead);
			}
		}
		// No more data available, end of request
		else
		{
			done = true;
		}
	}

	return request.str();
}

// ***************************************************************************
// * parseGET()
// *  Parse the GET request line and find the filename.  Since HTTP requests
// *  should always be relitive to a particular directory (so you don't 
// *  accidently expose the whole filesystem) you should prepend a path
// *  as well. In this case prepend "." to make the request relitive to
// *  the current directory. 
// *
// *  Note that a real webserver will include other protections to keep
// *  requests from traversing up the path, including but not limited to
// *  using chroot.  Since we can't do that you must remember that as long
// *  as your program is running anyone who knows what port you are using
// *  could get any of your files.
// ***************************************************************************
string parseGET(const string& getRequest)
{
	return "";
}

// ***************************************************************************
// * fileExists()
// *  Simple utility function I use to test to see if the file really
// *  exists.  Probably would have been simpler just to put it inline.
// ***************************************************************************
bool fileExists(const string& filename)
{
	return false;
}

// ***************************************************************************
// * sendHeader()
// *  Send the content type and rest of the header. For this assignment you
// *  only have to do TXT, HTML and JPG, but you can do others if you want.
// ***************************************************************************
bool sendHeader(const int sockfd)
{
	return false;
}

// ***************************************************************************
// * sendFile(int sockfd,string filename)
// *  Open the file, read it and send it.
// ***************************************************************************
bool sendFile(const int sockfd, const string& filename)
{
	return false;
}

// ***************************************************************************
// * send404(int sockfd)
// *  Send the whole error page.  I can really say anything you like.
// ***************************************************************************
bool send404(const int sockfd)
{
	return false;
}

// Processes incoming request, delegates to the appropriate functions
void* processRequest(void* arg)
{
	// Cast as long to prevent issues when the FD is more than 64 bits
	int sockfd = *((long*)arg);
	if (DEBUG)
	{
		cout << "We are in the thread with fd = " << sockfd << endl;
	}


	// Read the request
	string request = readRequest(sockfd);
	if (DEBUG)
	{
		cout << "Request is:\n\n" << request << endl;
	}

	// Split the request by line and determine the type
	vector<string> reqLines = split(request, '\n');
	string reqType = reqLines.front();

	// GET
	if (reqType.find("GET") != string::npos)
	{
		// Parse out the requested resource
		string requestedFile = parseGET(request);
		if (DEBUG)
		{
			cout << "The file they want is " << requestedFile << endl;
		}

		// *******************************************************
		// * Send the file
		// *******************************************************
		if (fileExists(requestedFile))
		{
			// *******************************************************
			// * Build & send the header.
			// *******************************************************
			sendHeader(sockfd);
			if (DEBUG)
			{
				cout << "Header sent" << endl;
			}

			// *******************************************************
			// * Send the file
			// *******************************************************
			sendFile(sockfd,requestedFile);
			if (DEBUG)
			{
				cout << "File sent" << endl;
			}
		}
		else
		{
			// *******************************************************
			// * Send an error message 
			// *******************************************************
			if (DEBUG)
			{
				cout << "File " << requestedFile << " does not exist." << endl;
			}
			send404(sockfd);
			if (DEBUG)
			{
				cout << "Error message sent." << endl;
			}
		}
	}

	if (DEBUG)
	{
		cout << "Thread terminating" << endl;
	}
	// Free the memory allocated for the FD
	free(arg);
}

int main(int argc, char **argv)
{
	// if (argc != 1)
	// {
	// 	cout << "Usage: " << argv[0] << endl;
	// 	exit(-1);
	// }

	// Create the inital socket
	int listenfd = -1;
	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		cout << "socket() failed: " << strerror(errno) << endl;;
		exit(1);
	}

	// Set up the sockaddr_in structure
	struct sockaddr_in servaddr;
	// Zero out the memory
	memset(&servaddr, 0, sizeof(servaddr));
	// Set family, port, and IP
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	// When testing, don't wait on the port (takes too long in-between runs)
	if (DEBUG)
	{
		int optval = 1;
		setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);
	}

	// Bind the socket to a port
	if (bind(listenfd, (sockaddr*) &servaddr, sizeof(servaddr)) == -1)
	{
		cerr << "bind() failed: " << strerror(errno) << endl;;
		exit(1);
	}
	if (DEBUG)
	{
		cerr << "Process has bound fd " << listenfd << " to port " << PORT << endl;
	}

    // Initialize the listening queue for incoming connections
    int listenqueue = 1;
    if (listen(listenfd, listenqueue) == -1)
    {
    	cerr << "listen() failed: " << strerror(errno) << endl;;
    	exit(1);
    }
	if (DEBUG)
	{
		cout << "We are now listening for new connections" << endl;
	}

    // Loop until finished
	set<pthread_t*> threads;
	while (1)
	{
		if (DEBUG)
		{
			cout << "Calling accept() in master thread." << endl;
		}
		int connfd = -1;
	    // accept() will block waiting for connections, creates a new socket FD when a request arrives
		if ((connfd = accept(listenfd, (sockaddr*)NULL, NULL)) == -1)
		{
			cerr << "accept() failed: " << strerror(errno) << endl;
		}
	
		if (DEBUG)
		{
			cout << "Spawning new thread to handle connect on fd = " << connfd << endl;
		}
		// Had issues passing connfd to processRequest, malloc seems to have fixed it
		int* cfd = (int*)malloc(sizeof(int));
		*cfd = connfd;

		// Spawn a new thread to handle the request
		pthread_t* threadID = new pthread_t;
		pthread_create(threadID, NULL, processRequest, (void*) cfd);
		threads.insert(threadID);
	}
}
