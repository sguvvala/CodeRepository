//////////////////////////////////////////////////////////////////////////////////////
//  Server.cpp - Provides methods to interact with the Client and acts as repository//
//  ver 1.0                                                                         //
//  Language:          C++, Visual Studio 2105							            //
//  Application:       Dependency-Based Remote Code Repository				    	//
//				       CSE687 - Object Oriented Design						        //
//  Author:            Sruthi Guvvala, Syracuse University					     	//
//																			        //
//////////////////////////////////////////////////////////////////////////////////////


#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <cctype>
#include "Server.h"
#include "../Repository/Repository.h"
#include "../XmlElement/XmlElement.h"
#include "../XmlDocument/XmlDocument.h"
#include "../XmlParser/XmlParser.h"
#include "../FileManager/FileManager.h"
#include "../Sockets/Sockets.h"
#include "../Logger/Logger.h"
#include "../Utilities/Utilities.h"
#include "../Receiver/Receiver.h"
#include "../Sender/Sender.h"
#include "../FileSystem-Windows/FileSystemDemo/FileSystem.h"
#include <shlwapi.h> 
#include <shlobj.h> 

#pragma comment(lib, "shlwapi")
#pragma comment(lib, "shell32")

using namespace XmlProcessing;

using Show = StaticLogger<1>;
using sPtr = std::shared_ptr<AbstractXmlElement>;

//This class handles all the required functionalities for listening to a client

class ClientHandler
{
private:
	Repository* repo;
	bool connectionClosed_;
	Server* srvr;
public:
	bool initializeRepo(string rootDirectory);
	bool clientRequestedPackagesList();
	ClientHandler(Server* srv) { srvr = srv; }
	void operator()(Socket& socket_);
	std::string findNodeValue(std::string node, XmlDocument* pDoc);
	HttpMessage readMessage(Socket& socket);
	bool readFile(const std::string& filename, const std::string& packageName, size_t fileSize, Socket& socket);
};

//Initializes the repository
bool ClientHandler::initializeRepo(string rootDirectory) {
	repo = new Repository(rootDirectory);
	srvr->addRepository(repo);
	return true;
}

//Reads file from the client
bool ClientHandler::readFile(const std::string& filename, const std::string& packageName, size_t size, Socket& socket_) {
	std::streampos fileSize = size;
	std::string fileName = filename;
	char* memblock = (char *) new char[(int)fileSize];
	socket_.recv((size_t)fileSize, memblock);
	repo->saveFile(fileName, packageName, memblock, (size_t) fileSize);
	return true;
}

//Reads Message from the socket
HttpMessage ClientHandler::readMessage(Socket& socket) {
	HttpMessage msg;
	while (true) {
		std::string attribString = socket.recvString('\n');
		if (attribString.size() > 1) {
			HttpMessage::Attribute attrib = HttpMessage::parseAttribute(attribString);
			msg.addAttribute(attrib);
		}
		else {
			break;
		}
	}
	if (msg.attributes().size() == 0) {
		connectionClosed_ = true;
		return msg;
	}
	cout << "\nReceived message from client:\n" << msg.toString();
	std::string reqType = msg.findValue("reqtype");
	//cout << "\n\nReqType is: " << reqType << "\n\n";
	if (reqType == "checkin") {
		std::string filename = msg.findValue("fileName");
		size_t contentSize;
		std::string sizeString = msg.findValue("fileSize");
		std::string packageName = msg.findValue("package");
		if (sizeString != "") {
			contentSize = Utilities::Converter<size_t>::toValue(sizeString);
		}
		readFile(filename, packageName, contentSize, socket);
	}
	else if (reqType == "dependencycreation") {
		//cout << "Extracting body";
		size_t numBytes = 0;
		size_t pos = msg.findAttribute("content-length");
		if (pos < msg.attributes().size()) {
			numBytes = Utilities::Converter<size_t>::toValue(msg.attributes()[pos].second);
			char* buffer = new char[numBytes + 1];
			socket.recv(numBytes, buffer);
			buffer[numBytes] = '\0';
			std::string msgBody(buffer);
			msg.addBody(msgBody);
			delete[] buffer;
		}
		return msg;
	}
	return msg;
}


