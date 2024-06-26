#!/bin/bash

# ---------------- Run the program ----------------

# 1. Clear previous run's output
#--------------------------------
rm -f image_processor # Force remove the executable file
cd q3-images/output_images # Change directory to output_images
for i in *; do # iterate through ALL files in output_images
    rm -f $i # Force remove each file
done
# 2. Compile program
# -------------------
clang++ -std=c++17 q3b-mac.cpp -o image_processor -O3 -lm 
# 3. Run the program
# -------------------
./image_processor q3-images/input_images q3-images/output_images # Run the program in the respective directorys, input_images and output_images