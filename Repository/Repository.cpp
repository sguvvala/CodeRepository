//////////////////////////////////////////////////////////////////////////////////
//  Repository.cpp - Provides methods to process checkin at the server          //
//  ver 1.0                                                                     //
//  Language:          C++, Visual Studio 2105							        //
//  Application:       Dependency-Based Remote Code Repository					//
//				       CSE687 - Object Oriented Design						    //
//  Author:            Sruthi Guvvala, Syracuse University						//
//																			    //
//////////////////////////////////////////////////////////////////////////////////

#include "Repository.h"
#include <fstream>
#include <mutex>
#include "../FileSystem-Windows/FileSystemDemo/FileSystem.h"
#include "../FileManager/FileManager.h"
#include <ctime>



//Constructor
Repository::Repository(string root) : rootDirectory(root) {
	//cout << "Reading XML";
	if (root != ""&&root != " ") {
		xmlFileFullSpeconDisk = rootDirectory + "\\" + "PackagesStructure.xml";
		XmlDocument xmlMessagefromDisk(xmlFileFullSpeconDisk, XmlDocument::file);
		//cout << endl << "Printing XML read from disk: " << xmlMessagefromDisk.toString();
		_packagesInfo = new XmlDocument(xmlFileFullSpeconDisk, XmlDocument::file);
	}
}

//Return rootDirectory
string& Repository::getRootDirectory() {
	return rootDirectory;
}

//Return filepaths for files in package
vector<string> Repository::getPathsforFilesinPackage(string dirctry, bool recursion) {
	string resultDirectory;
	sPtr resultVersion = getLatestPackageVersion(dirctry);
	if (resultVersion) {
		resultDirectory = resultVersion->getAttrib("Directory");
	}
	vector<string> files;
	cout << "\nChecking files in directory:" << resultDirectory;
	if (resultDirectory != ""&&resultDirectory != " ") {
		FileManager fileManager;
		vector<string> patterns = { "*.h", "*.cpp" };
		files = fileManager.getFiles(resultDirectory, patterns);
	}
	if (recursion) {
		vector<string> dependentFiles = getDependentFiles(resultDirectory);
		if (dependentFiles.size() > 0) {
			files.insert(files.end(), dependentFiles.begin(), dependentFiles.end());
		}
	}
	return files;
}




//Returns files dependent  on the directory
vector<string> Repository::getDependentFiles(string dirctry) {
	FileManager fm;
	vector<string> patterns = { "*.h", "*.cpp" };
	string file = dirctry + "\\" + "DependencyInfo.xml";
	cout << "Checking file: " << file;
	XmlDocument xmlDocument(file, XmlDocument::file);
	vector<sPtr> packages = xmlDocument.element("Package").select();
	vector<string> packageNames;
	if (packages.size() > 0) {
		for (auto pckg : packages) {
			string dir = pckg->getAttrib("Name");
			if (dir != ""&&dir != " ") {
				cout << "\nChecking dir" << dir;
				packageNames.push_back(dir);
			}
		}
	}
	vector<string> dependentFiles;
	if (packageNames.size() > 0) {
		for (auto package : packageNames) {
			vector<string> dep = getFilesinVersion(package);
			dependentFiles.insert(dependentFiles.end(), dep.begin(), dep.end());
		}
	}
	return dependentFiles;
}

//Get filepaths for a particular version
vector<string> Repository::getFilesinVersion(string version) {
	vector<sPtr> versionVector = _packagesInfo->element("Version").select();
	string resultDirectory;
	for (auto eachVersion : versionVector) {
		string versionName = eachVersion->getAttrib("Name");
		if (versionName == version) {
			resultDirectory = eachVersion->getAttrib("Directory");
			break;
		}
	}

	vector<string> files;
	if (resultDirectory != ""&&resultDirectory != " ") {
		FileManager fileManager;
		vector<string> patterns = { "*.h", "*.cpp" };
		files = fileManager.getFiles(resultDirectory, patterns);
	}
	return files;
}

