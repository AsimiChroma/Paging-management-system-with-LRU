#include "string_handler.h"
#include <vector>
#include <string>
#include <iostream>

using namespace std;

// Return a vector of strings
vector<string> split(string str)
{
    vector<string> result;
    string s = "";

    for (int i = 0; i < str.length(); i++)
    {
        if (s != "" && (str[i] == ' ' || str[i] == '\t'))
        {
            result.push_back(s);
            s = "";
        }
        else
        {
            s.push_back(str[i]);
        }
    }

    if (s != "")
    {
        result.push_back(s);
    }

    return result;
}
