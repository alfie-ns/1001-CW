#!/bin/bash
rm -f image_processor # Force remove the executable file
cd q3-images/output_images # Change directory to output_images
for i in *; do
    rm -f $i # Force remove all files in output_images
done