//Save a file to disk
bool Repository::saveFile(string fileName, string packageName, char* memblock, size_t fileSize) {
	string originalFileName = fileName;
	sPtr latestPckg = getLatestPackageVersion(packageName);
	if (latestPckg) {
		string status = latestPckg->getAttrib("Status");
		if (status == "Closed") {
			createPackageOnDisk(packageName);
			latestPckg = getLatestPackageVersion(packageName);
		}
	}
	if (!latestPckg) {
		createPackageOnDisk(packageName);
		latestPckg = getLatestPackageVersion(packageName);
	}
	string dir = latestPckg->getAttrib("Directory");
	if (dir != " ") {

		/*string path = FileSystem::Path::getPath(fileName);
		string kpath = "\\" + path;
		path = dir + kpath;
		path.pop_back();*/

		string path = dir;

		fileName = "\\" + fileName;
		fileName = dir + fileName;

		//cout << endl << "Creating path" << path;
		cout << endl << "Creating file" << fileName;

		std::wstring stemp = std::wstring(path.begin(), path.end());
		LPCWSTR sw = stemp.c_str();
		::SHCreateDirectoryEx(NULL, sw, NULL);

		std::ofstream file(fileName, std::ios::ate | std::ios::binary);
		if (file.is_open()) {
			file.write(memblock, fileSize);
			file.close();
		}
		addFiletoMetadata(path, originalFileName, fileName);
		return true;
	}
	return false;
}

//Add file info to Package Metadata
bool Repository::addFiletoMetadata(string path, string fileName, string filePath) {
	string xmlFileonDisk = path + "\\Metadata.xml";
	XmlDocument metFile(xmlFileonDisk, XmlDocument::file);
	cout << "\nAdding metadata";
	sPtr fileElem = makeTaggedElement("File");
	fileElem->addAttrib("Name", fileName);
	fileElem->addAttrib("Path", filePath);
	sPtr rootElem = metFile.xmlRoot();
	rootElem->addChild(fileElem);
	ofstream xmlFileWrite;
	xmlFileWrite.open(xmlFileonDisk, ios::ate);
	xmlFileWrite << metFile.toString();
	return true;
}

//Convert pointer to package information
PackageDetails Repository::convertsPtrToPackageDetails(sPtr result, string packageName) {
	PackageDetails package;
	if (result != nullptr) {
		if (result->tag() == "Version") {
			package.package() = packageName;
			package.version() = result->getAttrib("Name");
			package.path() = result->getAttrib("Directory");
			package.status() = result->getAttrib("Status");
			return package;
		}
	}
	return package;
}

//Save dependency info to disk
bool Repository::saveDependencyInfotoDisk(string packageName, string dependencyInfoString) {
	sPtr latestPackgeVersion = getLatestPackageVersion(packageName);
	string path = latestPackgeVersion->getAttrib("Directory");
	string xmlFileNameonDisk = path + "\\DependencyInfo.xml";
	XmlDocument dependencyInfoXML(dependencyInfoString);
	vector<string> depPackVector;
	vector<string> depVersionVector;
	std::vector<sPtr> found = dependencyInfoXML.element("Package").select();
	for (auto pElem : found)
	{
		string pckName = pElem->getAttrib("Name");
		if (pckName != ""&&pckName != " ") {
			cout << "\nDependent Package: " << packageName << " found";
			depPackVector.push_back(pckName);
		}
	}
	for (auto depPack : depPackVector)
	{
		sPtr pck = getLatestPackageVersion(depPack);
		string Version = pck->getAttrib("Name");
		if (Version != ""&&Version != " ")
			depVersionVector.push_back(Version);
	}
	
	sPtr root = makeTaggedElement("Dependency");
	for (auto latest : depVersionVector) {
		sPtr latestDpPck = makeTaggedElement("Package");
		latestDpPck->addAttrib("Name", latest);
		root->addChild(latestDpPck);
	}
	XmlDocument depXml(makeDocElement(root));
	if (path != ""&&path != " ") {
		ofstream file;
		//cout << "Writing to file" << xmlFileNameonDisk;
		file.open(xmlFileNameonDisk, ios::ate);
		file << depXml.toString();
		file.close();
	}
	return false;
}

