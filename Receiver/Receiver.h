#ifndef RECEIVER_H
#define RECEIVER_H

//////////////////////////////////////////////////////////////////////////////////
//  Receiver.h - Provides methods required for receiving meesages from a peer   //
//  ver 1.0                                                                     //
//  Language:          C++, Visual Studio 2105							        //
//  Application:       Dependency-Based Remote Code Repository					//
//				       CSE687 - Object Oriented Design						    //
//  Author:            Sruthi Guvvala, Syracuse University						//
//										.									    //
//////////////////////////////////////////////////////////////////////////////////
/*
Module Operations:
==================
Provides methods required for receiving meesages from a peer

Public Methods:
=================
* start() - Starts the Recceiver

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

//
#include <string>
#include <iostream>
#include "../Sockets/Sockets.h"

class Receiver
{
public:
	Receiver(size_t _port, Socket::IpVer _ipv):sl(_port, _ipv), ss(){   }
	template<typename CallObj>
	bool start(CallObj& co);
private:
	SocketSystem ss;
	SocketListener sl;
};

template<typename CallObj>
bool Receiver::start(CallObj& co)
{
	return sl.start(co);
}

#endif
