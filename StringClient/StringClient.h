#ifndef STRINGCLIENT_H
#define STRINGCLIENT_H
//////////////////////////////////////////////////////////////////////////////////
//  StringClient.h - Provides methods to interact with the server				//
//  ver 1.0                                                                     //
//  Language:          C++, Visual Studio 2105							        //
//  Application:       Dependency-Based Remote Code Repository					//
//				       CSE687 - Object Oriented Design						    //
//  Author:            Sruthi Guvvala, Syracuse University,						//
//																			    //
//////////////////////////////////////////////////////////////////////////////////
/*
Module Operations:
==================
This module provides methods to interact with the server and can sent and receive
HTTP styled messages and files.

Public Methods:
=================
* startSender() - Starts the sender thread which continuously listens on the Sender Queue
* addtoReceiveQ()  - Adds Http messages to the receive Queue
* processRxMessage() - Processes the received Http Message from the server.
* sendFiles() - Sends the files in a package to the server using Sockets
* sendQuit() - Sends final message to the Server to indicate the end of messages
* reqPackage() - Generates a Htttp Message for the extract package command from client

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
#include <map>
#include "../Logger/Cpp11-BlockingQueue.h"
#include "../Sockets/Sockets.h"
#include "../Sender/Sender.h"
#include "../HttpMessage/HttpMessage.h"

using namespace std;

class FileInfo {
public:
	string& fileName() {
		return _fileName;
	}
	string& fileNamewithPath() {
		return _fileNamewithPath;
	}
	string& packageName() {
		return _packageName;
	}
	string& moduleName() {
		return _moduleName;
	}
private:
	string _fileName;
	string _fileNamewithPath;
	string _packageName;
	string _moduleName;
};

/////////////////////////////////////////////////////////////////////
// Client class
// - was created as a class so more than one instance could be 
//   run on child thread
//
class Client
{
public:
	void startSender(std::string address, size_t port, Sender& sndr);
	void addtoReceiveQ(HttpMessage);
	void processRxMessage();
	void sendFiles(BlockingQueue<FileInfo>& fileInfoQueue, int numberOfFiles);
	void sendQuit();
	bool createPackage(string moduleName, string packageName);
	bool closePackage(string packageName);
	bool addDependency(string packageName, vector<string> dependablePackages);
	bool getPackagesListfromRepo();
	bool getFiles(string packageName);
private:
	
	BlockingQueue <HttpMessage> sendQ;
	BlockingQueue <HttpMessage> receiveQ;
};





#endif