//Get the latest package with the package name
sPtr Repository::getLatestPackageVersion(string packageName) {
	//cout << "\nFinding latest version of package: " << packageName;
	std::vector<sPtr> found = _packagesInfo->element("Package").select();
	PackageDetails package;
	vector<sPtr> resultpckgs;
	//cout << "\nFound package/s";
	if (found.size() > 0)
	{
		for (auto pElem : found)
		{
			if (pElem->getAttrib("Name") == packageName) {
				//cout << "\n Package: " << packageName << " found";
				resultpckgs.push_back(pElem);
			}
		}
	}
	//cout << "\n Taking latest package";
	//cout << "\nResult package size:" << resultpckgs.size();
	sPtr latestPckg = nullptr;
	if (resultpckgs.size() > 0) {
		latestPckg = resultpckgs[resultpckgs.size() - 1];
	}
	found.clear();
	if (latestPckg) {
		//cout << "\n Getting versions";
		found = latestPckg->children();
	}
	if (found.size() > 0)
	{
		//cout << "\nExtracting latest version";
		sPtr result = found[found.size() - 1];
		if (result->tag() == "Version") {
			package.package() = packageName;
			package.version() = result->getAttrib("Name");
			package.path() = result->getAttrib("Directory");
			package.status() = result->getAttrib("Status");
			return result;
		}
	}
	return nullptr;
}

//Get Package Versions with the given Package Name
vector<PackageDetails> Repository::getPackageVersions(string packageName) {
	std::vector<sPtr> found = _packagesInfo->element("Package").select();
	vector<PackageDetails> packages;
	sPtr resultElem;
	if (found.size() > 0)
	{
		for (auto pElem : found)
		{
			string attrib = pElem->getAttrib("Name");
			if (pElem->getAttrib("Name") == packageName) {
				resultElem = pElem;
				break;

			}
		}
	}
	found = resultElem->children();
	if (found.size() > 0)
	{
		for (auto pElem : found)
		{
			if (pElem->tag() == "Version") {
				PackageDetails package;
				package.package() = packageName;
				package.version() = pElem->getAttrib("Name");
				package.path() = pElem->getAttrib("Directory");
				package.status() = pElem->getAttrib("Status");
				packages.push_back(package);
			}
		}
	}
	return packages;
}

//Add packageInfoXML to Repository
bool Repository::addPackageInfoXML(XmlDocument* xmlDocument) {
	_packagesInfo = xmlDocument;
	return true;
}

//Close Package version
bool Repository::closePackageVersion(string packageName, string versionName) {
	sPtr latestPckgVersionPtr = getLatestPackageVersion(packageName);
	if (latestPckgVersionPtr != nullptr) {
		//cout << "Package:" << packageName << ", Status: " << latestPckgVersionPtr->getAttrib("Status");
		latestPckgVersionPtr->getAttrib("Status") = "Closed";
		//cout << "Package:" << packageName << ", Status: " << latestPckgVersionPtr->getAttrib("Status");
		savePackageInfoXMLtoDisk();
		return true;
	}
	return false;
}

//Creates module to disk
bool Repository::createModuleOnDisk(string moduleName) {
	string moduleVersionNumber = "1";
	string path = rootDirectory + "\\" + moduleName + "_" + moduleVersionNumber;
	std::wstring stemp = std::wstring(path.begin(), path.end());
	LPCWSTR sw = stemp.c_str();
	::SHCreateDirectoryEx(NULL, sw, NULL);
	return true;
}

//Save PackageInfoXML to disk
bool Repository::savePackageInfoXMLtoDisk() {
	ofstream xmlFileWrite;
	xmlFileWrite.open(xmlFileFullSpeconDisk, ios::ate);
	xmlFileWrite << _packagesInfo->toString();
	return true;
}

//Returns list of packages in XmlDocument
XmlDocument Repository::getPackagesList() {
	vector<sPtr> packages = _packagesInfo->element("Version").select();
	vector<string> packageNames;
	if (packages.size() > 0) {
		for (auto pckg : packages) {
			string pckgnm = pckg->getAttrib("Name");
			if (pckgnm != ""&& pckgnm != " ") {
				packageNames.push_back(pckgnm);
			}
		}
	}
	sPtr rootElem = makeTaggedElement("Packages");
	for (auto eachpckg : packageNames) {
		sPtr pckgelem = makeTaggedElement("Package");
		pckgelem->addAttrib("Name", eachpckg);
		rootElem->addChild(pckgelem);
	}
	XmlDocument xmlPackagesList(makeDocElement(rootElem));
	return xmlPackagesList;
}

