#!/bin/bash

# Title: Image Processing Script(IPS)
# Description: This is an indefinitely running menu-driven script that allows the user to process, view, and remove images.
# Author: Alfie Nurse 
# GitHub: https://github.com/alfie-ns/1001-cw

#         _  __ _                     
#   __ _ | |/ _(_) ___       _ __  ___
#  / _` || | |_| |/ _ \_____| '_ \/ __|
# | (_| || |  _| |  __/_____| | | \__ \
#  \__,_||_|_| |_|\___|     |_| |_|___/
#  

# ---------------------------------------------------------------------------------------------------------------------------

# Usage: Clone the repo to run the IPS script. Ensure .pgm images are in 'q3-images/input_images' directory.

# Functions ------------------------------------------------------------------------------------------------------------------

# Formatting functions
print_bold() { tput bold; printf "%s\n" "$1"; tput sgr0; } # tput bold == bold test
print_blue() { tput setaf 4; printf "%s\n" "$1"; tput sgr0; }
print_green() { tput setaf 2; printf "%s\n" "$1"; tput sgr0; }
print_red() { tput setaf 1; printf "%s\n" "$1"; tput sgr0; }
print_yellow() { tput setaf 3; printf "%s\n" "$1"; tput sgr0; }
# tput sgr0; == reset all attributes, so text is not bold anymore at the end
# tput setaf {n}; == colour codes

# Error checking function
check_command() {
    # Check if the command provided as the first argument ($1) is NOT available in system PATH
    if ! command -v $1 &> /dev/null; then # &> redirects both stdout and stderr
        print_red "Error: $1 is not installed or not in PATH." # Print error message with first argument
        return 1 #failure
    fi #end if - if command succeeds
    return 0 #success
}

# Progress bar function
show_progress() {
    local pid=$1 # Process ID (first argument) to monitor
    local duration=${2:-3} # Default to 3 seconds if not specified
    local steps=20 # Number of steps in the progress bar
    local delay=$(bc <<< "scale=3; $duration / $steps") # Calculate delay per step
    # loop will continue to execute as long as the process with the given PID ($pid) is still running
    while kill -0 $pid 2>/dev/null; do # While process is NOT dead
        for ((i=0; i<=steps; i++)); do # Loop through steps one-by-one
            printf "\rProcessing: [" # Print start of progress bar
            for ((j=0; j<i; j++)); do printf "#"; done # Print # for each step
            for ((j=i; j<steps; j++)); do printf " "; done # Print spaces for remaining steps
            printf "] %d%%" $(( i * 100 / steps )) # Print percentage
            sleep $delay # Sleep delay for each iteration
        done
        printf "\r" # Move cursor back to start of line for next step/iteration
    done
    printf "Processing: [####################] 100%%\n"
}

process_images() {
    clear # Clear the terminal first
    print_blue "\nProcessing images..." # Print message in blue
    
    # Check if the source file exists
    if [ ! -f "q3b-mac.cpp" ]; then
        print_red "Error: q3b-mac.cpp not found in the current directory."
        print_yellow "Current directory contents:"
        ls -la # List all files in the current directory, -la for long listing(permissions, size, etc.)
        return 1 #failure
    fi
    
    if check_command clang++; then # If clang++ is available(Mac OS)
        compiler="clang++" # Set compiler to clang++
    elif check_command g++; then # If g++ is available(Linux)
        compiler="g++"
    else
        print_red "Error: Neither clang++ nor g++ is installed or in PATH."
        return 1
    fi
    
    # Compile the program
    if ! clang++ -std=c++17 q3b-mac.cpp -o image_processor -O3 -lm; then # If cannot compile
        # -std=c++17: Use C++17 standard, -o: Output file name, -O3: Optimization level 3, -lm: Link math library
        print_red "Error: Compilation failed." # Print error message
        return 1 #failure
    fi
    
    # Make sure input directory exists and is not empty
    if [ ! -d "q3-images/input_images" ] || [ -z "$(ls -A q3-images/input_images)" ]; then
        print_red "Error: Input directory is missing or empty."
        return 1 #failure
    fi
    
    # Create output directory if it doesn't exist
    mkdir -p q3-images/output_images
    
    # Run the image processor
    ./image_processor q3-images/input_images q3-images/output_images &
    show_progress $! 2 & # Start process, show progress bar for 2 seconds, and run in background  
    wait $! # Wait for the process to finish
    
    # Check if any output was produced
    if [ -z "$(ls -A q3-images/output_images)" ]; then # If output directory is empty
        print_red "Warning: No output images were produced."
    else
        print_green "\nImage processing complete.\n"
    fi
    
    return 0 #success
}

