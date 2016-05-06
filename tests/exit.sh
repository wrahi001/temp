#!/bin/sh

clear
echo "#!/bin/sh" > test_exit.sh
echo "exit 30" >> test_exit.sh
chmod 711 test_exit.sh
echo ""

echo "Exit command terminates the script, so it is hard to prove if it works."
echo "Therefore, to prove that our exit works, we created test_exit.sh."
echo "We will predict the exit code number to prove that it works."
echo ""
ls
echo ""
echo "The file contains:"
cat test_exit.sh
echo ""

echo "Then we execute it (./test_exit.sh) "
echo "We can check the exit code number by printing \"\$?"\"
echo "Since exit status is 30, then the exit code number should be 30"
./test_exit.sh
echo "The exit code number: "$?""
echo ""

echo "Another tests:"
asd exit
ls exit ; #exit
cat exit
rm test_exit.sh exit

echo "This" ; echo "Will" && echo "Be" ; echo "The" ; echo "Last" &&
\echo "Line"

exit

echo "Everything from here will be ignored!"
ls
ls
asdasdfjasdfas
qweruwqi
ls asd
ls -alt
huhuhuhu
