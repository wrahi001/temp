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
    for (i = 0; i < (int)subWord.size(); i++)
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

bool runTest(char *command)
{
    struct stat stats;
    string testArg = "";
    const char *c;

    if (firstWordIs(command, "test"))
    {
        // example: test -e test.txt
        // get the argument
        testArg = substr(command, 5, 2);

        // strip "test " and the argument from the command to get just the file/directory name
        if (testArg == "-e" || testArg == "-f" || testArg == "-d")
        {
            c = substr2(command, 5 + 3).c_str();
        }
        else
        {
            c = substr2(command, 5).c_str();
        }
    }
    else
    {
        // example: [ -e test.txt ]
        // get the argument
        testArg = substr(command, 2, 2);

        // strip "[ ," " ]," and the argument from the command to get just the file/directory name
        if (testArg == "-e" || testArg == "-f" || testArg == "-d")
        {
            c = substr2(command, 2 + 3).c_str();
        }
        else
        {
            c = substr2(command, 2).c_str();
        }
    }

    // display whether the file/directory exists
    if (stat(c, &stats) == -1)
    {
        cout << "(False)" << endl;
        return false;
    }
    else
    {
        // display whether the file/directory is a file
        if (testArg == "-f")
        {
            if ((stats.st_mode & S_IFMT) != S_IFREG)
            {
                cout << "(False)" << endl;
                return false;
            }
            else
            {
                cout << "(True)" << endl;
                return true;
            }
        }
        // display whether the file/directory is a directory 
        else if (testArg == "-d")
        {
            if ((stats.st_mode & S_IFMT) != S_IFDIR)
            {
                cout << "(False)" << endl;
                return false;
            }
            else
            {
                cout << "(True)" << endl;
                return true;
            }
        }
        else
        {
            cout << "(True)" << endl;
            return true;
        }
    }
}

int main()
{
    Base *command_pointer = new CompositeCommand;
    int e = 0;
    bool ex_result;
    char **analyzed = new char*[256];
    char **analyzed2 = new char*[256];
    bool previous = true;
    bool skip = false;
    bool inPar = false;
    cout << "Welcome to the rshell" << endl;

    char hostname[1024];
    gethostname(hostname, 1024);
    while (true)
    {
        cout << getlogin() << "@";
        cout << hostname;
        cout << "$ ";

        command_pointer->get();
        if (command_pointer->check_if_exit() == "exit")
        {
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
                    inPar = true;
                }
                if (analyzed[i][j] == ';' && j != 0)
                {
                    analyzed[i][j] = '\0';
                    analyzed2[e] = analyzed[i];
                    e++;
                    analyzed2[e] = NULL;
                    ex_result = command_pointer->execute(analyzed2);
                    delete[] analyzed2;
                    analyzed2 = new char*[256];
                    e = 0;
                    skip = true;
                }
                if (analyzed[i][j] == ')')
                {
                    analyzed[i][j] = '\0';
                    inPar = false;
                }
                if (analyzed[i][j] == 'e' && analyzed[i][j + 1] == 'x' && analyzed[i][j + 2] == 'i' && analyzed[i][j + 3] == 't')
                    exit(1);
            }

            if (*analyzed[i] == ';')
            {
                if (previous)
                {
                    if (firstWordIs(*analyzed2, "test") || firstWordIs(*analyzed2, "["))
                    {
                        ex_result = runTest(*analyzed2);
                    }
                    else
                    {
                        ex_result = command_pointer->execute(analyzed2);
                    }
                }
                delete[] analyzed2;
                analyzed2 = new char*[256];
                e = 0;
            }
            else if (*analyzed[i] == '&')
            {
                if (previous)
                {
                    if (firstWordIs(*analyzed2, "test") || firstWordIs(*analyzed2, "["))
                    {
                        ex_result = runTest(*analyzed2);
                    }
                    else
                    {
                        ex_result = command_pointer->execute(analyzed2);
                    }
                }
                delete[] analyzed2;
                analyzed2 = new char*[256];
                e = 0;

                if (inPar)
                    continue;

                if (!ex_result)
                {
                    previous = false;
                }
                else
                {
                    previous = true;
                }
            }
            else if (*analyzed[i] == '|')
            {
                if (previous)
                {
                    if (firstWordIs(*analyzed2, "test") || firstWordIs(*analyzed2, "["))
                    {
                        ex_result = runTest(*analyzed2);
                    }
                    else
                    {
                        ex_result = command_pointer->execute(analyzed2);
                    }
                }
                delete[] analyzed2;
                analyzed2 = new char*[256];
                e = 0;




                if (ex_result)
                    previous = false;
                else if (inPar)
                    continue;
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
            {
                if (firstWordIs(*analyzed2, "test") || firstWordIs(*analyzed2, "["))
                {
                    runTest(*analyzed2);
                }
                else
                {
                    command_pointer->execute(analyzed2);
                }
            }
            delete[] analyzed2;
            analyzed2 = new char*[256];
            e = 0;
            previous = true;
        }
        /*********************************/
    }
    return 0;
}
