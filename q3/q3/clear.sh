#!/bin/bash
rm -f image_processor # Force remove the executable file
cd q3-images/output_images # Change directory to output_images
for i in *; do
    rm -f $i # loop through all files in the directory and force remove them
done