//Holds the functionality to be implemented for processing a message from the client
void ClientHandler::operator()(Socket& socket_)
{
	while (true) {
		HttpMessage msg = readMessage(socket_);
		std::string value = msg.toIndentedString();
		value.erase(std::find_if(value.rbegin(), value.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), value.end());
		if (value == "")
			continue;
		Show::write("\n\n  Received Http message:\n" + msg.toIndentedString());
		if (msg.findValue("reqtype") == "getfiles") {
			srvr->addtoReceiveQ(msg);
		}
		if (msg.findValue("reqtype") == "getpackages") {
			clientRequestedPackagesList();
		}
		if (msg.findValue("reqtype") == "dependencycreation") {
			string packageName = msg.findValue("packagename");
			cout << packageName;
			string xml = msg.bodyString();
			//cout << "XMLStringBody is:\n" << msg.bodyString();
			repo->saveDependencyInfotoDisk(packageName, xml);
		}
		if (msg.findValue("reqtype") == "closepackage") {
			string moduleName = msg.findValue("modulename");
			string packageName = msg.findValue("packagename");
			repo->closePackageVersion(packageName);
		}
		if (msg.findValue("reqtype") == "quit") {
			Show::write("\n\n  ClientHandler thread is terminating");
			srvr->addtoReceiveQ(msg);
			break;
		}
		////if we want to shut down the server then we will also have to close this client handler and enque a shut down message to server
		//if (msg.findValue("Command") == "Shutdown Server") {
		//	Show::write("\n\n  clientHandler thread is terminating");
		//	break;
		//}
	}
}

//Returns packages list in the repository
bool ClientHandler::clientRequestedPackagesList() {
	XmlDocument packagesListXML = repo->getPackagesList();
	string packagesListXMLString = packagesListXML.toString();
	try
	{
		HttpMessage msg;
		msg = HttpMessage::makeMessage(1, packagesListXMLString, "localhost:8080", "localhost:8081");
		msg.addAttribute(HttpMessage::attribute("reqtype", "packageslist"));
		srvr->addtoReceiveQ(msg);
		//cout << "\n Enquing message :\n" << msg.toString();
	}

	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}
	return true;
}


//Finds Node in the XML document
std::string ClientHandler::findNodeValue(std::string node, XmlDocument* pDoc)
{
	std::vector<sPtr> found = pDoc->element(node).descendents().select();
	if (found.size() > 0)
	{
		for (auto pElem : found)
		{
			if (pElem->tag() != "")
				std::cout << "\n  found tag: " << pElem->tag();
			else {
				std::cout << "\n  found value: " << pElem->value();
				std::string value = pElem->value();
				value.erase(std::find_if(value.rbegin(), value.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), value.end());
				return value;
			}
		}
	}
	else
		std::cout << "\n  couldn't find descendent elements";
	return "";
}

//Adds message to the Receive Q
void Server::addtoReceiveQ(HttpMessage message)
{
	receiveQ.enQ(message);
}

//Creates New Repository instance
bool Server::addRepository(Repository* re) {
	repo = new Repository(" ");
	repo = re;
	return true;
}

//Processes received Http Message 
void Server::processMessage()
{
	while (true)
	{
		HttpMessage msg = receiveQ.deQ();
		if (msg.findValue("reqtype") == "getfiles")
		{
			processExtractMsg(msg.findValue("packagename"));
		}
		sendQ.enQ(msg);
	}
}

//Processes the extracted Message
void Server::processExtractMsg(std::string packageName)
{
	vector<string> files = repo->getPathsforFilesinPackage(packageName);
	cout << "\nPrinting files";
	cout << "\nFiles Size : " << files.size();

	BlockingQueue<string> filesQueue;
	BlockingQueue<string> fileNamesQueue;
	for (size_t i = 0; i < files.size(); i++) {
		string fileConcise = FileSystem::Path::getName(files[i]);
		filesQueue.enQ(files[i]);
		fileNamesQueue.enQ(fileConcise);
		cout << endl << files[i];
		cout << endl << fileConcise;
	}
	sendFiles(filesQueue, fileNamesQueue, files.size(), packageName);
}

