#ifndef SERVER_H
#define SERVER_H
//////////////////////////////////////////////////////////////////////////////////////
//  Server.h - Provides methods to interact with the Client and acts as repository  //
//  ver 1.0                                                                         //
//  Language:          C++, Visual Studio 2105							            //
//  Application:       Dependency-Based Remote Code Repository				    	//
//				       CSE687 - Object Oriented Design						        //
//  Author:            Sruthi Guvvala, Syracuse University						   	//
//																			        //
//////////////////////////////////////////////////////////////////////////////////////
/*
Module Operations:
==================
This module provides methods to interact with the server and can sent and receive
HTTP styled messages and files.

Public Methods:
=================
* startSender() - Starts the sender thread which continuously listens on the Sender Queue
* sendFiles() - Sends the files in a package to the concerned client using Sockets
* addtoReceiveQ()  - Adds Http messages to the receive Queue
* processMessage() - Processes the received Http Message from the server.
* processExtractMsg() - Sends final message to the Server to indicate the end of messages
* getFileConciseFullSpec() - Generates trimmed file path


Build Process:
==============
Required files
-	Sockets.h, Sockets.cpp, Logger.h, Logger.cpp, Utilities.h, Utilities.cpp,
Sender.h, Sender.cpp, Receiver.h, Receiver.cpp, XmlElement.h, XmlElement.cpp
XmlDocument.h, XmlDocument.cpp, FileManager.h, FileManager.h, FileSystem.h, FileSystem.cpp
XmlElement.h, XmlElement.cpp, XmlDocument.h, XmlDocument.cpp, XmlParser.h, XmlParser.cpp
Cpp11-BlockingQueue.h, Cpp11-BlockingQueue.cpp, HttpMessage.h, HttpMessage.cpp

Build commands
- devenv StringSocketServer.sln

Maintenance History:
====================
ver 1.0 : 3 May 16
- first release

*/


#include <string>
#include <iostream>
#include "../Logger/Cpp11-BlockingQueue.h"
#include "../Sender/Sender.h"
#include "../Receiver/Receiver.h"
#include "../Sockets/Sockets.h"
#include "../HttpMessage/HttpMessage.h"
#include "../Repository/Repository.h"

class Server
{
public:
	void processMessage();
	void addtoReceiveQ(HttpMessage);
	void startSender(std::string address, size_t port, Sender& sndr);
	bool addRepository(Repository* re);
	void processExtractMsg(std::string packageName);
	void sendFiles(BlockingQueue<std::string> files, BlockingQueue<std::string> fileNames, int numberOfFiles, std::string packageName);

private:
	Repository* repo;
	BlockingQueue <HttpMessage> sendQ;
	BlockingQueue <HttpMessage> receiveQ;


};


#endif // !SERVER_H

