///////////////////////////////////////////////////////////////////////
// TestSuite.cpp - To DEmonstrate the Requirements of CSE687 Proj#1  //
// ver 1.0                                                           //
// Language:    C++, Visual Studio 2015                              //
// Application: Parser component, CSE687 - Object Oriented Design    //
// Author:      Vamsi Krishna Kodali, Syracuse University,           //
//              vkodali@syr.edu.                                     //
///////////////////////////////////////////////////////////////////////

#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <exception>
#include "../SemiExp/SemiExp.h"
#include "../Tokenizer/Tokenizer.h"
#include "TestSuite.h"

using namespace Scanner;
//Test Suite Constructor
TestSuite::TestSuite()
{

}
//testSuite Destructor
TestSuite::~TestSuite()
{

}

//Demonstrates Requirement 4
void TestSuite::testTokens()
{
	std::cout << "\n**************************************************";
	std::cout << "\n*      Demonstrating requirement 4               *";
	std::cout << "\n**************************************************";
	std::cout << "\n============================================================\n";
	std::cout << "                Testing Tokenizer Package	      	";
	std::cout << "\n============================================================\n";
	std::cout << "\n*******Testing Alphanumeric, C Comment, Cpp Comment & Quoted Strings*********\n";
	std::cout << "\n==============Input File=================\n";
	std::cout << std::ifstream("tokens.txt").rdbuf() << '\n';
	std::string fileSpec = "tokens.txt";
	std::ifstream in(fileSpec);
	std::cout << "\n===============Output Tokens================\n";
	if (!in.good())
	{
		std::cout << "\n  can't open " << fileSpec << "\n\n";
		return;
	}
	Toker *toker = new Toker();
	toker->attach(&in);
	do
	{
		std::string tok = toker->getTok();
		if (tok == "\n")
			tok = "newline";
		std::cout << "\n -- " << tok;
	} while (in.good());
	std::cout << "\n\n*******Testing Punctuators and Special single , Double Characters*********\n";
	std::cout << "\n==============Input File=================\n";
	std::cout << std::ifstream("test2.txt").rdbuf() << '\n';
	std::ifstream _in("test2.txt");
	std::cout << "\n===============Output Tokens================\n";
	if (!_in.good())
	{
		std::cout << "\n  can't open " << "../Tokenizer/test2.txt" << "\n\n";
		return;
	}
	Toker* toker2 = new Toker();
	toker2->attach(&_in);
	do
	{
		std::string tok = toker2->getTok();
		if (tok == "\n")
			tok = "newline";
		std::cout << "\n -- " << tok;
	} while (_in.good());
	testSetChar();
}

//Demonstrates setSpecial Characters requirement of requirement 4
void TestSuite::testSetChar()
{
	std::cout << "\n*******Testing setSpecialCharPairs & setSpecialSingleChars functions*********\n";
	std::cout << "\n==============Input File=================\n";
	std::cout << std::ifstream("setSpecial.txt").rdbuf() << '\n';
	std::string fileSpec = "setSpecial.txt";
	std::ifstream in(fileSpec);
	std::cout << "\n===============Output Tokens================\n";
	if (!in.good())
	{
		std::cout << "\n  can't open " << fileSpec << "\n\n";
		return;
	}
	Toker *toker = new Toker();
	toker->attach(&in);
	toker->setSpecialSingleChars("$");
	toker->setSpecialCharPairs("^&");
	std::cout << "\n\n '$' set as special single Character Token";
	std::cout << "\n\n \"^&\" set as special Double Character Token";
	do
	{
		std::string tok = toker->getTok();
		if (tok == "\n")
			tok = "newline";
		std::cout << "\n -- " << tok;
	} while (in.good());
}

//Demonstrates requirements 6, 7, 8
void TestSuite::testSemiExp()
{
	std::cout << "\n\n**************************************************";
	std::cout << "\n*      Demonstrating requirements    6           *";
	std::cout << "\n**************************************************";
	std::cout << "\n\nThe get function SemiExp.cpp implements the requirement 6";
	std::cout << "\n\n**************************************************";
	std::cout << "\n*      Demonstrating requirements    7,8         *";
	std::cout << "\n**************************************************";
	std::cout << "\n============================================================\n";
	std::cout << "                Testing SemiExp Package	      	";
	std::cout << "\n============================================================\n";
	std::cout << "\n==============Input File=================\n";
	std::cout << std::ifstream("test.txt").rdbuf() << '\n';
	std::string fileSpec = "test.txt";
	std::ifstream in(fileSpec);
	std::cout << "\n===============Output Semi Expressions================\n";
	if (!in.good())
	{
		std::cout << "\n  can't open " << fileSpec << "\n\n";
		return;
	}
	Toker toker;
	toker.attach(&in);
	SemiExp semi(&toker);
	while (semi.get())
	{
		std::cout << "\n\n  -- semiExpression --\n";
		std::cout << semi.show(true);
	}
	std::cout << "\n\n";
	/*
	May have collected tokens, but reached end of stream
	before finding SemiExp terminator.
	*/
	if (semi.length() > 0)
	{
		std::cout << "\n  -- semiExpression --\n";
		std::cout << semi.show();
		std::cout << "\n\n";
	}
}

