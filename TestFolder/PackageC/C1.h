#ifndef ASTNODE_H
#define ASTNODE_H
/////////////////////////////////////////////////////////////////////////////
// ASNode.h		 - Provides node for the ASTree							   //
//  ver 1.0                                                                //
//  Language:     C++, Visual Studio 2105								   //
//  Application:  Code Parser with Abstract Syntax Tree					   //
//				  CSE687 - Object Oriented Design						   //
//  Source:      Jim Fawcett, Syracuse University, CST 4-187			   //
//               jfawcett@twcny.rr.com									   //
//  Author:      Vamsi Krishna Kodali, Syracuse University,				   //
//               vkodali@syr.edu.										   //
/////////////////////////////////////////////////////////////////////////////
/*
Package Operations:
===================
This package povides node structure for the Abstract Syntax Tree

Public Methods
==============
No public methods in this header file

Required Files:
===============
ASTree.h, ASTNode.h, ASTNode.cpp, ASTree.cpp

Build Command:
==============
devenv ASTree.sln /rebuild debug

Maintenance History:
====================
ver 1.0 : 14 Mar 16
- first release
*/
#include<vector>
#include <sstream>

struct element
{
	std::string type;
	std::string name;
	size_t startLineCount = 0;
	size_t endLineCount = 0;
	size_t complexity = -1;
	std::string show()
	{
		std::ostringstream temp;
		temp << "(";
		temp << type;
		temp << ", ";
		temp << name;
		temp << ", ";
		temp << startLineCount;
		temp << ", ";
		temp << endLineCount;
		temp << ")";
		return temp.str();
	}
	std::vector<element*> _children;
};
#endif
