#include <string>
#include <sstream>
#include <vector>

#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <dirent.h>

using namespace std;

// Useful utility functions that I've collected or created over time

// http://stackoverflow.com/a/236803/1693087
// Split 's' based on 'delim'
vector<string> split(const string &s, const char delim)
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

// http://stackoverflow.com/a/3418285/1693087
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

// http://stackoverflow.com/a/4980849/1693087
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
}

// http://stackoverflow.com/a/145309/1693087
// Return current directory (where this code is running)
string getCurrentDirectory()
{
	char currentPath[FILENAME_MAX];
	return getcwd(currentPath, sizeof(currentPath));
}

// http://stackoverflow.com/a/4204758/1693087
// Return the contents (files and directories) of a given directory
vector<string> getDirectoryContents(const string& path, bool showHidden = false)
{
	vector<string> entries;
	string cur(".");
	string up("..");
	DIR* d;
	struct dirent* dir;
	d = opendir(path.c_str());
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
			// Ignore "." and ".."
			if (cur.compare(dir->d_name) == 0 || up.compare(dir->d_name) == 0)
			{
				continue;
			}
			// Ignore files starting with "." if not showing hidden files
			if (dir->d_name[0] == '.' && !showHidden)
			{
				continue;
			}
			entries.push_back(dir->d_name);
		}

		closedir(d);
	}
	return entries;
}

// http://www.cplusplus.com/forum/windows/10853/#msg50985
// Return the size (in bytes) of the given file
long getFileSize(const string& path)
{
	struct stat filestatus;
	stat(path.c_str(), &filestatus);
	return filestatus.st_size;
}

// http://codereview.stackexchange.com/a/22907
// Read and return all of the bytes in a file
vector<char> readAllBytes(const string& filename)
{
    ifstream ifs(filename.c_str(), ios::binary|ios::ate);
    ifstream::pos_type pos = ifs.tellg();

    vector<char> result(pos);

    ifs.seekg(0, ios::beg);
    ifs.read(&result[0], pos);

    return result;
}