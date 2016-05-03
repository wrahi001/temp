#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "rshell.h"
using namespace std;


int main()
{
  Base *command_pointer = new CompositeCommand;
  int e = 0;
  bool ex_result;
  char **analyzed = new char*[256];
  char **analyzed2 = new char*[256];
  bool skip = false;

  cout << "Welcome to the rshell" << endl;					
  while(true)
  {
    cout << "rshell$ ";   
    command_pointer->get();
    if(command_pointer->check_if_exit() == "exit")
    {
	    cout << "Now exiting the rshell" << endl;
	    exit(1);
    }
    command_pointer->analyze(analyzed);
/*********************/    
    for (int i = 0; analyzed[i] != NULL; i++)
    {
	for (int j = 0; analyzed[i][j] != '\0'; j++)
		if(analyzed[i][j] == ';' && j!=0)
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
	
      if (*analyzed[i] == ';')
      {
        ex_result = command_pointer->execute(analyzed2);
        delete [] analyzed2;
        analyzed2 = new char*[256];
        e = 0;
      }
      else if (*analyzed[i] == '&')
      {
        ex_result = command_pointer->execute(analyzed2);
        delete [] analyzed2;
        analyzed2 = new char*[256];
        e = 0;
        if (!ex_result)
		{
			break;
		}
      }
      else if (*analyzed[i] == '|' )
      {
        ex_result = command_pointer->execute(analyzed2);
        delete [] analyzed2;
        analyzed2 = new char*[256];
        e = 0;
        if (ex_result)
		{
			break;
		}
      }
      else if (*analyzed[i] == '#' )
      {
		break;		       
      }
      else
      {
	if(!skip)
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
		command_pointer->execute(analyzed2);
	      	delete [] analyzed2;
		analyzed2 = new char*[256];
	        e = 0;	
	}
/*********************************/	
  }
  return 0;
}
