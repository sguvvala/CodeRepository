#ifndef TESTSUITE_H
#define TESTSUITE_H
///////////////////////////////////////////////////////////////////////
// TestSuite.h - tests Tokenizer and SemiExp packages                //
// ver 3.3                                                           //
// Language:    C++, Visual Studio 2015                              //
// Application: Parser component, CSE687 - Object Oriented Design    //
// Source:      Jim Fawcett, Syracuse University, CST 4-187          //
//              jfawcett@twcny.rr.com                                //
// Author:      Vamsi Krishna Kodali, Syracuse University,           //
//              vkodali@syr.edu.                                     //
///////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package is designed to test Tokenizer package and SemiExp package and to demonstate
* the requirements of Project#1 of CSE687 - Object Oriented Design being taught in Spring 2016
* Build Process:
* --------------
* Required Files:
*   SemiExpression.h, SemiExpression.cpp, Tokenizer.h, Tokenizer.cpp, itokcollection.h
*   TestSuite.cpp, TestSuite.h
*
* Build Command: devenv Project1.sln /rebuild debug
*
* Maintenance History:
* --------------------
* - first release
*/

#include <iosfwd>
#include <string>
#include <vector>
#include "../SemiExp/SemiExp.h"
namespace Scanner
{
	class TestSuite
	{
	public:
		TestSuite();
		~TestSuite();
		void testTokens();
		void testSemiExp();
		void testIToken();
		void testSetChar();
	};
}
#endif
#define a