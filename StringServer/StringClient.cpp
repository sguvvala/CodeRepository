//////////////////////////////////////////////////////////////////////////////////
//  StringClient.cpp - Provides methods to interact with the server				//
//  ver 1.0                                                                     //
//  Language:          C++, Visual Studio 2105							        //
//  Application:       Dependency-Based Remote Code Repository					//
//				       CSE687 - Object Oriented Design						    //
//  Source:            Jim Fawcett, Syracuse University, CST 4-187			    //
//                     jfawcett@twcny.rr.com									//
//  Author:            Martin R Borugadda, Syracuse University,					//
//                     mborugad@syr.edu.									    //
//////////////////////////////////////////////////////////////////////////////////

#include <string>
#include <iostream>
#include <thread>
#include <mutex>
#include <fstream>
#include <map>
#include <cctype>
#include "StringClient.h"
#include "../Sockets/Sockets.h"
#include "../Logger/Logger.h"
#include "../Utilities/Utilities.h"
#include "../Sender/Sender.h"
#include "../Receiver//Receiver.h"
#include "../XmlElement/XmlElement.h"
#include "../XmlDocument/XmlDocument.h"
#include "../FileManager/FileManager.h"
#include "../FileSystem-Windows/FileSystemDemo/FileSystem.h"
#include "../XmlElement/XmlElement.h"
#include "../XmlDocument/XmlDocument.h"
#include "../XmlParser/XmlParser.h"
#include <shlwapi.h> 
#include <shlobj.h> 

#pragma comment(lib, "shlwapi")
#pragma comment(lib, "shell32")


using Show = StaticLogger<1>;
using namespace Utilities;
using namespace std;
using namespace XmlProcessing;
using sPtr = std::shared_ptr<AbstractXmlElement>;

/////////////////////////////////////////////////////////////////////
// ClientCounter creates a sequential number for each client
//
class ClientCounter
{
public:
	ClientCounter() { ++clientCount; }
	size_t count() { return clientCount; }
private:
	static size_t clientCount;
};

size_t ClientCounter::clientCount = 0;

//Handles the connections and messaes that are sent by the server

class ServerHandler
{
private:
	Client* clnt;
	vector<string> packagesInRepository;
	BlockingQueue<vector<string>> repositoryPackagesQueue;
	string downloadsPath;
public:
	bool setDownloadsPath(string path);
	vector<string> getPackagesInRepository();
	BlockingQueue<vector<string>> getRepositoryPackagesQueue();
	bool displayPackageList(HttpMessage msg);
	bool readFile(const std::string& filename, size_t size, Socket& socket_);
	HttpMessage readMessage(Socket& socket);
	ServerHandler(Client* cl) { clnt = cl; }
	void operator()(Socket& socket_);
};

//Gets the packages from the repository
vector<string> ServerHandler::getPackagesInRepository() {
	return repositoryPackagesQueue.deQ();
	//return packagesInRepository;
}

//Gets Repository packages in a queue
BlockingQueue<vector<string>> ServerHandler::getRepositoryPackagesQueue() {
	return repositoryPackagesQueue;
}

//operator () overridding to handle the receive messages from the server

void ServerHandler::operator()(Socket& socket_)
{
	while (true) {
		//std::cout << "\nStarting Client Handler:";
		HttpMessage msg = readMessage(socket_);
		std::string value = msg.toIndentedString();
		value.erase(std::find_if(value.rbegin(), value.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), value.end());
		if (value == "")
			continue;
		//Show::write("\n\n  Received Http message:\n" + msg.toIndentedString());
		//std::string reqType = msg.findValue("reqtype");
		//if (connectionClosed_ || msg.findValue("reqtype") == "quit") {
		if (msg.findValue("reqtype") == "quit") {
			Show::write("\n\nServerHandler thread is terminating");
			clnt->addtoReceiveQ(msg);
			break;
		}
		if (msg.findValue("reqtype") == "packageslist") {
			clnt->addtoReceiveQ(msg);
			break;
		}
		//if we want to shut down the server then we will also have to close this client handler and enque a shut down message to server
		if (msg.findValue("Command") == "Shutdown Server") {
			//srvr->addtoReceiveQ.enQ(msg);
			break;
		}
		//srvr->addtoReceiveQ.enQ(msg);
	}
}

//sends the files over channel to the seervr using sockets

