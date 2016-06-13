//////////////////////////////////////////////////////////////////////////////////
//  Sender.h - Provides methods required for sending meesages to a peer         //
//  ver 1.0                                                                     //
//  Language:          C++, Visual Studio 2105							        //
//  Application:       Dependency-Based Remote Code Repository					//
//				       CSE687 - Object Oriented Design						    //
//  Author:            Sruthi Guvvala, Syracuse University						//
//																			    //
//////////////////////////////////////////////////////////////////////////////////
/*
Module Operations:
==================
Provides methods required for sending meesages to a peer using Sockets

Public Methods:
=================
*connect() - Connects to a peer
sendMessage() - Sends String message to a peer
sendfile() - Sends file stram to a peer

Build Process:
==============
Required files
-	Sockets.h, Sockets.cpp

Build commands
- devenv StringSocketServer.sln

Maintenance History:
====================
ver 1.0 : 3 May 16
- first release

*/

#ifndef SENDER_H
#define SENDER_H
//
#include <string>
#include <iostream>
#include "../Sockets/Sockets.h"
#include <sstream>
#include <iostream>



class Sender {
public:
	bool connect(std::string address, size_t port );
	bool sendMessage(std::string message);
	bool sendfile(size_t bytes, char* buffer);
private:
	SocketSystem ss;
	SocketConnecter si;
};

#endif