view_images() {
    clear # Clear the terminal first
    print_blue "\nAttempting to open output images..."
    output_dir="q3-images/output_images" # Set output directory
    
    if [ ! -d "$output_dir" ]; then # If output directory does not exist
        print_red "Error: Output directory $output_dir does not exist."
        return 1 #failure
    fi
    
    image_count=$(find "$output_dir" -name "*.pgm" | wc -l) # Count number of PGM images in output directory
    if [ "$image_count" -eq 0 ]; then # if count==0
        print_yellow "No PGM images found in $output_dir." # Print warning message
        return 1 #failure
    fi
    
    if check_command open; then # If open command is available(Mac OS)
        open "$output_dir"/*.pgm # Open all PGM images in output directory
    elif check_command xdg-open; then # If xdg-open command is available(Linux)
        xdg-open "$output_dir"/*.pgm # Open all PGM images in output directory
    elif check_command display; then # If display command is available(Linux)
        display "$output_dir"/*.pgm # Open all PGM images in output directory
    else # If no suitable command found
        print_yellow "Warning: No suitable command found to open images automatically."
        print_yellow "You can find the images in the $output_dir directory."
    fi
    
    print_green "Found $image_count image(s)."
    return 0 #success
}

remove_images() {
    clear # Clear the terminal first
    print_yellow "\nRemoving output images..."
    
    rm -f image_processor # Remove the compiled program
    if [ -d "q3-images/output_images" ]; then # If output directory exists
        rm -f q3-images/output_images/* # Remove all files in output directory
        print_green "Output images removed."
    else
        print_yellow "Output directory not found. Nothing to remove."
    fi
    return 0 #success
}

show_menu() {
    printf "%s\n" "$(print_bold "    🖼️  Image Processing Menu 🖼️")"
    printf "%s\n" "$(print_yellow "    1) Process images")"
    printf "%s\n" "$(print_yellow "    2) View output images")"
    printf "%s\n" "$(print_yellow "    3) Remove output images")"
    printf "%s\n" "$(print_yellow "    4) Help")"
    printf "%s\n" "$(print_yellow "    5) Exit")"
}
# function to show help information
show_help() {
    clear
    printf "%s\n" "$(print_bold "\n📘 Help Information:")"
    printf "%s\n" "$(print_blue "1. Process images:") Compiles and runs the image processing program."
    printf "%s\n" "$(print_blue "2. View output images:") Opens processed images and shows statistics."
    printf "%s\n" "$(print_blue "3. Remove output images:") Clears all processed images."
    printf "%s\n" "$(print_blue "4. Help:") Displays this help information."
    printf "%s\n" "$(print_blue "5. Exit:") Quits the program and cleans up."
    printf "\n%s\n" "$(print_yellow "Note:") Ensure input images are in 'q3-images/input_images' directory."
}

print_ascii() {
    cat << "EOF"
         _  __ _                     
   __ _ | |/ _(_) ___       _ __  ___
  / _` || | |_| |/ _ \_____| '_ \/ __|
 | (_| || |  _| |  __/_____| | | \__ \
  \__,_||_|_| |_|\___|     |_| |_|___/
  
EOF
}

# Main -----------------------------------------------------------------------------------------------------------------------
clear # Clear the terminal first
print_ascii # Print ASCII art
print_bold "C++ Image Processing Script" # Print bold title
while true; do # Infinite loop
    echo # Print newline
    show_menu # Show menu
    read -p "$(print_bold "\nEnter your choice (1-5): ")" choice # Read user input
    case $choice in # Case statement for user input
        1) # If user input is 1 process images
            if ! process_images; then
                print_red "Image processing encountered an error."
            fi
            ;;
        2) # If user input is 2 view images 
            if ! view_images; then
                print_red "Unable to view images."
            fi
            ;;
        3) # If user input is 3 remove images 
            if ! remove_images; then
                print_red "Failed to remove images."
            fi
            ;;
        4) show_help ;; # If user input is 4 show help
        5)  # If user input is 5 exit while clearing images
            clear
            print_bold "\nExiting...\n"
            remove_images
            print_green "Cleaned up output images."
            sleep 3
            clear
            exit 0
            ;;
        *) # If user input is anything else apart from 1-5
            clear
            print_red "\nInvalid option. Please choose a number between 1 and 5."
            ;;
    esac # End case statement
    echo # Print newline
done