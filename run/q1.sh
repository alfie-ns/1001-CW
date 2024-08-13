#!/bin/bash
cd ../q1
if clang++ -std=c++11 -O3 -march=armv8-a+simd q1-mac.cpp -o q1-mac-executable; then
    clear && echo -e "\nCompiled q1-mac.cpp successfully\n";
fi

if ./q1-mac-executable; then rm -f q1-mac-executable; fi
# if executable runs successfully, force remove it