#!/bin/sh

clear
echo "Clear!"
echo "Hello world!" > hello_world.txt
echo ""

echo "We typed pwd (print working directory) so we know where we are!"
pwd
echo ""

echo "ls, ls -a, ls -l, ls -t:"
ls
ls -a
ls -l
ls -t
echo ""

echo "'ls -alt' should give the same output as the above:"
ls -alt
echo ""

echo "echo 'ls #-alt' should be the same as ls:"
ls #-alt
echo ""

touch sample.txt
ls
rm sample.txt
ls
echo ""

echo "Errors:"
how
are
you?
ls hello
echo ""

echo "Create a file named cs100.txt:"
echo "CS 100 - Assignment 1" > cs100.txt
ls
echo ""

echo "Outputs the contents of the file:"
cat cs100.txt
echo ""

echo "'>' overwrites all original content:"
echo "CS 100 - Assignment 2" > cs100.txt
cat cs100.txt
echo ""

echo "'>>' appends to the original content:"
echo "Friday, May 6 2016" >> cs100.txt
cat hello_world.txt >> cs100.txt
cat cs100.txt
echo ""

echo "Copy a file:"
cp hello_world.txt hello_world_2.txt
ls
cat hello_world_2.txt
echo ""

echo "Change filename: "
echo "Before:"
ls
echo "After:"
mv hello_world.txt hello_me.txt
ls
echo ""

echo "Remove every .txt files except LICENSE.txt:"
echo "Before:"
ls
echo "After:"
cp LICENSE.txt LICENSE.temp
rm *.txt
mv LICENSE.temp LICENSE.txt
ls
echo ""

asd=test
echo "$asd"
asd=asd
echo "$asd"
echo "$?"
echo " "
