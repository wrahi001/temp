#include <iostream>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;



void die(const char*);

// split the input line into an array
void parse(char *text, char **ca)
{
  while (*text != '\0')
  {    /* if not the end of line ....... */
    while (*text == ' ' || *text == '\t' || *text == '\n')
    *text++ = '\0';  
    *ca++ = text;    
    while (*text != '\0' && *text != ' ' &&
    *text != '\t' && *text != '\n')
    text++;         }
  *ca = '\0';        
}

// execute the command that's been split up into an array
bool execute(char **ca)
{
  pid_t pid = fork();
  int status;

  if (pid < 0)
  {  
    perror("fork failed");
    exit(1);
  }
  else if (pid == 0)
  {    
    if (execvp(*ca, ca) < 0)
    {        perror("execvp failed");
      exit(1);
    }
  }
  else
  {                
    while (waitpid(pid, &status, 0) != pid)        ;
    if (WIFEXITED(status))
    {
      if (WEXITSTATUS(status) != 0)
      {
        return false;
      }
    }
  }
  return true;
}

// for piping two commands
bool execute2(char **ca1, char **ca2) {
  int pipe_d[2];
  pid_t prc;
  int filedes0;
  int filedes1;

  // save the stdin and stdout file descriptors
  filedes0 = dup(0);
  filedes1 = dup(1);

  if(pipe(pipe_d) == -1)
  die("pipe()");

  prc = fork();
  if(prc == (pid_t)(-1))
  die("fork()"); /* fork failed */

  if(prc == (pid_t)0) {
    /* prc process */

    close(1);       /* close stdout */

    if(dup(pipe_d[1]) == -1)
    die("dup()");

    
    if(!execute(ca1))
    die("execlp()");

    _exit(EXIT_SUCCESS);
  } else {
    

    close(0);       

    if(dup(pipe_d[0]) == -1)
    die("dup()");

    
    if(!execute(ca2))
    die("execlp()");

    // exit(EXIT_SUCCESS);
  }

  // restore the stdin and stdout file descriptors
  dup2(filedes0, 0);
  dup2(filedes1, 1);
  close(filedes0);
  close(filedes1);

  return true;
}

void die(const char *msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}

// for an input redirector
bool input_redirection(char **ca, string inputfile) {
  int input;
  bool execution_result;
  int filedes0;

  // save the stdin file descriptor
  filedes0 = dup(0);

  // open input file
  input = open(inputfile.c_str(), O_RDONLY);

  // replace standard input with input file
  dup2(input, 0);

  // close unused file descriptors
  close(input);

  // execute
  execution_result = execute(ca);

  // restore the stdin file descriptor
  dup2(filedes0, 0);
  close(filedes0);

  return execution_result;
}

// for an output redirector
bool out_redirection(char **ca, string output_file) {
  int out;
  bool execution_result;
  int filedes1;

  // save the stdin and stdout file descriptors
  filedes1 = dup(1);

  // open output file
  out = open(output_file.c_str(), O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);

  // replace standard output with output file
  dup2(out, 1);

  // close unused file descriptor
  close(out);

  // execute
  execution_result = execute(ca);

  // restore the stdout file descriptor
  dup2(filedes1, 1);
  close(filedes1);

  return execution_result;
}

// for an appending output redirector
bool out_redirection2(char **ca, string output_file) {
  int out;
  bool execution_result;
  int filedes1;

  // save the stdin and stdout file descriptors
  
  filedes1 = dup(1);

  // open input and output files
  out = open(output_file.c_str(), O_WRONLY | O_APPEND, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);

  // replace standard output with output file
  dup2(out, 1);

  // close unused file descriptor
  close(out);

  // execute
  execution_result = execute(ca);

  // restore the stdout file descriptor
  dup2(filedes1, 1);
  close(filedes1);

  return execution_result;
}

