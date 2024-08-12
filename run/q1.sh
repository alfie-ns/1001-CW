#!/bin/bash
cd ../q1
if clang++ -std=c++11 -O3 -march=armv8-a+simd q1-mac.cpp -o q1-mac-executable; then
    clear
    echo -e "\nCompiled q1-mac.cpp successfully\n"
fi

if ./q1-mac-executable; then sleep 2 && rm -f q1-mac-executable; 
else echo -e "\nCould not run q1-mac-executable\m"; fi
