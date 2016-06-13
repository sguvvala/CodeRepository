/////////////////////////////////////////////////////////////////////////////
// ASTree.cpp - Builds Abstract Syntax Tree						    	   //
//  ver 1.0                                                                //
//  Language:     C++, Visual Studio 2105								   //
//  Application:  Code Parser with Abstract Syntax Tree					   //
//				  CSE687 - Object Oriented Design						   //
//  Source:      Jim Fawcett, Syracuse University, CST 4-187			   //
//               jfawcett@twcny.rr.com									   //
//  Author:      Vamsi Krishna Kodali, Syracuse University,				   //
//               vkodali@syr.edu.										   //
/////////////////////////////////////////////////////////////////////////////

//----< test stub >------------------------------------------------------

#ifdef TEST_ASTREE

#include "ASTree.h"
#include "ASTNode.h"
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

int main()
{
	std::cout << "\n  Testing AST and AST NODE";
	std::cout << "\n =====================\n";

	std::cout << "\n  pushing items onto ASTree instance ";
	std::cout << "\n ---------------------------------------------";

	AST *asTree = new AST;
	element* pItem = new element;

	pItem->type = "function";
	pItem->name = "fun1";
	pItem->startLineCount = 33;
	asTree->setRoot(pItem);

	element* pItem2 = new element;
	pItem2->type = "if";
	pItem2->name = "";
	pItem2->startLineCount = 38;
	(pItem->_children).push_back(pItem2);

	element* pItem3 = new element;
	pItem3->type = "for";
	pItem3->name = "";
	pItem3->startLineCount = 40;
	(pItem2->_children).push_back(pItem3);
	
	element* pItem4 = new element;
	pItem4->type = "while";
	pItem4->name = "";
	pItem4->startLineCount = 60;
	(pItem->_children).push_back(pItem4);
	
	
	std::cout << "\n  Walking simulated AST tree:";
	std::cout << "\n -----------------------------";
	asTree->TreeWalk(pItem);
	std::cout << "\n\n";
}

#endif