//Demonstrates requirement 9
void TestSuite::testIToken()
{
	std::cout << "\n**************************************************";
	std::cout << "\n*         Demonstrating requirement 9            *";
	std::cout << "\n**************************************************";
	std::cout << "\n\n From the lines 37 and 43 of SemiExp.h it can be inferred that \n\t itokencollection interface is implented in the proj";
	std::cout << "\n\n All below SemiExpressions are Retrieved using get function and displayed using show() function";
	std::cout << "\n==============Input File=================\n";
	std::cout << std::ifstream("testIToken.txt").rdbuf() << '\n';
	std::cout << "\nhello";
	std::string fileSpec = "testIToken.txt";
	std::ifstream in(fileSpec);
	std::cout << "\n===============Output================\n";
	if (!in.good())
	{
		std::cout << "\n  can't open " << fileSpec << "\n\n";
		return;
	}
	Toker toker;
	toker.attach(&in);
	SemiExp semi(&toker);
	std::cout << "\n Initial output";
	while (semi.get())
		std::cout << "\n\n  -- semiExpression --\n" << semi.show() << "\n";
	std::cout << "\n\n";
	if (semi.length() > 0)
	{
		std::cout << "\n  -- semiExpression --\n" << semi.show() << "\n\n";
	}
	Toker toker2;
	std::ifstream in2(fileSpec);
	toker2.attach(&in2);
	SemiExp semi2(&toker2);
	semi2.get();
	std::cout << "\n  semiExpression Length : " << semi2.length() << "\n\n";
	std::cout << "\n  Using Operator [] with input 4:" << semi2[4] << "\n\n";
	std::cout << "\n  The position of Token \"#\" using function find :" << semi2.find("#") << "\n\n";
	semi2.push_back("This is ADDED token");
	std::cout << "\n -- semiExpression after adding token using push_back function--\n" << semi2.show() << "\n\n";
	semi2.trimFront();
	std::cout << "\n  --semiExpression trimFront() function--\n"<<semi2.show() <<"\n\n";
	semi2.remove(1);
	std::cout << "\n  --semiExpression after using remove function at position 1--\n" << semi2.show() << "\n\n"; 
	semi2.remove("#");
	std::cout << "\n  --semiExpression after removing # using remove function--\n" << semi2.show() << "\n\n";
	semi2.toLower();
	std::cout << "\n  --semiExpression after using toLower function--\n" << semi2.show() << "\n\n";
	semi2.clear();
	std::cout << "\n  --semiExpression after using function clear--\n" << semi2.show() << "\n\n";
}

//Demonstrates all the requirements of project1
int main()
{
	std::cout << "\n**************************************************";
	std::cout << "\n*         Demonstrating requirement 3            *";
	std::cout << "\n**************************************************";
	std::cout << "\n\nAll the required packages are submitted with this project\n\n";
	TestSuite test;// = new TestSuite();
	test.testTokens();
	std::cout << "\n**************************************************";
	std::cout << "\n*         Demonstrating requirement 5            *";
	std::cout << "\n**************************************************";
	std::cout << "\n==============Input File=================\n";
	std::cout << std::ifstream("test.txt").rdbuf() << '\n';
	std::string fileSpec = "test.txt";
	std::ifstream in(fileSpec);
	std::cout << "\n===============Output================\n";
	if (!in.good())
	{
		std::cout << "\n  can't open " << fileSpec << "\n\n";
		return 1;
	}
	Toker toker;
	toker.attach(&in);
	std::cout << "\n Calling getTok() method of Toker class for first time";
	std::cout << "\n--" << toker.getTok()<<"\n";
	std::cout << "\n Calling getTok() method of Toker class for second time";
	std::cout << "\n--" << toker.getTok()<<"\n";
	std::cout << "\n Calling getTok() method of Toker class for Third time";
	std::cout << "\n--" << toker.getTok()<<"\n";
	test.testSemiExp();
	test.testIToken();
	return 0;
}

