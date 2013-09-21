#include <string>
#include <sstream>
#include <vector>

#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

using namespace std;

// Useful utility functions that I've collected or created over time

// http://stackoverflow.com/questions/236129/splitting-a-string-in-c/236803#236803
// Split 's' based on 'delim'
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
// Replace all occurrences of 'from' with 'to' in 'str'
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

// http://stackoverflow.com/questions/4980815/c-determining-if-directory-not-a-file-exists-in-linux/4980833#4980833
// Return true if the given directory path is valid
bool isDir(const string& filename)
{
	struct stat st;
	if ((stat(filename.c_str(), &st) == 0) && (((st.st_mode) & S_IFMT) == S_IFDIR))
	{
		return true;
	}
	return false;
}

// Return true if the given file path is valid
bool isFile(const string& filename)
{
	struct stat st;
	if ((stat(filename.c_str(), &st) == 0) && (((st.st_mode) & S_IFMT) == S_IFREG))
	{
		return true;
	}
	return false;
	// if (stat(filename.c_str(), &st) == 0)
	// 	if (st.st_mode & S_IFREG != 0)
	// 		printf("%s is a file\n", filename.c_str());

}

// http://stackoverflow.com/questions/143174/how-do-i-get-the-directory-that-a-program-is-running-from/145309#145309
// Return current directory
string getCurrentDirectory()
{
	char currentPath[FILENAME_MAX];
	return getcwd(currentPath, sizeof(currentPath));
}