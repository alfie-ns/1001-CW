#!/bin/bash

print_red() { printf "\033[0;31m%s\033[0m\n" "$1"; }
print_green() { printf "\033[0;32m%s\033[0m\n" "$1"; }

# Compile C++ program q3a-mac.cpp, if successful print success message, else print error message
if clang++ -std=c++11 -o q3 q3a-mac.cpp; then
    print_green "q3a.cpp compiled successfully!"
else
    print_red "Error: q3a.cpp did not compile successfully..."
    exit 1
fi 

# Create variables to hold the image locations. Use the full path name for each image.
input_image=q3-images/input_images/a2.pgm
blur_image=q3-images/output_images/blurred.pgm
edge_image=q3-images/output_images/edge_detected.pgm

# Run program, passing the variables for the 3 image locations, if run successfully print success message, else print error message
if ./q3 "$input_image" "$blur_image" "$edge_image"; then
    print_green "q3a.sh executed successfully!"
    exit 0
else
    print_red "Error: q3a.sh did not execute successfully..."
    exit 1
fi

# Compile bash script with: chmod +x q3a.sh

: <<'END'

Use clang++ to compile the C++ program q3a.cpp 
located in the q3 directory; thus creating a 
macOS-compatible executable.

I am mirroring the old q3a.sh script, only adding
if statements and print_green and print_red                     
functions, and adapting for macOS.

END