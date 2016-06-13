#ifndef REPOSITORY_H
#define REPOSITORY_H
//////////////////////////////////////////////////////////////////////////////////
//  Repository.h - Provides methods to process checkin at the server            //
//  ver 1.0                                                                     //
//  Language:          C++, Visual Studio 2105							        //
//  Application:       Dependency-Based Remote Code Repository					//
//				       CSE687 - Object Oriented Design						    //
//  Author:            Sruthi Guvvala, Syracuse University						//
//																			    //
//////////////////////////////////////////////////////////////////////////////////
/*
Module Operations:
==================
Provides methods to process checkin at the server

Public Methods:
=================
* getDependentFiles() - Return list of dependent files as string
* getPathsforFilesinPackage() - Get filenames paths for files in package
* getPackagesList() - Get list of packages
* getRootDirectory() - Return root directory of package
* saveFile()  - Save a file to disk
* getLatestPackageVersion() - return latest version of a package
* getPackageVersions() - Get versions of a package
* addPackageInfoXML()  - add package info XML file to repository
* savePackageInfoXMLtoDisk() - Save pacakge info xml file to disk
* closePackageVersion() - close package 
* createModuleOnDisk() - create a module on disk
* createPackageOnDisk() - create a package on disk
* getTimeStamp() - return system timestamp
* convertsPtrToPackageDetails() - converts abstrat XML element pointer to package
* saveDependencyInfotoDisk() - save package info to disk
* getFilesinVersion() - Returns   list of files in a version
* addFiletoMetadata() - add File Info Metadata to XML

Build Process:
==============
Required files
-	XmlDocument.h, XmlDocument.cpp

Build commands
- devenv StringSocketServer.sln

Maintenance History:
====================
ver 1.0 : 3 May 16
- first release

*/


#include <iostream>
#include <string>
#include <shlwapi.h> 
#include <shlobj.h> 
#include "../XmlDocument/XmlDocument.h"

#pragma comment(lib, "shlwapi")
#pragma comment(lib, "shell32")

using namespace std;
using namespace XmlProcessing;
using sPtr = shared_ptr<AbstractXmlElement>;

//Container for Holding package info - Holds latest package info
class PackageDetails {
private:
	string _package;
	string _version;
	string _path;
	string _status;
public:
	//returns package name
	string& package() {
		return _package;
	}
	//returns version name
	string& version() {
		return _version;
	}
	//returns path of package
	string& path() {
		return _path;
	}
	//returns status of package
	string& status() {
		return _status;
	}
	//print package details
	void print() {
		cout << endl << "Package: " << _package << ", Version: " << _version << ", Path: " << _path << ", Status: " << _status;
	}

};

//Repository Class
class Repository {
private:
	XmlDocument* _packagesInfo;
	string rootDirectory;
	string xmlFileFullSpeconDisk;
	string packageIdentifier;
public:
	vector<string> getDependentFiles(string dirctry);
	vector<string> getPathsforFilesinPackage(string dirctry, bool recursion=true);
	XmlDocument getPackagesList();
	Repository(string root);
	string& getRootDirectory();
	bool saveFile(string fileName, string packageName, char* memblock, size_t size);
	sPtr getLatestPackageVersion(string packageName);
	vector<PackageDetails> getPackageVersions(string packageName);
	bool addPackageInfoXML(XmlDocument*);
	bool savePackageInfoXMLtoDisk();
	bool closePackageVersion(string packageName, string versionName=" ");
	bool createModuleOnDisk(string moduleName);
	bool createPackageOnDisk(string packageName, string moduleName = " ");
	string getTimeStamp();
	PackageDetails convertsPtrToPackageDetails(sPtr result, string packageName);
	bool saveDependencyInfotoDisk(string packageName, string dependencyInfoString);
	vector<string> getFilesinVersion(string version);
	bool addFiletoMetadata(string path, string fileName, string filePath);
};
#endif