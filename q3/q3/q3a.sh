#!/bin/bash

# Compile C++ program
g++ -o q3 ../../q3/q3a.cpp -lm # -lm for the math library

# Create variables to hold the image locations. Use the full path name for each image.
input_image=../../q3-images/input_images/a1.pgm
blur_image=../../q3-images/output_images/blurred.pgm
edge_image=../../q3-images/output_images/edge_detected.pgm

# Run the program, passing the variables for the 3 image locations
./q3 $input_image $blur_image $edge_image

# Compile bash script with: chmod +x bash-script.sh

: <<'END'


Use g++ to compile the C++ program q3a.cpp 
located in the q3 directory, creating a 
Linux-compatible executable. Include the math
library by specifying -lm in the compilation command.

END

