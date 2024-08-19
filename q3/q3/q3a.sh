#!/bin/bash

print_red() { tput setaf 1; echo -e "$1"; tput sgr0; }
print_green() { tput setaf 2; echo -e "$1"; tput sgr0; }
print_amber() { tput setaf 3; echo -e "$1"; tput sgr0; }

compile_success=false

if [[ "$OSTYPE" == "darwin"* ]]; then # if macOS
    if clang++ -std=c++11 -o q3_mac q3a-mac.cpp; then # if compilation in macOS succeeded
        print_green "q3a-mac.cpp compiled successfully for macOS!"
        compile_success=true
        executable="./q3_mac"
    else
        print_amber "Warning: q3a-mac.cpp did not compile successfully for macOS..."
    fi 
else
    if g++ -std=c++11 -o q3 q3a.cpp -lm; then
        print_green "q3a.cpp compiled successfully for Linux!"
        compile_success=true
        executable="./q3"
    else
        print_amber "Warning: q3a.cpp did not compile successfully for Linux..."
    fi 
fi

if [ "$compile_success" = false ]; then
    print_red "Error: Failed to compile the program."
    exit 1
fi

input_dir="q3-images/input_images"
images=()
while IFS= read -r file; do
    images+=("$file")
done < <(find "$input_dir" -name "*.pgm" | sort -R | head -n 3)

if [ ${#images[@]} -lt 3 ]; then
    print_red "Error: Not enough .pgm files found in $input_dir"
    exit 1
fi

for input_image in "${images[@]}"; do
    base_name=$(basename "$input_image" .pgm)
    blur_image="q3-images/output_images/${base_name}_blurred.pgm"
    edge_image="q3-images/output_images/${base_name}_edge_detected.pgm"

    if [ -f "$executable" ]; then
        print_amber "Processing $input_image..."
        if $executable "$input_image" "$blur_image" "$edge_image"; then
            print_green "q3a.sh executed successfully for $input_image!"
        else
            print_red "Error: q3a.sh did not execute successfully for $input_image..."
            print_amber "Skipping this image and continuing with the next..."
        fi
    else
        print_red "Error: Executable not found. Compilation may have failed."
        exit 1
    fi
done

exit 0

# make bash script executable with: chmod +x q3a.sh

: <<'END'
Use g++ to compile the C++ program q3a.cpp 
located in the q3 directory; thus creating a 
Linux-compatible executable. Include the math
library by specifying -lm in the compilation command.

I am mirroring the old q3a.sh script, only adding
if statements and print_green and print_red                     
functions.
ENDs