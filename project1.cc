#include "includes.h"
#include "utils.h"

#define MAXLINE 1024
#define PORT 8504
#define DEBUG 1

// Read the entire request from the socket and return it
string readRequest(const int sockfd)
{
	const int BUFSIZE = 256;
	int available     = 0;
	int charsRead     = 0;
	bool started      = false;
	bool done         = false;
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

// Determine which resource the GET request is asking for
string parseGET(const string& getRequest)
{
	// Split tokens based on whitespace, path will be the second one
	vector<string> tokens = split(getRequest, ' ');
	string path = tokens[1];
	// Replace HTML-encoded spaces
	replaceAll(path, "%20", " ");
	return path;
}

// Build the HTTP response, headers and body
void buildResponse(const string& path, stringstream& response)
{
	// Determine the type of resource requested
	string type = path.substr(path.rfind('.'));
	// Assume we know how to handle this type
	bool knownType = true;

	// Assume 200 OK
	response << "HTTP/1.1 200 OK\r\n";

	// Plain text
	if (strcasecmp(type.c_str(), ".txt") == 0)
	{
		response << "Content-Type: text/plain\r\n";
	}
	// HTML
	else if (strcasecmp(type.c_str(), ".html") == 0)
	{
		response << "Content-Type: text/html\r\n";
	}
	// JPEG
	else if (strcasecmp(type.c_str(), ".jpg") == 0 || strcasecmp(type.c_str(), ".jpeg") == 0)
	{
		response << "Content-Type: image/jpeg\r\n";
	}
	// Something we don't know how to send yet
	else
	{
		knownType = false;
		string message = "Sorry, I don't know how to send that type of file (yet).";
		response << "Content-Type: text/html\r\n";
		response << "Content-Length: " << message.length() << "\r\n\n";
		response << message;
	}

	// If we know how to handle this type of resource, add it to the response
	if (knownType)
	{
		response << "Content-Length: " << getFileSize(path.c_str()) << "\r\n\r\n";
		vector<char> bytes = readAllBytes(path.c_str());
		copy(bytes.begin(), bytes.end(), ostream_iterator<char>(response));
	}
}

// Write the response to the socket
bool sendResponse(const int sockfd, stringstream& response)
{
	// Ensure that we're at the end of the stringstream before determining length
	response.seekp(0, ios::end);

	if (write(sockfd, response.str().c_str(), response.tellp()) < 0)
	{
		return false;
	}
	return true;
}

// Build a 404 response
void build404(const int sockfd, const string& path)
{
	string message = "<!DOCTYPE html><html><head><title>Resource does not exist</title></head><body>The requested resource " + path + " could not be found.</body></html>";
	stringstream response;

	response << "HTTP/1.1 404 Not Found\r\n";
	response << "Content-Type: text/html\r\n";
	response << "Content-Length: " << message.length() << "\r\n\n";
	response << message;

	sendResponse(sockfd, response);
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
	stringstream response;

	// GET
	if (reqType.find("GET") != string::npos)
	{
		// Parse out the requested resource
		string localPath = parseGET(reqType);
		string requestedPath = getCurrentDirectory().append(localPath);
		if (DEBUG)
		{
			cout << "The requested resource is " << requestedPath << endl;
		}

		// If a directory is requested, return its contents
		if (isDir(requestedPath))
		{
			cout << "That's a directory, sending contents" << endl;
			vector<string> contents = getDirectoryContents(requestedPath);
			for (vector<string>::iterator i = contents.begin(); i != contents.end(); ++i)
			{
				cout << *i << endl;
			}
		}
		// If just a file is requested, send it
		else if (isFile(requestedPath))
		{
			cout << "That's a file, sending it" << endl;

			// Create the response with headers and body content
			buildResponse(requestedPath, response);
		}
		// If it's not a valid file or directory, send an error message
		else
		{
			if (DEBUG)
			{
				cout << "Resource " << requestedPath << " does not exist." << endl;
			}
			build404(sockfd, localPath);
			if (DEBUG)
			{
				cout << "Error message sent." << endl;
			}
		}

		// Send the response
		sendResponse(sockfd, response);
		if (DEBUG)
		{
			cout << "Response sent" << endl;
		}
	}

	if (DEBUG)
	{
		cout << "Thread terminating" << endl;
	}
	// Close the socket
	close(sockfd);
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
