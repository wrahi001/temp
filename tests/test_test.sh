#!/bin/sh

[ /home ] && echo "/home exists" || echo "/home doesn't exist"
[ -e /home ] && echo "/home exists" || echo "/home doesn't exist"
[ -f /home ] && echo "file /home exists" || echo "file /home doesn't exist"
[ -d /home ] && echo "directory /home exists" || echo "directory /home doesn't exist"

echo ""
echo "[ /home]" && [ /home]
echo "[ /home" && [ /home
echo "[/home ] && [/home ]
echo "[/home] && [/home]
echo "[/home" && [/home

pwd
ls

echo "" 
[ makefile ] && echo "makefile exists" || echo "makefile doesn't exist"
[ -e makefile ] && echo "makefile exists" || echo "makefile doesn't exist"
[ -f makefile ] && echo "file makefile exists" || echo "file makefile doesn't exist"
[ -d makefile ] && echo "directory makefile exists" || echo "directory makefile doesn't exist"

echo ""
[ makefile]
[ makefile
[makefile ]
[makefile]
[makefile
