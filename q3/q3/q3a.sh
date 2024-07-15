#!/bin/bash

# Compile C++ program
g++ -o q3 ../../q3/main_linux.cpp -lm # -lm for the math library

# Create variables to hold the image locations. Use the full path name for each image.
input_image=../../q3-images/input_images/a1.pgm
blur_image=../../q3-images/output_images/blurred.pgm
edge_image=../../q3-images/output_images/edge_detected.pgm

# Run the program, passing the variables for the 3 image locations
./q3 $input_image $blur_image $edge_image

# Compile bash script with: chmod +x bash-script.sh

: <<'END'

        You use g++ to compile the C++ program in the q3 directory,
        main_linux.cpp for the Linux-type main.cpp file that's successfully compiled in Linux.
        Specify math library use with -lm.

END

