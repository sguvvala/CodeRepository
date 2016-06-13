#ifndef STRINGCLIENT_H
#define STRINGCLIENT_H
//
#include <string>
#include <iostream>
#include <map>
#include "../Logger/Cpp11-BlockingQueue.h"
#include "../Sockets/Sockets.h"
#include "../Sender/Sender.h"

using namespace std;
/////////////////////////////////////////////////////////////////////
// StringClient class
// - was created as a class so more than one instance could be 
//   run on child thread
//
class StringClient
{
public:
	void execute(const size_t TimeBetweenMessages, const size_t NumMessages);
	void startSender(std::string address, size_t port, Sender& sndr);
	void addtoReceiveQ(std::string);
	void processRxMessage();
	void sendFiles(BlockingQueue<string> files, BlockingQueue<string> fileNames, int numberOfFiles);
	std::string genMessage(std::string fromURL, std::string toURL, std::string reqType, std::string fileSize, std::string fileName, std::string filePath);
	void sendQuit();
private:
	BlockingQueue <std::map<std::string, std::string>> sendQ;
	BlockingQueue <std::string> receiveQ;
	

};





#endif