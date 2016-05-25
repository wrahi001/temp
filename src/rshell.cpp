#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <pwd.h>
#include <stdio.h>
#include "rshell.h"
using namespace std;

bool firstWordIs(char *word, string subWord)
{
	int i;
	for (i = 0; i < subWord.size(); i++)
	{
		if (word[i] != subWord[i])
		return false;
	}
	if (word[i] != '\0')
	{
		return false;
	}
	return true;
}

string substr(char *word, int pos, int len)
{
	string temp = "";

	for (int i = pos; i < pos + len; i++)
	{
		temp += word[i];
	}
	return temp;
}

string substr2(char *word, int pos)
{
	string temp = "";
	int i = pos;

	while (word[i] != '\0')
	{
		temp += word[i];
		i++;
	}
	return temp;
}

bool runTest(char *cmd, int cmdLen) {
	struct stat sb;
	bool statVal = false;
	string testArg = "";
	const char *c;

	if (firstWordIs(cmd, "test")) { // example: test -e test.txt
		// get the argument
		testArg = substr(cmd, 5, 2);

		// strip "test " and the argument from the command to get just the file/directory name
		if (testArg == "-e" || testArg == "-f" || testArg == "-d") {
			c = substr2(cmd, 5 + 3).c_str();
		} else {
			c = substr2(cmd, 5).c_str();
		}
	} else { // example: [ -e test.txt ]
		// get the argument
		testArg = substr(cmd, 2, 2);

		// strip "[ ," " ]," and the argument from the command to get just the file/directory name
		if (testArg == "-e" || testArg == "-f" || testArg == "-d") {
			c = substr2(cmd, 2 + 3).c_str();
		} else {
			c = substr2(cmd, 2).c_str();
		}
	}

	// display whether the file/directory exists
	if (stat(c, &sb) == -1) {
		cout << "(False)" << endl;
		return false;
	} else {
		// display whether the file/directory is a file
		if (testArg == "-f") {
			if ((sb.st_mode & S_IFMT) != S_IFREG) {
				cout << "(False)" << endl;
				return false;
			} else {
				cout << "(True)" << endl;
				return true;
			}
			// display whether the file/directory is a directory
		} else if (testArg == "-d") {
			if ((sb.st_mode & S_IFMT) != S_IFDIR) {
				cout << "(False)" << endl;
				return false;
			} else {
				cout << "(True)" << endl;
				return true;
			}
		} else {
			cout << "(True)" << endl;
			return true;
		}
	}
}

int main()
{
	Base *command_pointer = new CompositeCommand;
	int e = 0;
	bool par = false;
	bool ex_result;
	char **analyzed = new char*[256];
	char **analyzed2 = new char*[256];
	bool previous = true;
	bool skip = false;
	int curCmdPos = 0;
	cout << "Welcome to the rshell" << endl;

	// char hostname[1024];
	// gethostname(hostname, 1024);
	while(true)
	{
		// cout << getlogin() << "@";
		// cout << hostname;
		cout << "$ ";

		command_pointer->get();
		if (command_pointer->check_if_exit() == "exit")
		{
			cout << "Now exiting the rshell" << endl;
			exit(1);
		}
		command_pointer->analyze(analyzed);
		/*********************/
		for (int i = 0; analyzed[i] != NULL; i++)
		{
			for (int j = 0; analyzed[i][j] != '\0'; j++)
			{
				if (analyzed[i][j] == '(')
				{
					for (int m = 0; analyzed[i][m] != '\0'; m++)
					{
						analyzed[i][m] = analyzed[i][m + 1];
					}
				}
				if (analyzed[i][j] == ';' && j != 0)
				{
					analyzed[i][j] = '\0';
					analyzed2[e] = analyzed[i];
					e++;
					analyzed2[e] = NULL;
					ex_result = command_pointer->execute(analyzed2);
					delete [] analyzed2;
					analyzed2 = new char*[256];
					e = 0;
					skip = true;
				}
				if (analyzed[i][j] == ')')
				{
					analyzed[i][j] = '\0';
					par = true;
				}
			}

			if (*analyzed[i] == ';')
			{
				if (firstWordIs(*analyzed2, "test") || firstWordIs(*analyzed2, "["))
				{
					ex_result = runTest(*analyzed2, e + 1);
				}
				else
				{
					ex_result = command_pointer->execute(analyzed2);
				}
				delete [] analyzed2;
				analyzed2 = new char*[256];
				e = 0;
			}
			else if (*analyzed[i] == '&')
			{
				if (previous)
					if (firstWordIs(*analyzed2, "test") || firstWordIs(*analyzed2, "["))
					{
						ex_result = runTest(*analyzed2, e + 1);
					}
					else
					{
						ex_result = command_pointer->execute(analyzed2);
					}
				delete [] analyzed2;
				analyzed2 = new char*[256];
				e = 0;
				if (!ex_result && par)
					break;
				if (!ex_result)
				{
					previous = false;
				}
				else
					previous = true;
			}
			else if (*analyzed[i] == '|')
			{
				if (previous)
					if (firstWordIs(*analyzed2, "test") || firstWordIs(*analyzed2, "["))
					{
						ex_result = runTest(*analyzed2, e);
					}
					else
					{
						ex_result = command_pointer->execute(analyzed2);
					}
				delete [] analyzed2;
				analyzed2 = new char*[256];
				e = 0;

				if (ex_result && par)
					break;
				if (ex_result)
				{
					previous = false;
				}
				else
					previous = true;
			}
			else if (*analyzed[i] == '#')
			{
				break;
			}
			else
			{
				if (!skip)
				{
					analyzed2[e] = analyzed[i];
					e++;
					analyzed2[e] = NULL;
				}
				else
					skip = false;
			}
		}
		if (e > 0)
		{
			if (previous)
				if (firstWordIs(*analyzed2, "test") || firstWordIs(*analyzed2, "["))
				{
					runTest(*analyzed2, e);
				}
				else
				{
					command_pointer->execute(analyzed2);
				}
			delete [] analyzed2;
			analyzed2 = new char*[256];
			e = 0;
			previous = true;
		}
		/*********************************/
	}
	return 0;
}