//Get system time as a string
string Repository::getTimeStamp() {
	time_t now = time(0);
	char str[26];
	ctime_s(str, sizeof str, &now);
	std::string timestamp = str;
	std::cout << "\n  timestamp using ctime - " << timestamp << std::endl;

	std::string temp = "";
	for (std::string::iterator i = timestamp.begin(); i < timestamp.end(); ++i) {
		if (*i == ':' || *i == ' ') {
			temp += '_';
			if (*(i + 1) == ' ')
				++i;
		}
		else {
			temp += *i;
		}
	}
	temp.pop_back();
	return temp;
}

//Create a package directory on Disk
bool Repository::createPackageOnDisk(string packageName, string moduleName) {
	//cout << "Getting latest Package";
	sPtr pckgPtr = getLatestPackageVersion(packageName);
	if (pckgPtr) {
		PackageDetails pckg = convertsPtrToPackageDetails(pckgPtr, packageName);
		if (pckg.status() == "Open") {
			cout << "\nPackage: " << packageName << " already exists with Open status";
			return true;
		}
		if (pckg.status() == "Closed") {
			cout << "\nPackage: " << packageName << "exists with closed Status. Creating new version";
		}
	}
	string timestamp = getTimeStamp();
	string path = rootDirectory + "\\" + packageName + "_" + timestamp;
	cout << "\nCreating package: " << packageName << ", with directory: " << path;
	std::wstring stemp = std::wstring(path.begin(), path.end());
	LPCWSTR sw = stemp.c_str();
	::SHCreateDirectoryEx(NULL, sw, NULL);
	sPtr XMLroot = _packagesInfo->xmlRoot();
	sPtr packageElement = makeTaggedElement("Package");
	packageElement->addAttrib("Name", packageName);
	XMLroot->addChild(packageElement);
	string versionName = packageName + "_" + timestamp;
	sPtr versionElement = makeTaggedElement("Version");
	versionElement->addAttrib("Name", versionName);
	versionElement->addAttrib("Directory", path);
	versionElement->addAttrib("Status", "Open");
	packageElement->addChild(versionElement);
	savePackageInfoXMLtoDisk();
	sPtr metRoot = makeTaggedElement("Package Metadata");
	XmlDocument metadataDoc(makeDocElement(metRoot));
	ofstream file;
	string metaDataFileOnDisk = path + "\\Metadata.xml";
	file.open(metaDataFileOnDisk, ios::ate);
	file << metadataDoc.toString();
	return true;
}

