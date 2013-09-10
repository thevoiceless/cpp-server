#include "includes.h"

#define MAXLINE 1024
#define PORT 9873
#define DEBUG 1

// ***************************************************************************
// * readGetRequest()
// *  This function should read each line of the request sent by the client
// *  and check to see if it contains a GET request (which is the part  we are
// *  interested in).  You know you have read all the lines when you read
// *  a blank line.
// ***************************************************************************
string readGetRequest(int sockfd)
{
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
string parseGET(string getRequest)
{
}

// ***************************************************************************
// * fileExists()
// *  Simple utility function I use to test to see if the file really
// *  exists.  Probably would have been simpler just to put it inline.
// ***************************************************************************
bool fileExists(string filename)
{
}

// ***************************************************************************
// * sendHeader()
// *  Send the content type and rest of the header. For this assignment you
// *  only have to do TXT, HTML and JPG, but you can do others if you want.
// ***************************************************************************
bool sendHeader(int sockfd)
{
}

// ***************************************************************************
// * sendFile(int sockfd,string filename)
// *  Open the file, read it and send it.
// ***************************************************************************
bool sendFile(int sockfd, string filename)
{
}

// ***************************************************************************
// * send404(int sockfd)
// *  Send the whole error page.  I can really say anything you like.
// ***************************************************************************
bool send404(int sockfd)
{
}

// ***************************************************************************
// * processRequest()
// *  Master function for processing thread.
// *  !!! NOTE - the IOSTREAM library and the cout varibables may or may
// *      not be thread safe depending on your system.  I use the cout
// *      statments for debugging when I know there will be just one thread
// *      but once you are processing multiple rquests it might cause problems.
// ***************************************************************************
void* processRequest(void *arg)
{
	// *******************************************************
	// * This is a little bit of a cheat, but if you end up
	// * with a FD of more than 64 bits you are in trouble
	// *******************************************************
	int sockfd = (long)arg;
	if (DEBUG)
	{
		cout << "We are in the thread with fd = " << sockfd << endl;
	}

	// *******************************************************
	// * Now we need to find the GET part of the request.
	// *******************************************************
	string getRequest = readGetRequest(sockfd);
	if (DEBUG)
	{
		cout << "Get request is " << getRequest << endl;
	}

	// *******************************************************
	// * Find the path/file part of the request.
	// *******************************************************
	string requestedFile = parseGET(getRequest);
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

	if (DEBUG)
	{
		cout << "Thread terminating" << endl;
	}
}

// ***************************************************************************
// * Main
// ***************************************************************************
int main(int argc, char **argv)
{
   if (argc != 1)
   {
		cout << "Usage: " << argv[0] << endl;
		exit(-1);
	}

	// *******************************************************************
	// * Creating the inital socket is the same as in a client.
	// ********************************************************************
	int listenfd = -1;

	// ********************************************************************
	// * The same address structure is used, however we use a wildcard
	// * for the IP address since we don't know who will be connecting.
	// ********************************************************************
	struct sockaddr_in servaddr;

	// ********************************************************************
	// * Binding configures the socket with the parameters we have
	// * specified in the servaddr structure.  This step is implicit in
	// * the connect() call, but must be explicitly listed for servers.
	// ********************************************************************
	if (DEBUG)
	{
		cout << "Process has bound fd " << listenfd << " to port " << PORT << endl;
	}

	// ********************************************************************
    // * Setting the socket to the listening state is the second step
	// * needed to being accepting connections.  This creates a que for
	// * connections and starts the kernel listening for connections.
    // ********************************************************************
	if (DEBUG)
	{
		cout << "We are now listening for new connections" << endl;
	}

	// ********************************************************************
    // * The accept call will sleep, waiting for a connection.  When 
	// * a connection request comes in the accept() call creates a NEW
	// * socket with a new fd that will be used for the communication.
    // ********************************************************************
	set<pthread_t*> threads;
	while (1)
	{
		if (DEBUG)
		{
			cout << "Calling accept() in master thread." << endl;
		}
		int connfd = -1;
	
		if (DEBUG)
		{
			cout << "Spawing new thread to handle connect on fd = " << connfd << endl;
		}

		pthread_t* threadID = new pthread_t;
		pthread_create(threadID, NULL, processRequest, &connfd);
		threads.insert(threadID);
	}
}
