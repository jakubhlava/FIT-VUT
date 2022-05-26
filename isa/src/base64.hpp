#include <string>
#include <iostream>

#ifndef __BASE64_H__
#define __BASE64_H__

using namespace std;

const string symboltable = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

string base64encode(string input);

#endif