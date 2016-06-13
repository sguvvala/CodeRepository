#pragma once
//////////////////////////////////////////////////////////////////////////////////
// HttpMessage.h - Build and parse Http style messages							//
//  ver 1.0                                                                     //
//  Language:          C++, Visual Studio 2105							        //
//  Application:       Dependency-Based Remote Code Repository					//
//				       CSE687 - Object Oriented Design						    //
//  Author:           Sruthi Guvvala, Syracuse University						//
//																			    //
//////////////////////////////////////////////////////////////////////////////////
/*
Module Operations:
==================
Build and parse Http style messages

Public Methods:
=================
* addAttribute() - Adds attribute to the Http Message
  findValue() - Finds attribute and returns the attrubutes in the Http Message
  findAttribute() - Checks if the Attribute is present in the Http Message
  removeAttribute()- Removes an attribute from the Http Message
  attributes() - Retunrns the attribute pointer
  attribString() - Converts the Attribute  to the string 
  attribute() - Returns Attribute
  parseAttribute() - Parses Attribute
  setBody() - Sets body to the Http Message
  getBody() - Gets the Body from the Message
  addBody() - Appends Body to the HTTP Message
  bodyLength() - Returns Length of the Body
  makeMessage() - Makes Http Message


Build Process:
==============
Required files
-	Sockets.h, Sockets.cpp

Build commands
- devenv StringSocketServer.sln

Maintenance History:
====================
ver 1.0 : 3 May 16
- first release

*/
#include <vector>
#include <string>
#include <array>

class HttpMessage
{
public:
  using byte = char;
  using Name = std::string;
  using Value = std::string;
  using Attribute = std::pair<Name, Value>;
  using Attributes = std::vector<Attribute>;
  using Terminator = std::string;
  using Body = std::vector<byte>;

  // message attributes
  void addAttribute(const Attribute& attrib);
  Value findValue(const Name& name);
  size_t findAttribute(const Name& name);
  bool removeAttribute(const Name& name);
  Attributes& attributes();
  static std::string attribString(const Attribute& attrib);
  static Attribute attribute(const Name& name, const Value& value);
  static Attribute parseAttribute(const std::string& src);

  // message body
  void setBody(byte buffer[], size_t Buflen);
  size_t getBody(byte buffer[], size_t& Buflen);
  void addBody(const Body& body);
  void addBody(const std::string& body);
  void addBody(size_t numBytes, byte* pBuffer);
  Body& body();
  size_t bodyLength();

  // construct message
  //static HttpMessage parseHeader(const std::string& src);
  //static HttpMessage parseMessage(const std::string& src);

  //make message
  static HttpMessage makeMessage(size_t n, const std::string& body, const std::string& remoteUrl, const std::string& localUrl);

  // display
  std::string headerString() const;
  std::string bodyString() const;
  std::string toString() const;
  std::string toIndentedHeaderString() const;
  std::string toIndentedBodyString() const;
  std::string toIndentedString() const;

  // cleanup
  void clear();
  static void fillBuffer(byte buffer[], size_t BufSize, byte fill = '\0');

private:
  Attributes attributes_;
  Terminator term_ = "\n";
  Body body_;
};