//Sends Files to the Client
void Server::sendFiles(BlockingQueue<std::string> files, BlockingQueue<std::string> fileNames, int numberOfFiles, std::string packageName)
{
	try
	{
		for (int i = 0; i < numberOfFiles; ++i) {
			std::string file = files.deQ();
			std::string fileName = fileNames.deQ();
			string filePath = FileSystem::Path::getName(fileName);
			HttpMessage msg;
			msg = HttpMessage::makeMessage(1, "", "localhost:8081", "localhost:8080");
			msg.addAttribute(HttpMessage::attribute("reqtype", "checkin"));
			msg.addAttribute(HttpMessage::attribute("file", file));
			msg.addAttribute(HttpMessage::attribute("fileName", filePath));
			msg.addAttribute(HttpMessage::attribute("packageName", packageName));
			sendQ.enQ(msg);
		}
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}
}

//Starts the Sender
void Server::startSender(std::string address, size_t port, Sender& sndr){
	if (!sndr.connect(address, port)){
		Show::write("\n Failed to Establish a connection");
		return;
	}
	std::function<void()> threadProc = [&](){
		while (true){
			HttpMessage hmsg = sendQ.deQ();
			std::string message1 = hmsg.findValue("reqtype");
			if (message1 == "checkin"){
				std::string fileName = hmsg.findValue("fileName");
				string file1 = hmsg.findValue("file");
				FileSystem::FileInfo fi(hmsg.findValue("file"));
				hmsg.removeAttribute("file");
				hmsg.removeAttribute("packageName");
				hmsg.addAttribute(HttpMessage::attribute("file", fi.name()));
				size_t fileSize = fi.size();
				std::string sizeString = Utilities::Converter<size_t>::toString(fileSize);
				hmsg.addAttribute(HttpMessage::attribute("fileSize", sizeString));
				cout << "\nServer sending to client\nMessage:\n" << hmsg.toString();
				sndr.sendMessage(hmsg.toString());
				FileSystem::File ReadFile(file1);
				ReadFile.open(FileSystem::File::in, FileSystem::File::binary);
				if (!ReadFile.isGood()) {
					std::cout << "\n  Error while opening input file ";
					return false;
				}
				const int BufferLength = 1024;
				char buffer[BufferLength];
				size_t ReadLength;
				int count = 0;
				while (true) {
					ReadLength = ReadFile.getBuffer(BufferLength, buffer);
					count++;
					sndr.sendfile(ReadLength, buffer);
					if (ReadLength < 1024) {
						ReadFile.close();
						break;
					}
				}
			}
			if (message1 != "checkin") {
				cout << "\nServer sending to client\nMessage:\n" << hmsg.toString();
				sndr.sendMessage(hmsg.toString());
			}
		}
	};
	std::thread sendThread(threadProc);
	sendThread.detach();
}

//----< test stub >--------------------------------------------------

int main(int argc, char* argv[])
{
	Show::attach(&std::cout);
	Show::start();
	Show::title("\nString Server started");
	try
	{
		//SocketSystem ss;
		::Sleep(2000);
		//SocketListener sl(8080, Socket::IP6);
		Server srvr;
		Receiver rx(8080, Socket::IP6);
		ClientHandler cp(&srvr);
		string repoPath=FileSystem::Path::getFullFileSpec(argv[1]);
		cout << "\nInitializing Repository at:" << repoPath;
		
		cp.initializeRepo(repoPath);
		rx.start(cp);

		Sender sndr;
		srvr.startSender("localhost", 8081, sndr);

		srvr.processMessage();
		Show::write("\n --------------------\n  press key to exit: \n --------------------");
		std::cout.flush();
		std::cin.get();
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}
	int a;
	cin >> a;
}