bool exe_test(string command)
{
  struct stat statb;
  string string_test = "";
  const char *character;

  if (command.substr(0, 5) == "test ")
  { // example: test -e test.txt
    // get the argument
    string_test = command.substr(5, 3);

    // strip "test " and the argument from the command to get just the file/directory name
    if (string_test == "-e " || string_test == "-f " || string_test == "-d ")
    {
      character = command.substr(5 + 3).c_str();
    }
    else
    {
      character = command.substr(5).c_str();
    }
  }
  else
  { // example: [ -e test.txt ]
    // get the argument
    string_test = command.substr(2, 3);

    // strip "[ ", " ]", and the argument from the command to get just the file/directory name
    if (string_test == "-e " || string_test == "-f " || string_test == "-d ")
    {
      character = command.substr(2 + 3, command.length() - (2 + 3) - 2).c_str();
    }
    else
    {
      character = command.substr(2, command.length() - 2).c_str();
    }
  }

  // display whether the file/directory exists
  if (stat(character, &statb) == -1)
  {
    cout << "(False)" << endl;
    return false;
  }
  else
  {
    // display whether the file/directory is a file
    if (string_test == "-f ")
    {
      if ((statb.st_mode & S_IFMT) != S_IFREG)
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
    else if (string_test == "-d ")
    {
      if ((statb.st_mode & S_IFMT) != S_IFDIR)
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

void trim2(string &s)
{
  while (s[0] == ' ')
  {
    s = s.substr(1);
  }
  while (s[s.length() - 1] == ' ')
  {
    s = s.substr(0, s.length() - 1);
  }
}

int main()
{
  string command = "";
  string currcommand = ""; // stores a single command from the input line with potentially multiple commands
  char *ca[64];
  char *ca2[64];
  bool execution_result = true;
  bool prevexecution_result = true;
  string prevConn = ";";
  bool doExit = false;
  bool skipPar = false;
  bool stopParSkip = false;
  string prevcommand = "";
  bool prevPipe = false;
  bool previnput_redirection = false;
  bool prevout_redirection = false;
  bool prevout_redirection2 = false;

  while (!doExit)
  {

    // display the command prompt
    cout << "$ ";

    // read the input
    getline(cin, command);

    // reset things
    currcommand = "";
    execution_result = true;
    prevexecution_result = true;
    prevConn = ";";
    skipPar = false;
    stopParSkip = false;
    prevcommand = "";
    prevPipe = false;
    previnput_redirection = false;
    prevout_redirection = false;
    prevout_redirection2 = false;

    // loop through the command line one character at a time
    for (int i = 0; i < (int)command.length() + 1; i++)
    {

      // check if we're in parenthesis
      if (command[i] == '(')
      {
        // if the previous result failed, start skipping everything inside the parenthesis
        if ((prevConn == "&&" && !prevexecution_result) || (prevConn == "||" && prevexecution_result))
        {
          skipPar = true;

          // set the previous result for use after the parenthesis has been skipped
          if (prevConn == "&&")
          {
            prevexecution_result = prevexecution_result && false;
          }
          else if (prevConn == "||")
          {
            prevexecution_result = prevexecution_result || false;
          }
        }
        continue;
      }
      else if (command[i] == ')')
      { // we've reached the end of a parenthesis.
        stopParSkip = true;
        continue;
      }
      else if (command[i] == '|' && command[i + 1] != '|')
      { // A wild pipe appeared!

        // trim leading and trailing spaces
        trim2(currcommand);

        prevPipe = true;
        prevcommand = currcommand;
        currcommand = "";
        continue;
      }
      else if (command[i] == '<')
      { // A wild input redirector appeared!

        // trim leading and trailing spaces
        trim2(currcommand);

        previnput_redirection = true;
        prevcommand = currcommand;
        currcommand = "";
        continue;
      }
      else if (command[i] == '>' && command[i + 1] == '>')
      { // A wild output redirector appeared!

        // trim leading and trailing spaces
        trim2(currcommand);

        prevout_redirection2 = true;
        prevcommand = currcommand;
        currcommand = "";
        i++;
        continue;
      }
      else if (command[i] == '>')
      { // A wild output redirector appeared!

        // trim leading and trailing spaces
        trim2(currcommand);

        prevout_redirection = true;
        prevcommand = currcommand;
        currcommand = "";
        continue;
      }

      // Here comes a new delimiter!
      if (command[i] == ';' || command[i] == '&' || command[i] == '|' || command[i] == '#' || i == (int)command.length())
      {
        // trim leading and trailing spaces
        trim2(currcommand);

        // skip to the next iteration if the command is blank after being trimmed
        if (currcommand == "")
        {
          continue;
        }

        // execute the current command if we're not skipping parenthesis
        if (!skipPar)
        {
          // execute the current command if the previous result was successful
          if (prevConn == ";" || (prevConn == "&&" && prevexecution_result) || (prevConn == "||" && !prevexecution_result))
          {
            if (previnput_redirection)
            { // we've encountered a normal command
              cout << "We're input redirecting! Previous command: " << prevcommand << ". currcommand: " << currcommand << endl;
              char *currcommand2 = &prevcommand[0];
              parse(currcommand2, ca);

              execution_result = input_redirection(ca, currcommand);
              previnput_redirection = false;
            }
            else if (prevout_redirection)
            { // we've encountered a normal command
              cout << "We're output redirecting! Previous command: " << prevcommand << ". currcommand: " << currcommand << endl;
              char *currcommand2 = &prevcommand[0];
              parse(currcommand2, ca);

              execution_result = out_redirection(ca, currcommand);
              prevout_redirection = false;
            }
            else if (prevout_redirection2)
            { // we've encountered a normal command
              cout << "We're output redirecting! Previous command: " << prevcommand << ". currcommand: " << currcommand << endl;
              char *currcommand2 = &prevcommand[0];
              parse(currcommand2, ca);

              execution_result = out_redirection2(ca, currcommand);
              prevout_redirection2 = false;
            }
            else if (prevPipe)
            { // we've encountered a normal command
              cout << "We're piping! Previous command: " << prevcommand << ". currcommand: " << currcommand << endl;
              char *currcommand2 = &prevcommand[0];
              char *currcommand3 = &currcommand[0];
              parse(currcommand2, ca);
              parse(currcommand3, ca2);

              execution_result = execute2(ca, ca2);
              prevPipe = false;
            }
            else if (currcommand.substr(0, 5) == "test " || currcommand.substr(0, 2) == "[ ")
            { // we've encountered a test command
              execution_result = exe_test(currcommand);
            }
            else if (currcommand == "exit")
            { // we've encountered an exit command
              doExit = true;
              break;
            }
            else
            { // we've encountered a normal command
              char *currcommand2 = &currcommand[0];
              parse(currcommand2, ca);

              execution_result = execute(ca);
            }
          }

          if (prevConn == ";")
          { // for a ";" connector
            prevexecution_result = execution_result;
          }
          else if (prevConn == "&&")
          { // for a "&&" connector
            prevexecution_result = prevexecution_result && execution_result;
          }
          else if (prevConn == "||")
          { // for a "||" connector
            prevexecution_result = prevexecution_result || execution_result;
          }
          else if (command[i] == '#')
          { // for a comment "#"
            break;
          }
          else if (i == (int)command.length())
          { // we've reached the end of the command. execute whatever is remaining
            break;
          }

        }

        // stop skipping parenthesis
        if (stopParSkip)
        {
          skipPar = false;
          stopParSkip = false;
        }

        // update the previous connector
        if (command[i] == ';' || command[i] == '&' || command[i] == '|')
        {
          prevConn = command[i];
        }

        // reset things
        currcommand = "";

        // skip the second "&&" in the "&&" connector
        if (command[i] == '&')
        {
          i++;
        }

      }
      else
      {
        // add the current character to the current command variable
        currcommand += command[i];
      }
    }
  }

  cout << "Exiting..." << endl;

  return 0;
}