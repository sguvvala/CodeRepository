//////////////////////////////////////////////////////////////////////////////////
//  Receiver.cpp - Provides methods required for receiving meesages from a peer //
//  ver 1.0                                                                     //
//  Language:          C++, Visual Studio 2105							        //
//  Application:       Dependency-Based Remote Code Repository					//
//				       CSE687 - Object Oriented Design						    //
//  Author:            Sruthi Guvvala, Syracuse University						//
//																			    //
//////////////////////////////////////////////////////////////////////////////////

#include "Receiver.h"
#include "../Sockets/Sockets.h"

//Test Stub
#ifdef TEST_RECEIVER


int main()
{
	std::cout << "\n  Demonstrating Receiver";
	std::cout << "\n ====================================";
	Receiver rx(8081, Socket::IP6);
	ServerHandler cp(&c1);
	rx.start(cp);
	Sender sndr;
	string dir = "D:\\Spring16\\OOD\\Assignments\\Project1\\TestSuite\\";
	string packageName = "TestSuite";
	//string dir = "..\\FileManager";
	FileManager fileManager;
	vector<string> patterns = { "*.h", "*.cpp" };
	vector<string> files = fileManager.getFiles(dir, patterns);
	cout << "\nPrinting files";
	cout << "\nFiles Size : " << files.size();

	BlockingQueue<string> filesQueue;
	BlockingQueue<string> fileNamesQueue;
	for (size_t i = 0; i < files.size(); i++) {

		string fileConcise = getFileConciseFullSpec(files[i], dir);
		filesQueue.enQ(files[i]);
		fileNamesQueue.enQ(fileConcise);
		cout << endl << files[i];
		cout << endl << fileConcise;
	}

	c1.startSender("localhost", 8080, sndr);
	c1.sendFiles(filesQueue, fileNamesQueue, files.size(), packageName);
	c1.reqPackage("SemiExp");
	c1.sendQuit();

	std::thread t1(
		[&]() {

		c1.processRxMessage();
	});
}

#endif

