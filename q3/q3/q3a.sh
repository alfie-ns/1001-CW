#!/bin/bash

print_red() { tput setaf 1; echo -e "$1"; tput sgr0; }
print_green() { tput setaf 2; echo -e "$1"; tput sgr0; }
print_amber() { tput setaf 3; echo -e "$1"; tput sgr0; }

compile_success=false # flag to check if compilation was successful

if [[ "$OSTYPE" == "darwin"* ]]; then # if macOS
    if clang++ -std=c++11 -o q3_mac q3a-mac.cpp; then # run mac-compatible C++ program
        print_green "q3a-mac.cpp compiled successfully for macOS!"
        compile_success=true 
        executable="./q3_mac"
    else
        print_amber "Warning: q3a-mac.cpp did not compile successfully for macOS..."
    fi 
else
    if g++ -std=c++11 -o q3 q3a.cpp -lm; then # if ran on g++-compatible c++ program
        print_green "q3a.cpp compiled successfully for Linux!"
        compile_success=true
        executable="./q3"
    else
        print_red "Error: Could not compile q3a"
        exit 1 #failure
    fi 
fi

if [ "$compile_success" = false ]; then # if compile_success was never set to true
    print_red "Error: Failed to compile the program."
    exit 1
fi

input_dir="q3-images/input_images"
images=() # init empty array to store selected PGM files

# While proccessing 3 random PGM files, add them to the images array
while IFS= read -r file; do # use IFS to essentially make sure to ensure all critical data is processed; read all as one line
    images+=("$file")
done < <(find "$input_dir" -name "*.pgm" | sort -R | head -n 3) # sort randomly and find 3 .pgm files

if [ ${#images[@]} -lt 3 ]; then # Error check: if less than 3 PGM files found
    print_red "Error: Not enough PGM files found in $input_dir"
    exit 1
fi

for input_image in "${images[@]}"; do # iterate through .pgm files
    # Define output file paths to create blurred and edge-detected images
    base_name=$(basename "$input_image" .pgm)
    blur_image="q3-images/output_images/${base_name}_blurred.pgm"
    edge_image="q3-images/output_images/${base_name}_edge_detected.pgm"

    if [ -f "$executable" ]; then # if executable exists, begin processing
        print_amber "Processing $input_image..."
        if $executable "$input_image" "$blur_image" "$edge_image"; then # run executable with input image and output file paths
            print_green "q3a.sh executed successfully for $input_image!"
        else
            print_red "Error: q3a.sh did not execute successfully for $input_image..."
            print_amber "Skipping this image; continuing with the next..."
        fi
    else
        print_red "Error: Executable not found. Compilation may have failed."
        exit 1
    fi
done

exit 0

# make bash script executable with: chmod +x q3a.sh

: <<'END'
q3a.sh compiles the C++ program q3a.cpp or q3a-mac.cpp depending on the operating system 
(Linux or macOS, respectively). It uses g++ for Linux, including the math library by specifying 
-lm, and clang++ for macOS development.

The script then selects 3 random PGM files from the 'q3-images/input_images' directory.
For each selected image, it processes the image using the compiled executable and 
generates two output images: a blurred version and an edge-detected version. These 
output images are saved in the 'q3-images/output_images' directory with the appropriate suffixes.

The script also includes error handling, printing messages in different colours 
(green for success, amber for warnings, and red for errors) using the functions:
print_green, print_amber, and print_red.
END