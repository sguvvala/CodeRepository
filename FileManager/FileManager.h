#ifndef FILEMANAGER_H
#define FILEMANAGER_H
/////////////////////////////////////////////////////////////////////
// FileManager.h-  Class to provide traversal through directory    //
// and return filename											   //
// ver 1.0 , 03/13/2016.                                           //
// Language:    C++, Visual Studio 2015                            //
// Application: CodeParser, CSE687 - Object Oriented Design        //
//  Platform:   Macbook Pro										   //
//  Author:     Sruthi Guvvala, MS Computer Science                //
//              Syracuse University                                //
// Source:      Jim Fawcett, Syracuse University, CST 4-187        //
//              jfawcett@twcny.rr.com                              //
/////////////////////////////////////////////////////////////////////
/* Package Operations :
*------------------ -
* This package provides a public FileManager class for traversing through
* given path and returns all the files with a specific pattern
* along with full path.
* 
* Build Process :
*--------------
* Required Files : FileManager.h, FileManager.cpp, FileSystem.h
*
* Build Command : devenv StringSocketServer.sln /rebuild debug
*
*
* Public Methods
* -------------- -
* FileManager();          -- FileManager Class Constructor
* addDirectory;			  -- Method to add Directory to FileManager
* countComplexity();      -- Returns the files in a directory, which satisfy a pattern
*
* Maintenance History :
*--------------------
* ver 1.0 : 13 Mar 2016
* First Release
*
*/
#include<iostream>
#include<string>
#include<vector>
#include"../FileSystem-Windows/FileSystemDemo/FileSystem.h"


using namespace std;

//FileManager Class
class FileManager {
public:
	FileManager();
	void addDirectory(string);
	vector<string> getFiles(string, vector<string>);
private:
	string directory;
	static vector<string> filesInDirectory;
	vector<string> find(const std::string& , vector<string>);
};
#endif