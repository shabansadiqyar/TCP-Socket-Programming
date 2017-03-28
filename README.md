# TCP-Sockets

The purpose of the project was to develop a Web Server in C using a TCP connection between a client and a server. I created the server using sockets in order to establish the TCP connection. The client side was done by using a browser and requesting from the server. The server must be able to receive the HTTP requests from the client and process the HTTP requests. Then the server must send a response message back to the server.

## How to Run and Test

In order to compile the program, type
```
make
```
which will create the C object. The Makefile will have everything in order for make to work. One can also run 
```
gcc –o server.c server 
```
in order to compile the program. The server is ran by typing ./server 2500 on the command line . The port number is arbitrary but we specified it as 2500. Then on the client side or browser just type http://<i></i>127.0.0.1:2500/somefile.html. The somefile.html can be any html file as long as it is contained within the same directory as the server.
