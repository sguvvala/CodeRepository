/////////////////////////////////////////////////////////////////////
// FileManager.cpp-  Implementaion of FileManager Class to		   //
// provide traversal through directory	and return filenames	   //
// ver 1.0 , 03/13/2016.                                           //
// Language:    C++, Visual Studio 2015                            //
// Application: CodeParser, CSE687 - Object Oriented Design        //
//  Platform:   Macbook Pro										   //
//  Author:     Sruthi Guvvala, MS Computer Science                //
//              Syracuse University                                //
// Source:      Jim Fawcett, Syracuse University, CST 4-187        //
//              jfawcett@twcny.rr.com                              //
/////////////////////////////////////////////////////////////////////

#include "FileManager.h"
#include <iostream>
#include "../FileSystem-Windows/FileSystemDemo/FileSystem.h"

using namespace FileSystem;
using namespace std;

//FileManager class void constructor
FileManager::FileManager() {

}

//Add directory adds a path to the directory held by FileManager class
void FileManager::addDirectory(string path) {
	directory = path;
}

// getFiles returns the file names matching pattern in the directory
vector<string> FileManager::getFiles(string path, vector<string> patterns_) {
	filesInDirectory = {};
	return find(path, patterns_);
}

vector<string> FileManager::filesInDirectory = {};

// Find method fetches the files matching the pattern in the given directory
vector<string> FileManager::find(const std::string& path, vector<string> patterns_)
{
	std::string fpath = FileSystem::Path::getFullFileSpec(path);
	//std::cout << "\n  ++ " << fpath;
	for (auto patt : patterns_)  // the scope of this for loop was
	{                            // changed to improve efficiency
		std::vector<std::string> files = FileSystem::Directory::getFiles(fpath, patt);
		for (auto f : files)
		{
			string fileName = fpath + "\\" + f;
			filesInDirectory.push_back(fpath+"\\"+f);
		}
	}
	std::vector<std::string> dirs = FileSystem::Directory::getDirectories(fpath);
	for (auto d : dirs)
	{
		if (d == "." || d == "..")
			continue;
		std::string dpath = fpath + "\\" + d;  // here's the fix
		find(dpath, patterns_);
	}
	return filesInDirectory;
}

#ifdef TEST_FILEMANAGER
//main function to test FileManager

int main(int argc, char* argv[]) {
	string a = Directory::getCurrentDirectory();
	cout << a;
	FileManager fileManager;
	vector<string> patterns = { "*.h" };
	vector<string> files = fileManager.getFiles(a, patterns);
	for (size_t i = 0; i < files.size(); i++)
		cout << endl << files[i];
	return 0;
}
#endif