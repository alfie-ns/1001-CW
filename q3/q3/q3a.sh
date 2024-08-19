#!/bin/bash

print_red() { tput setaf 1; echo -e "$1"; tput sgr0; } # tput setaf 1 == red text
print_green() { tput setaf 2; echo -e "$1"; tput sgr0; } # tput setaf 2 == green text
print_amber() { tput setaf 3; echo -e "$1"; tput sgr0; } # tput setaf 3 == amber/yellow text

compile_success=false

# Determine which compilation to attempt based on the operating system
if [[ "$OSTYPE" == "darwin"* ]]; then # star matches any string that starts with "darwin" and is followed by any characters (or no characters).
    # Compile C++ program q3a-mac.cpp for macOS
    if clang++ -std=c++11 -o q3_mac q3a-mac.cpp; then
        print_green "q3a-mac.cpp compiled successfully for macOS!"
        compile_success=true
        executable="./q3_mac"
    else
        print_amber "Warning: q3a-mac.cpp did not compile successfully for macOS..."
    fi 
else # Only try g++ if NOT on macOS
    # Compile C++ program q3a.cpp for Linux
    if g++ -std=c++11 -o q3 q3a.cpp -lm; then
        print_green "q3a.cpp compiled successfully for Linux!"
        compile_success=true
        executable="./q3"
    else
        print_amber "Warning: q3a.cpp did not compile successfully for Linux..."
    fi 
fi

# if compilation failed, exit with error 
if [ "$compile_success" = false ]; then
    print_red "Error: Failed to compile the program."
    exit 1
fi

# Create variables to hold the image locations
input_image="q3-images/input_images/a2.pgm"
blur_image="q3-images/output_images/blurred.pgm"
edge_image="q3-images/output_images/edge_detected.pgm"

# Run the program if it was compiled successfully
if [ -f "$executable" ]; then
    if $executable "$input_image" "$blur_image" "$edge_image"; then
        print_green "q3a.sh executed successfully!"
    else
        print_red "Error: q3a.sh did not execute successfully..."
        exit 1
    fi
else
    print_red "Error: Executable not found. Compilation may have failed."
    exit 1
fi

exit 0 #success

# make bash script executable with: chmod +x q3a.sh

: <<'END'

Use g++ to compile the C++ program q3a.cpp 
located in the q3 directory; thus creating a 
Linux-compatible executable. Include the math
library by specifying -lm in the compilation command.

I am mirroring the old q3a.sh script, only adding
if statements and print_green and print_red                     
functions.

END