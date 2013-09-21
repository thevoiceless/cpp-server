Name: Riley Moses

Instructions and Notes: 
tar xvf rimoses.tar, cd rimoses, make, ./project1
The application uses port 8504 and debug output is enabled by default (wasn't sure if we were supposed to keep it).
Only GET requests for directories, text files, HTML files, and JPEG files are supported.
GET requests for directories return a list of the directory contents.
Any other type of request returns 418 "I'm a teapot" (RFC 2324).