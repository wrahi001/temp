Assignment 3: Writing a Basic Command Shell
===========================================

Introduction
------------

This program was created to emulate a basic command shell for educational purposes.

Usage
-----

Enter any command as you would in a terminal such as "ls -a". If you wish to put multiple commands on one line, separate each command with connector. The possible connectors are ";", "&&", and "||". The command after a ";" is always executed. The command after a "&&" is only executed if the command before the "&&" executed successfully. The command after a "||" is only executed if the command before the "||" executed unsuccessfully.

Examples
--------

To run a single command:
> mkdir test

To run multiple commands:
> Execute commands without checking for successful/unsuccessful executions:
>> echo hello; echo something; echo random; mkdir foldername

> Execute a command only if the previous command executed successfully:
>> mkdir test && echo hello

> Execute a command only if the previous command did not executed unsuccessfully:
>> mkdir test || echo goodbye

> Execute a mixed command:
>> touch testfile1; mkdir testdir1 && ls somefolder || cal

Known Issues
------------

As long as a connector begins with a ";", "&", or "|", the command will be accepted. For example, in the command "ls -a |random date", the "|random" part will be treated as a "||" connector. This is because the program only checks the first character of each word in the command to see if it's a connector.

All ";", "&", and "|"'s are considered to be connectors. Using a connector character as part of a file, folder, or argument will not likely work as expected. As an example, the commands:
> mkdir "a;b"
> touch "c|d"
> mkdir "Mary & John"
will not execute as they should.

Commands that terminate abnormally may incorrectly be considered executed successfully to the program.

Bugs
----

The connectors proved to produced the highest amount of bugs.  The ; connector outputed the most complier and run-time erros.  This was by far the most difficult bug for us to resolve.  This was due to the reason that we did not have a full understanding on how two dimensional arrays worked.  
