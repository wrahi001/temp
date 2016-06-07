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
// #include <boost/algorithm/string.hpp>

using namespace std;
// using namespace boost::algorithm;

/* function prototypes */
void die(const char*);

// split the input line into an array
void parse(char *line, char **argv)
{
  while (*line != '\0')
  {    /* if not the end of line ....... */
    while (*line == ' ' || *line == '\t' || *line == '\n')
    *line++ = '\0';  /* replace white spaces with 0    */
    *argv++ = line;    /* save the argument position  */
    while (*line != '\0' && *line != ' ' &&
    *line != '\t' && *line != '\n')
    line++;       /* skip the argument until ...    */
  }
  *argv = '\0';        /* mark the end of argument list  */
}

// execute the command that's been split up into an array
bool execute(char **argv)
{
  pid_t pid = fork();
  int status;

  if (pid < 0)
  {  /* fork a child process     */
    perror("fork failed");
    exit(1);
  }
  else if (pid == 0)
  {    /* for the child process:      */
    if (execvp(*argv, argv) < 0)
    {  /* execute the command  */
      perror("execvp failed");
      exit(1);
    }
  }
  else
  {                /* for the parent:   */
    while (waitpid(pid, &status, 0) != pid)    /* wait for completion  */
    ;
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
bool execute2(char **argv1, char **argv2) {
  int pdes[2];
  pid_t child;
  int fd0;
  int fd1;

  // save the stdin and stdout file descriptors
  fd0 = dup(0);
  fd1 = dup(1);

  if(pipe(pdes) == -1)
  die("pipe()");

  child = fork();
  if(child == (pid_t)(-1))
  die("fork()"); /* fork failed */

  if(child == (pid_t)0) {
    /* child process */

    close(1);       /* close stdout */

    if(dup(pdes[1]) == -1)
    die("dup()");

    /* now stdout and pdes[1] are equivalent (dup returns lowest free descriptor) */

    if(!execute(argv1))
    die("execlp()");

    _exit(EXIT_SUCCESS);
  } else {
    /* parent process */

    close(0);       /* close stdin */

    if(dup(pdes[0]) == -1)
    die("dup()");

    /* now stdin and pdes[0] are equivalent (dup returns lowest free descriptor) */

    if(!execute(argv2))
    die("execlp()");

    // exit(EXIT_SUCCESS);
  }

  // restore the stdin and stdout file descriptors
  dup2(fd0, 0);
  dup2(fd1, 1);
  close(fd0);
  close(fd1);

  return true;
}

void die(const char *msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}

// for an input redirector
bool inRedir(char **argv, string inFile) {
  int in;
  bool execRes;
  int fd0;

  // save the stdin file descriptor
  fd0 = dup(0);

  // open input file
  in = open(inFile.c_str(), O_RDONLY);

  // replace standard input with input file
  dup2(in, 0);

  // close unused file descriptors
  close(in);

  // execute
  execRes = execute(argv);

  // restore the stdin file descriptor
  dup2(fd0, 0);
  close(fd0);

  return execRes;
}

// for an output redirector
bool outRedir(char **argv, string outFile) {
  int out;
  bool execRes;
  int fd1;

  // save the stdin and stdout file descriptors
  fd1 = dup(1);

  // open output file
  out = open(outFile.c_str(), O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);

  // replace standard output with output file
  dup2(out, 1);

  // close unused file descriptor
  close(out);

  // execute
  execRes = execute(argv);

  // restore the stdout file descriptor
  dup2(fd1, 1);
  close(fd1);

  return execRes;
}

// for an appending output redirector
bool outRedir2(char **argv, string outFile) {
  int out;
  bool execRes;
  int fd1;

  // save the stdin and stdout file descriptors
  fd0 = dup(0);
  fd1 = dup(1);

  // open input and output files
  out = open(outFile.c_str(), O_WRONLY | O_APPEND, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);

  // replace standard output with output file
  dup2(out, 1);

  // close unused file descriptor
  close(out);

  // execute
  execRes = execute(argv);

  // restore the stdout file descriptor
  dup2(fd1, 1);
  close(fd1);

  return execRes;
}

// run a test command such as
// test -e test.txt
// or
// [ -e test.txt ]
bool runTest(string cmd)
{
  struct stat sb;
  string testArg = "";
  const char *c;

  if (cmd.substr(0, 5) == "test ")
  { // example: test -e test.txt
    // get the argument
    testArg = cmd.substr(5, 3);

    // strip "test " and the argument from the command to get just the file/directory name
    if (testArg == "-e " || testArg == "-f " || testArg == "-d ")
    {
      c = cmd.substr(5 + 3).c_str();
    }
    else
    {
      c = cmd.substr(5).c_str();
    }
  }
  else
  { // example: [ -e test.txt ]
    // get the argument
    testArg = cmd.substr(2, 3);

    // strip "[ ", " ]", and the argument from the command to get just the file/directory name
    if (testArg == "-e " || testArg == "-f " || testArg == "-d ")
    {
      c = cmd.substr(2 + 3, cmd.length() - (2 + 3) - 2).c_str();
    }
    else
    {
      c = cmd.substr(2, cmd.length() - 2).c_str();
    }
  }

  // display whether the file/directory exists
  if (stat(c, &sb) == -1)
  {
    cout << "(False)" << endl;
    return false;
  }
  else
  {
    // display whether the file/directory is a file
    if (testArg == "-f ")
    {
      if ((sb.st_mode & S_IFMT) != S_IFREG)
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
    else if (testArg == "-d ")
    {
      if ((sb.st_mode & S_IFMT) != S_IFDIR)
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
  string cmd = "";
  string currCmd = ""; // stores a single command from the input line with potentially multiple commands
  char *argv[64];
  char *argv2[64];
  bool execRes = true;
  bool prevExecRes = true;
  string prevConn = ";";
  bool doExit = false;
  bool skipPar = false;
  bool stopParSkip = false;
  string prevCmd = "";
  bool prevPipe = false;
  bool prevInRedir = false;
  bool prevOutRedir = false;
  bool prevOutRedir2 = false;

  while (!doExit)
  {

    // display the command prompt
    cout << "$ ";

    // read the input
    getline(cin, cmd);

    // reset things
    currCmd = "";
    execRes = true;
    prevExecRes = true;
    prevConn = ";";
    skipPar = false;
    stopParSkip = false;
    prevCmd = "";
    prevPipe = false;
    prevInRedir = false;
    prevOutRedir = false;
    prevOutRedir2 = false;

    // loop through the command line one character at a time
    for (int i = 0; i < (int)cmd.length() + 1; i++)
    {

      // check if we're in parenthesis
      if (cmd[i] == '(')
      {
        // if the previous result failed, start skipping everything inside the parenthesis
        if ((prevConn == "&&" && !prevExecRes) || (prevConn == "||" && prevExecRes))
        {
          skipPar = true;

          // set the previous result for use after the parenthesis has been skipped
          if (prevConn == "&&")
          {
            prevExecRes = prevExecRes && false;
          }
          else if (prevConn == "||")
          {
            prevExecRes = prevExecRes || false;
          }
        }
        continue;
      }
      else if (cmd[i] == ')')
      { // we've reached the end of a parenthesis.
        stopParSkip = true;
        continue;
      }
      else if (cmd[i] == '|' && cmd[i + 1] != '|')
      { // A wild pipe appeared!

        // trim leading and trailing spaces
        trim2(currCmd);

        prevPipe = true;
        prevCmd = currCmd;
        currCmd = "";
        continue;
      }
      else if (cmd[i] == '<')
      { // A wild input redirector appeared!

        // trim leading and trailing spaces
        trim2(currCmd);

        prevInRedir = true;
        prevCmd = currCmd;
        currCmd = "";
        continue;
      }
      else if (cmd[i] == '>' && cmd[i + 1] == '>')
      { // A wild output redirector appeared!

        // trim leading and trailing spaces
        trim2(currCmd);

        prevOutRedir2 = true;
        prevCmd = currCmd;
        currCmd = "";
        i++;
        continue;
      }
      else if (cmd[i] == '>')
      { // A wild output redirector appeared!

        // trim leading and trailing spaces
        trim2(currCmd);

        prevOutRedir = true;
        prevCmd = currCmd;
        currCmd = "";
        continue;
      }

      // Here comes a new delimiter!
      if (cmd[i] == ';' || cmd[i] == '&' || cmd[i] == '|' || cmd[i] == '#' || i == (int)cmd.length())
      {
        // trim leading and trailing spaces
        trim2(currCmd);

        // skip to the next iteration if the command is blank after being trimmed
        if (currCmd == "")
        {
          continue;
        }

        // execute the current command if we're not skipping parenthesis
        if (!skipPar)
        {
          // execute the current command if the previous result was successful
          if (prevConn == ";" || (prevConn == "&&" && prevExecRes) || (prevConn == "||" && !prevExecRes))
          {
            if (prevInRedir)
            { // we've encountered a normal command
              cout << "We're input redirecting! Previous command: " << prevCmd << ". currCmd: " << currCmd << endl;
              char *currCmd2 = &prevCmd[0];
              parse(currCmd2, argv);

              execRes = inRedir(argv, currCmd);
              prevInRedir = false;
            }
            else if (prevOutRedir)
            { // we've encountered a normal command
              cout << "We're output redirecting! Previous command: " << prevCmd << ". currCmd: " << currCmd << endl;
              char *currCmd2 = &prevCmd[0];
              parse(currCmd2, argv);

              execRes = outRedir(argv, currCmd);
              prevOutRedir = false;
            }
            else if (prevOutRedir2)
            { // we've encountered a normal command
              cout << "We're output redirecting! Previous command: " << prevCmd << ". currCmd: " << currCmd << endl;
              char *currCmd2 = &prevCmd[0];
              parse(currCmd2, argv);

              execRes = outRedir2(argv, currCmd);
              prevOutRedir2 = false;
            }
            else if (prevPipe)
            { // we've encountered a normal command
              cout << "We're piping! Previous command: " << prevCmd << ". currCmd: " << currCmd << endl;
              char *currCmd2 = &prevCmd[0];
              char *currCmd3 = &currCmd[0];
              parse(currCmd2, argv);
              parse(currCmd3, argv2);

              execRes = execute2(argv, argv2);
              prevPipe = false;
            }
            else if (currCmd.substr(0, 5) == "test " || currCmd.substr(0, 2) == "[ ")
            { // we've encountered a test command
              execRes = runTest(currCmd);
            }
            else if (currCmd == "exit")
            { // we've encountered an exit command
              doExit = true;
              break;
            }
            else
            { // we've encountered a normal command
              char *currCmd2 = &currCmd[0];
              parse(currCmd2, argv);

              execRes = execute(argv);
            }
          }

          if (prevConn == ";")
          { // for a ";" connector
            prevExecRes = execRes;
          }
          else if (prevConn == "&&")
          { // for a "&&" connector
            prevExecRes = prevExecRes && execRes;
          }
          else if (prevConn == "||")
          { // for a "||" connector
            prevExecRes = prevExecRes || execRes;
          }
          else if (cmd[i] == '#')
          { // for a comment "#"
            break;
          }
          else if (i == (int)cmd.length())
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
        if (cmd[i] == ';' || cmd[i] == '&' || cmd[i] == '|')
        {
          prevConn = cmd[i];
        }

        // reset things
        currCmd = "";

        // skip the second "&&" in the "&&" connector
        if (cmd[i] == '&')
        {
          i++;
        }

      }
      else
      {
        // add the current character to the current command variable
        currCmd += cmd[i];
      }
    }
  }

  cout << "Exiting..." << endl;

  return 0;
}