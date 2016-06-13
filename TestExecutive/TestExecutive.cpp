
#include <iostream>
#include <string>
#include "../Utilities/Utilities.h"

using namespace Utilities;
using namespace std;

#define Util StringHelper
// Main function - Demonstrate Requirements
int main(int argc, char* argv[])
{
	cout << "\n  Please see the ReadMe file before running the project \n";

	cout << "\n=============================================  Requirement 3 ======================================================\n";

	cout << "\n  SringServer package acts as Server and operates on a base folder Repositor which acts as a repository ";
	cout << "\n  MetaData for each package is stored in Metadata.xml\n";


	cout << "\n=============================================== Requirements 5 and 6 =============================================\n";

	cout << "\n  Open package is created initially by checkin in three files with the package name OpenPackage";
	cout << "\n  A directory is created in the server reposiitory folder with package name appended with timestamp";
	cout << "\n  ClosedPackage is then created by checkin in two files with the package name OpenPackage";
	cout << "\n\n  Now,one file is sent to Open Package. Since, its state is open, It receives the file";
	cout << "\n  Another file is sent to ClosedPackage. Since its state is closed,another package ";
	cout << "\n  with same name and updated timestamp is created. The file is then sent to this folder ";
	cout << "\n\n  Now a new package is created by sending two files with NewPackage packagename ";
	cout << "\n------------------------------------------------------------------------------------------------------------------\n";
	cout << "\n  Package information can be seen in ServerRepository folder's PackagesStructure.xml";


	cout << "\n================================================== Requirement 7 ==================================================\n";

	cout << "\n  NewPackage is extracted with Dependencies and the extracted files are stored in Solution's DownloadedFiles folder";


	cout << "\n========================================== Requirements  8 and 9 ==================================================\n";

	cout << "\n  Provided HttpMessage Package for message-passing communication system between server and client ";


	cout << "\n=============================================== Requirement 10 ====================================================\n";

	cout << "\n  File transfer is done and post messages are sent with request type as 'checkin' ";

	std::cout << "\n";
	int a;
	cin >> a;
	return 0;
}
