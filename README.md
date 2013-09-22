Simple C++ Server
=====
This is a simple HTTP server written in C++ (with some bastardized C thrown in for good measure) that uses raw sockets for communication. So far it only responds to GET requests for directories, HTML files, text files, and JPEGs (based on file extensions). Requests for directories return a listing of the directory contents, and the other requests return the files themselves. If the requested resource is not a directory and doesn't end with a known file extension, the server replies saying it doesn't know how to send that type of resource. Successful requests return 200 OK, requests for nonexistent resources return 404 Not Found, and any type of request besides GET returns 418 I'm a teapot (yes, that's a real status code). It should be trivial to add support for other file types and requests.

Installation & Use
-----
Simply run `make` and then `./server`. This creates a TCP socket, binds it to port 8504 (specified in `server.cpp`), and then starts listening for incoming connections. When a request is received, a new thread is spawned to handle the request.

Notes
-----
I have not tested the server under any kind of excessive (or even moderate) load. The code was developed on Ubuntu 12.04 and probably won't work on any system that isn't POSIX-compliant.