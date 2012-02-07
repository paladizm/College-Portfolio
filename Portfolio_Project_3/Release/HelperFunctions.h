/********************************************************
* Author: Zac Paladino
* Class: CPS 472
* Assignment: 1
* Due Date: Feb/16/2010
* File: HelperFunctions.h
*********************************************************/
#ifndef HELPERFUNCTIONS_H
#define HELPERFUNCTIONS_H
#include <string>
#include <stdlib.h> 
#include <fstream>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <sstream>
using namespace std;
vector<string> MakeTokens(const string& str, const string& delim);
	//This function makes tokens out of the numbers/letters given in the
	//input file and places them into a vector for ease of use.
	//Pre: The numbers/letters must be the ones that you want to be in
	//	   the vector. It takes care of all the whitespace.
	//Post: Return the vector with tokens in it.
string TokenToUpper(char command[]);
	//This function checks to see if the char array has any lowercase
	//letters if it does it makes them uppercase.
	//Pre: There must be a char array of some kind given to the function.
	//Post: Returns the char array as a string and the letters are now uppercase.
int StringToInt(string strconvert);
	//This function makes strings into ints.
	//Pre: The string must be an integer before placing in function.
	//Post: Returns the int from the conversion of the string.
double StringToDub(string strconvert);
	//This function makes strings into ints.
	//Pre: The string must be double before placing in function.
	//Post: Returns the double from the conversion of the string.
#endif