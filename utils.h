#include <string>
#include <sstream>
#include <vector>

using namespace std;

// http://stackoverflow.com/questions/236129/splitting-a-string-in-c/236803#236803
vector<string> split(const string &s, char delim)
{
    vector<string> tokens;
    stringstream ss(s);
    string token;

    while (getline(ss, token, delim))
    {
        tokens.push_back(token);
    }

    return tokens;
}