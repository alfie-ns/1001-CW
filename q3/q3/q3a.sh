#!/bin/bash

print_green() { tput setaf 2; echo -e "$1"; tput sgr0; } # tput setaf 2 == green text
print_red() { tput setaf 1; echo -e "$1"; tput sgr0; } # tput setaf 1 == red text
# -e: interpret backslash escapes - allows for more advanced text formatting(newlines etc)
# echo the first argument ($1) in the predefined color eg: print_green 'test' where 'test' is $1
# tput sgr0: reset all attributes (so don't affect future text)

# Compile C++ program q3a.cpp, if successful print success message, else print error message
if g++ -o q3 ../../q3/main_linux.cpp -lm; then print_green "q3a.cpp compiled successfully!"
else #if compilation fails, echo error and exit with status 1: failure
    print_red "Error: q3a.cpp did not compile successfully..."
    exit 1 #failure
fi 

# Create variables to hold the image locations. Use the full path name for each image.
input_image=../../q3-images/input_images/a1.pgm
blur_image=../../q3-images/output_images/blurred.pgm
edge_image=../../q3-images/output_images/edge_detected.pgm

# Run program, passing the variables for the 3 image locations, if run successfully print success message, else print error message
if ./q3 $input_image $blur_image $edge_image; then
    print_green "q3a.sh executed successfully!"
    exit 0 #success
else
    print_red "Error: q3a.sh did not execute successfully..."
    exit 1 #failure
fi

# Compile bash script with: chmod +x q3a.sh

: <<'END'

Use g++ to compile the C++ program q3a.cpp 
located in the q3 directory, creating a 
Linux-compatible executable. Include the math
library by specifying -lm in the compilation command.

I am mirroring the old q3a.sh script, only adding
if statements and print_green and print_red
functions.

END

