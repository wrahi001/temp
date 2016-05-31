#!/bin/sh

echo A && echo B || echo C && echo D

(echo A && echo B) || (echo C && echo D)

test /home || echo skipped && echo skippedtoo

test /home || (echo skipped && echo skippedtoo)

( echo A )
( echo A)
(echo A )
(echo A)

echo P && echo Q || (echo R ;  echo S ; echo T) || (echo A || echo B) && echo C

echo B || (echo C && echo D)

Hello || echo "What" || (echo "Do" && echo "You" && echo "Want?")

(echo "One" && echo "Two") || (echo "Three" && echo "Four") || (echo "Five" && echo "Six" && echo "Seven") && echo "Eight"

(echo A || echo B && echo C) && (echo "Hi" || echo "Hello") || echo "Dude" ; echo "Jude"

echo A ; echo B ; echo C || echo D ; echo E