bool ServerHandler::displayPackageList(HttpMessage msg) {
	string packagesXMLstring = msg.bodyString();
	XmlDocument packagesXMLdoc(packagesXMLstring);
	vector<sPtr> pckgElemVctr = packagesXMLdoc.element("Package").select();
	vector<string> packages;
	if (pckgElemVctr.size() > 0) {
		for (auto pckg : pckgElemVctr) {
			string packageName = pckg->getAttrib("Name");
			if (packageName != ""&&packageName != " ") {
				packages.push_back(packageName);
			}
		}
	}
	cout << "\nThe packages in repository are:\n" << endl;
	if (packages.size() > 0) {
		for (auto packge : packages) {
			cout << packge << endl;
		}
	}
	packagesInRepository = packages;
	repositoryPackagesQueue.enQ(packages);
	return true;
}

// Reads Message  from a socket
HttpMessage ServerHandler::readMessage(Socket& socket) {
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
		return msg;
	}
	cout << "\nReceived message from server:\n";
	cout << msg.toString();
	std::string filename = msg.findValue("fileName");
	std::string reqType = msg.findValue("reqtype");
	cout << "\n\n";
	if (reqType == "checkin") {
		size_t contentSize;
		std::string sizeString = msg.findValue("fileSize");
		if (sizeString != "") {
			contentSize = Utilities::Converter<size_t>::toValue(sizeString);
		}
		readFile(filename, contentSize, socket);
	}
	else if (reqType == "packageslist") {
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
		displayPackageList(msg);
		return msg;
	}
	return msg;
}

// Sets the Download path for the extracted files at the client end
bool ServerHandler::setDownloadsPath(string path) {
	downloadsPath = path;
	return true;
}

//Reads the files that that are to be sent from the client
bool ServerHandler::readFile(const std::string& filename, size_t size, Socket& socket_) {
	std::streampos fileSize = size;
	std::string fileName = filename;
	string path = FileSystem::Path::getPath(fileName);
	string kpath = "\\" + path;
	path = downloadsPath + kpath;
	path.pop_back();
	fileName = "\\" + fileName;
	fileName = downloadsPath + fileName;
	//cout << endl << "Creating path: " << path;
	cout << endl << "Creating file: " << fileName;
	std::wstring stemp = std::wstring(path.begin(), path.end());
	LPCWSTR sw = stemp.c_str();
	::SHCreateDirectoryEx(NULL, sw, NULL);
	FileSystem::File WriteFile(fileName);
	WriteFile.open(FileSystem::File::out, FileSystem::File::binary);
	if (!WriteFile.isGood()) {
		std::cout << "  Error while opening output file" << std::endl;
		return false; // or return;
	}
	const int BufferLength = 1024;
	char buffer[BufferLength];
	size_t bytesToRead;
	while (true) {
		if (fileSize > BufferLength)
			bytesToRead = BufferLength;
		else
			bytesToRead = (size_t) fileSize;

		socket_.recv(bytesToRead, buffer);
		WriteFile.putBuffer(bytesToRead, buffer);
		if (fileSize < BufferLength) {
			break;
		}
		fileSize -= BufferLength;
	}
	WriteFile.close();
	return true;
}

//Sends the files to the Server
void Client::sendFiles(BlockingQueue<FileInfo>& fileInfoQueue, int numberOfFiles)
{
	try
	{
		for (int i = 0; i < numberOfFiles; ++i) {
			FileInfo fileInfo = fileInfoQueue.deQ();
			HttpMessage msg;
			msg = HttpMessage::makeMessage(1, "", "localhost:8080", "localhost:8081");
			msg.addAttribute(HttpMessage::attribute("reqtype", "checkin"));
			msg.addAttribute(HttpMessage::attribute("file", fileInfo.fileNamewithPath()));
			msg.addAttribute(HttpMessage::attribute("fileName", fileInfo.fileName()));
			msg.addAttribute(HttpMessage::attribute("package", fileInfo.packageName()));
			msg.addAttribute(HttpMessage::attribute("module", fileInfo.moduleName()));
			sendQ.enQ(msg);
			//cout << "\nEnquing message:\n" << msg.toString();
		}

	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}
}

