#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <vector>
using namespace std;


/***************************************************************/
//This is our base class.  Our subclasses will make use of //these functions. 
class Base
{
	public:
		virtual void print() = 0;
		virtual void analyze(char **analyzed_command) = 0;
	     virtual bool execute(char **input_analyzed) = 0;
		virtual void get() = 0;
		virtual string check_if_exit() = 0;	
	protected:
		char userinput[256];    //This is the array to hold
						   //the type command
};
/***************************************************************/



/***************************************************************/
class Readinput : public Base
{
	public:
		void analyze(char **analyzed_command){}
	     bool execute(char **input_analyzed){
		return false;}	
		void get()
		{
			cin.getline(userinput, 256);			
		}
		void print()
		{
			cout << "you entered: " 
			     << userinput << endl;
		}
		string check_if_exit()
		{
			string exit(userinput);
			return exit;

		}
};
/***************************************************************/



/***************************************************************/
//This is our composite class.  It has a vector of Base //pointers.
class CompositeCommand : public Base
{
	public:
		void analyze(char **analyzed_command); 
		bool execute(char **input_analyzed);
		void print()
		{
			cout << "you typed " 
			     << userinput << endl;
		}
		void get()
		{
			cin.getline(userinput,256);
		}
		string check_if_exit()
		{
			string exit(userinput);
			return exit;
		}
	private:
		vector<Base*> children;
};
/***************************************************************/






/***************************************************************/
//This code will analyzed the type command and make distinctions
void CompositeCommand::analyze(char **analyzed_command)  
{
  		char *one_command = strtok(userinput, " "); 	  		      int i = 0;                                      
  		while(one_command != NULL)
 		{  
			analyzed_command[i] = one_command ;
    			i++;
    			one_command = strtok(NULL, " ");
  		}
  		analyzed_command[i] = NULL;                         
  		return;
}
/***************************************************************/



/***************************************************************/
//This code will execute a command through a process.
//It will return a true or false value depending if the command 
//was successful
bool CompositeCommand::execute(char **input_analyzed)	
{
  int state = 0;
  pid_t process = fork();

  if(process < 0)
  {                                 
    perror("fork error");       
    exit(1);
  }
  else if(process == 0)
  {
    
    if (execvp(input_analyzed[0], input_analyzed) < 0)
	{
		perror("execvp error");
		exit(1);
	}
  }
  else
  {
    while (waitpid(process, &state, 0) != process);
    if(WIFEXITED(state))
    {
	    if(WEXITSTATUS(state) != 0)
	    {
		    return false;
	    }
    }
  }

  return true;
}
/***************************************************************/











