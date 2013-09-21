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

// http://stackoverflow.com/questions/3418231/replace-part-of-a-string-with-another-string/3418285#3418285
void replaceAll(string& str, const string& from, const string& to)
{
    if (from.empty())
        return;

    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != string::npos)
    {
        str.replace(start_pos, from.length(), to);
        // In case 'to' contains 'from', like replacing 'x' with 'yx'
        start_pos += to.length();
    }
}