//Sends the quit message to the Server
void Client::sendQuit()
{
	try
	{
		HttpMessage msg;
		msg = HttpMessage::makeMessage(1, "", "localhost:8080", "localhost:8081");
		msg.addAttribute(HttpMessage::attribute("reqtype", "quit"));
		sendQ.enQ(msg);
		Show::write("\n");
		Show::write("\nAll done folks");
	}
	catch (std::exception& exc)
	{
		Show::write("\nExeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}
}

//Gets the packages from the Server
bool Client::getPackagesListfromRepo() {
	try
	{
		HttpMessage msg;
		msg = HttpMessage::makeMessage(1, "", "localhost:8080", "localhost:8081");
		msg.addAttribute(HttpMessage::attribute("reqtype", "getpackages"));
		sendQ.enQ(msg);
		return true;
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}
	return false;

}

//Returns files in a package
bool Client::getFiles(string packageName) {
	try
	{
		HttpMessage msg;
		msg = HttpMessage::makeMessage(1, "", "localhost:8080", "localhost:8081");
		msg.addAttribute(HttpMessage::attribute("reqtype", "getfiles"));
		msg.addAttribute(HttpMessage::attribute("packagename", packageName));
		msg.addAttribute(HttpMessage::attribute("directory", packageName));
		sendQ.enQ(msg);
		return true;
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}
	return false;
}

//Starts the Sender
void Client::startSender(std::string address, size_t port, Sender& sndr){
	if (!sndr.connect(address, port))	{
		Show::write("\n Failed to Establish a connection");
		return;}
	std::function<void()> threadProc = [&](){
		while (true){
			HttpMessage hmsg = sendQ.deQ();
			cout << "\nClient sending message to server:\n";
			cout << hmsg.toIndentedString();
			std::string message1 = hmsg.findValue("reqtype");
			if (message1 == "checkin"){
				std::string file1 = hmsg.findValue("file");
				std::string fileName = hmsg.findValue("fileName");
				std::string fileSpec = file1;
				std::ifstream file(fileSpec, std::ios::binary | std::ios::ate | std::ios::in);
				std::streampos fileSize;
				char * memblock = new char;
				if (file.is_open()) {
					fileSize = file.tellg();
					std::cout << "\n File Size :" << fileSize;
					memblock = (char *) new char[(int)fileSize];
					file.seekg(0, std::ios::beg);
					file.read(memblock, fileSize);
					file.close();}
				size_t fileSize2 = (size_t) fileSize;
				std::string s = std::to_string(fileSize2);
				hmsg.addAttribute(HttpMessage::attribute("fileSize", s));
				sndr.sendMessage(hmsg.toString());
				sndr.sendfile((size_t)fileSize, memblock);}
			if (message1 != "checkin"){
				sndr.sendMessage(hmsg.toString());}
		}
	};
	std::thread sendThread(threadProc);
	sendThread.detach();
}


// Adds message to the Receive Q
void Client::addtoReceiveQ(HttpMessage message)
{
	receiveQ.enQ(message);
}

//Processes the Receive Message
void Client::processRxMessage()
{
	while (true)
	{
		HttpMessage msg = receiveQ.deQ();
		//cout << "\nClient recevied message from server: " << msg.toString();
		//Show::write("\n Processing Message :" + msg.toString());
		if (msg.findValue("reqtype") == "quit")
		{
			Show::write("\n All Messages Received");
			break;
		}

	}
}

//Adds Dependency information
bool Client::addDependency(string packageName, vector<string> dependablePackages) {
	try
	{
		sPtr rootElem = makeTaggedElement("Dependency");
		for (auto pckg : dependablePackages) {
			cout << "Dependent files" << pckg;
			sPtr dpPackage = makeTaggedElement("Package");
			dpPackage->addAttrib("Name", pckg);
			rootElem->addChild(dpPackage);
		}
		XmlDocument dependencyXml(makeDocElement(rootElem));
		//cout << "Printing Dependency XML file" << dependencyXml.toString();
		string xmlString = dependencyXml.toString();
		
		HttpMessage msg;
		msg = HttpMessage::makeMessage(1, xmlString, "localhost:8080", "localhost:8081");
		msg.addAttribute(HttpMessage::attribute("reqtype", "dependencycreation"));
		msg.addAttribute(HttpMessage::attribute("packagename", packageName));
		//cout << "\nsent";
		//cout << "XmlString after converting dependency to string:\n" << xmlString;
		//cout << "\nHTTP body is: " << msg.bodyString();
		sendQ.enQ(msg);
		Show::write("\n");
		//cout << "\nsent";
		return true;
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}
	return false;

}

//Creates Package
bool Client::createPackage(string moduleName, string packageName) {
	try
	{
		HttpMessage msg;
		msg = HttpMessage::makeMessage(1, "", "localhost:8080", "localhost:8081");
		msg.addAttribute(HttpMessage::attribute("reqtype", "createpackage"));
		msg.addAttribute(HttpMessage::attribute("modulename", moduleName));
		msg.addAttribute(HttpMessage::attribute("packagename", packageName));
		sendQ.enQ(msg);
		//Show::write("\n");
		return true;
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}
	return false;
}

//Generates message to close the package
bool Client::closePackage(string packageName) {
	try
	{
		HttpMessage msg;
		msg = HttpMessage::makeMessage(1, "", "localhost:8080", "localhost:8081");
		msg.addAttribute(HttpMessage::attribute("reqtype", "closepackage"));
		msg.addAttribute(HttpMessage::attribute("modulename", "module"));
		msg.addAttribute(HttpMessage::attribute("packagename", packageName));
		sendQ.enQ(msg);
		//Show::write("\n");
		return true;
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}
	return false;
}

//Gets the package name if full path is given as input
string getFileConciseFullSpec(string fullName, string directory) {
	string name = FileSystem::Path::getName(fullName);
	return name;
}

//add file info to Blocking Queue for sending
void addFilestoBQ(string dir, BlockingQueue<FileInfo>& fileInfoQueue) {
	FileManager fileManager;
	vector<string> patterns = { "*.h", "*.cpp" };
	vector<string> files = fileManager.getFiles(dir, patterns);
	for (size_t i = 0; i < 3; ++i) {
		FileInfo fileInfo;
		string fileConcise = getFileConciseFullSpec(files[i], dir);
		fileInfo.fileName() = fileConcise;
		fileInfo.fileNamewithPath() = files[i];
		fileInfo.packageName() = "OpenPackage";
		fileInfo.moduleName() = "Module 1";
		fileInfoQueue.enQ(fileInfo);
	}
	for (size_t i = 3; i < 5; ++i) {
		FileInfo fileInfo;
		string fileConcise = getFileConciseFullSpec(files[i], dir);
		fileInfo.fileName() = fileConcise;
		fileInfo.fileNamewithPath() = files[i];
		fileInfo.packageName() = "ClosedPackage";
		fileInfo.moduleName() = "Module 1";
		fileInfoQueue.enQ(fileInfo);
	}
	for (size_t i = 5; i < 6; ++i) {
		FileInfo fileInfo;
		string fileConcise = getFileConciseFullSpec(files[i], dir);
		fileInfo.fileName() = fileConcise;
		fileInfo.fileNamewithPath() = files[i];
		fileInfo.packageName() = "OpenPackage";
		fileInfo.moduleName() = "Module 1";
		fileInfoQueue.enQ(fileInfo);
	}
	for (size_t i = 6; i < 7; ++i) {
		FileInfo fileInfo;
		string fileConcise = getFileConciseFullSpec(files[i], dir);
		fileInfo.fileName() = fileConcise;
		fileInfo.fileNamewithPath() = files[i];
		fileInfo.packageName() = "ClosedPackage";
		fileInfo.moduleName() = "Module 1";
		fileInfoQueue.enQ(fileInfo);
	}
	for (size_t i = 7; i < 9; i++) {
		FileInfo fileInfo;
		string fileConcise = getFileConciseFullSpec(files[i], dir);
		fileInfo.fileName() = fileConcise;
		fileInfo.fileNamewithPath() = files[i];
		fileInfo.packageName() = "NewPackage";
		fileInfo.moduleName() = "Module 1";
		fileInfoQueue.enQ(fileInfo);
	}
}

//Entry point to the Client
int main(int argc, char* argv[]){
	Show::title("Demonstrating Clients");
	Client c1;
	Receiver rx(8081, Socket::IP6);
	ServerHandler cp(&c1);
	rx.start(cp);
	Sender sndr;
	string dir = argv[1];
	BlockingQueue<FileInfo> fileInfoQueue;
	string dl = FileSystem::Path::getFullFileSpec(argv[2]); cout << "\nDownload path is" << dl;
	cp.setDownloadsPath(dl);
	std::thread t1(
		[&]() {
		c1.processRxMessage();
	});
	c1.startSender("localhost", 8080, sndr);
	addFilestoBQ(dir, fileInfoQueue);
	c1.sendFiles(fileInfoQueue, 3);
	c1.sendFiles(fileInfoQueue, 2);
	c1.closePackage("ClosedPackage");
	::Sleep(5000);
	c1.sendFiles(fileInfoQueue, 1);
	c1.sendFiles(fileInfoQueue, 1);
	c1.sendFiles(fileInfoQueue, 2);
	c1.addDependency("NewPackage", { "OpenPackage", "ClosedPackage" });
	c1.getFiles("NewPackage");
	::Sleep(10000);
	c1.sendQuit();
	t1.join(); int a; cin >> a;
	return 0;
}