//Main method
//Test Stub
#ifdef TEST_REPOSITORY
void main() {
	string thePath = "C:\\Users\\Martin\\Desktop\\TestingFolder";

	string timestamp = __TIMESTAMP__;
	cout << endl << "TimeStamp before concatenation: " << timestamp;
	for (string::iterator i = timestamp.begin(); i < timestamp.end(); ++i) {
		if (*i == ':' || *i == ' ' || *i == '  ')
			timestamp.erase(i);
	}
	cout << endl << "TimeStamp after concatenation: " << timestamp;
	string folderwithDate = thePath + timestamp;
	cout << endl << "Folder along with TimeStamp" << folderwithDate;
	std::wstring stemp = std::wstring(folderwithDate.begin(), folderwithDate.end());
	LPCWSTR sw = stemp.c_str();
	::SHCreateDirectoryEx(NULL, sw, NULL);
	vector<string> folders = FileSystem::Directory::getDirectories("C:\\Users\\Martin\\Desktop\\TestingFolder");
	for (auto folder : folders)
		cout << endl << folder;

	sPtr rootElement = makeTaggedElement("Dependency");
	for (auto folder : folders) {
		if (folder != "."&&folder != "..") {
			sPtr folderElement = makeTaggedElement("Package");
			folderElement->addAttrib("Name", folder);
			rootElement->addChild(folderElement);
			cout << endl << folder;
		}
	}

	XmlDocument xmlFolderDocument(makeDocElement(rootElement));
	std::ofstream file;
	file.open("PackagesDependency.xml", ios::ate);
	file << xmlFolderDocument.toString();
	file.close();


	string xmlString = xmlFolderDocument.toString();
	XmlDocument xml(xmlString);
	XmlDocument xmlInputFile("PackagesDependency.xml", XmlDocument::file);
	vector<sPtr> packages = xml.element("Package").select();
	cout << "No. of Packages found: " << packages.size();

	vector<string> foldersList;
	for (auto package : packages) {
		if (package->getAttrib("Name") != "."&&package->getAttrib("Name") != ".."&&package->getAttrib("Name") != "") {
			cout << "Hello\n";
			foldersList.push_back(package->getAttrib("Name"));
		}
	}
	vector<string> filesList;
	for (auto folder : foldersList) {
		std::vector<std::string> files = FileSystem::Directory::getFiles(thePath + "\\" + folder);
		filesList.insert(filesList.end(), files.begin(), files.end());
	}

	for (auto file : filesList)
		cout << endl << file;






	sPtr XMLroot = makeTaggedElement("Packages");

	sPtr packageElement = makeTaggedElement("Package");
	packageElement->addAttrib("Name", "Package 1");
	XMLroot->addChild(packageElement);


	sPtr versionElement = makeTaggedElement("Version");
	versionElement->addAttrib("Name", "1");
	versionElement->addAttrib("Directory", "Package1_1");
	versionElement->addAttrib("Status", "Closed");
	packageElement->addChild(versionElement);

	versionElement = makeTaggedElement("Version");
	versionElement->addAttrib("Name", "2");
	versionElement->addAttrib("Directory", "Package1_2");
	versionElement->addAttrib("Status", "Open");
	packageElement->addChild(versionElement);

	packageElement = makeTaggedElement("Package");
	packageElement->addAttrib("Name", "Package 2");
	XMLroot->addChild(packageElement);


	versionElement = makeTaggedElement("Version");
	versionElement->addAttrib("Name", "1");
	versionElement->addAttrib("Directory", "Package2_1");
	versionElement->addAttrib("Status", "Open");
	packageElement->addChild(versionElement);



	XmlDocument xmlMessage(makeDocElement(XMLroot));
	//cout << xmlMessage.toString();

	cout << endl;
	/*string fileSpec = "Package XML Structure.xml";
	XmlDocument file(fileSpec, XmlDocument::file);
	XmlDocument* filePointer = &file;*/

	/*Repository repo;
	repo.addPackageInfoXML(&xmlMessage);


	vector<PackageDetails> packages = repo.getPackageVersions("Package 1");
	cout << endl << "Printing all Packages with name: Package 1";
	for (auto packg : packages) {
	packg.print();
	}*/

	cout << endl;

	string xmlInputFileName = "PackagesStructure.xml";
	XmlDocument xmlMessagefromDisk(xmlInputFileName, XmlDocument::file);
	cout << endl << "Printing XML read from disk: " << xmlMessagefromDisk.toString();

	Repository repo2("C:\\Users\\Martin\\Desktop\\OODFolder");
	repo2.addPackageInfoXML(&xmlMessagefromDisk);
	repo2.closePackageVersion("Package 1", "2");
	cout << endl << "Printing XML read from disk after closing module status: " << xmlMessagefromDisk.toString();
	string xmlOutputFileName = "PackagesStructure.xml";

	cout << "Writing file to output: " << xmlOutputFileName;

	ofstream xmlFileWrite;
	xmlFileWrite.open(xmlOutputFileName, ios::ate);
	xmlFileWrite << xmlMessagefromDisk.toString();
	cout << endl;

	/*try
	{
	XmlDocument xmlMessagefromDisk(xmlInputFileName, XmlDocument::file);
	cout << "Printing XML read from disk: " << xmlMessagefromDisk.toString();
	}
	catch (std::exception& ex)
	{
	std::cout << "\n\n  " << ex.what();
	}*/

	cout << endl;

	/*PackageDetails package = repo.getLatestPackageVersion("Package 1");
	cout << endl << "Printing latest package version with name: Package 1";
	package.print();

	cout << endl;

	packages = repo.getPackageVersions("Package 2");
	cout << endl << "Printing all Packages with name: Package 2";
	for (auto packg : packages) {
	packg.print();
	}*/
	cout << endl;
}
#endif