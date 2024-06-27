#!/bin/bash

# Directory to clear
OUTPUT_DIR="q3/q3/q3-images/output_images"

# Check if the directory exists
if [ -d "$OUTPUT_DIR" ]; then
  # Loop through each file in the directory
  for file in "$OUTPUT_DIR"/*; do
    # Check if it's a file before attempting to remove it
    if [ -f "$file" ]; then
      rm "$file"
      echo "Removed $file"
    fi
  done
  echo "All files in $OUTPUT_DIR have been cleared."
else
  echo "Directory $OUTPUT_DIR does not exist."
fi