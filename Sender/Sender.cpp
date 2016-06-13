//////////////////////////////////////////////////////////////////////////////////
//  Sender.cpp - Provides methods required for sending meesages to a peer       //
//  ver 1.0                                                                     //
//  Language:          C++, Visual Studio 2105							        //
//  Application:       Dependency-Based Remote Code Repository					//
//				       CSE687 - Object Oriented Design						    //
//  Author:            Sruthi Guvvla, Syracuse University						//
//																			    //
//////////////////////////////////////////////////////////////////////////////////

#include "Sender.h"
#include "../Logger/Logger.h"

using Show = StaticLogger<1>;

//Connects to the Server
bool Sender::connect(std::string address, size_t port)
{
	Show::attach(&std::cout);
	Show::start();
	size_t try_count = 0;
	try {
		while (!si.connect(address, port))
		{
			if (try_count >= 10)
			{
				Show::write("\n Try connect reached Max count");
				return false;
			}
			std::string myCountString = Utilities::Converter<size_t>::toString(try_count);
			std::string myPort = Utilities::Converter<size_t>::toString(port);

			Show::write("\n Connecting to :" + address + " port : " + myPort +", Number of retries :" + myCountString + "");
			::Sleep(100);
			try_count++;
		}

	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}

	return true;
}

//Sends Message to the peer
bool Sender::sendMessage(std::string message)
{
	return si.sendString(message);
}

//Sends File from the Peer 
bool Sender::sendfile(size_t bytes, char* buffer)
{
	return si.send(bytes, buffer);
}

#ifdef TEST_SENDER


int main()
{
	std::cout << "\n  Demonstrating Receiver";
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

