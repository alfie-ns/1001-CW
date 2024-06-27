#!/bin/bash
cd q3/q3
clang++ -std=c++17 q3b-mac.cpp -o image_processor -O3 -lm # Compile the program
./image_processor q3-images/input_images q3-images/output_images # Run the program in the respective directorys, input_images and output_images
