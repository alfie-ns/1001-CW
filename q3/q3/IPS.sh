#!/bin/bash

# ---------------------------------------------------------------------------------------------------------------------------
# RUN ONLY ON LINUX(only saying this because Windows breaks it)
#         _  __ _                     
#   __ _ | |/ _(_) ___       _ __  ___
#  / _` || | |_| |/ _ \_____| '_ \/ __|
# | (_| || |  _| |  __/_____| | | \__ \
#  \__,_||_|_| |_|\___|     |_| |_|___/
#

# Title: Image Processing Script(IPS)
# Language: Bash
# Description: This is an indefinitely running menu-driven script that allows the user to process, view, and remove images.
# GitHub: https://github.com/alfie-ns/1001-cw

# ---------------------------------------------------------------------------------------------------------------------------

# 🔴Vidbriefs-desktop:  https://github.com/alfie-ns/VidBriefs-Desktop
#                      ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
# Youtube video loader and conversational AI content advisor Python script.
# This python script is very easy to setup and run; just follow the readme.md instructions.

# ---------------------------------------------------------------------------------------------------------------------------

# Usage: Clone the repo to run the IPS script. Ensure .pgm images are in 'q3-images/input_images' directory.
# https://github.com/alfie-ns/1001-CW

# Functions ------------------------------------------------------------------------------------------------------------------

# Formatting functions: '%b' to interpret newlines
print_red() { tput setaf 1; printf "%b\n" "$1"; tput sgr0; } # Error
print_green() { tput setaf 2; printf "%b\n" "$1"; tput sgr0; } # Success
print_yellow() { tput setaf 3; printf "%b\n" "$1"; tput sgr0; } # Warning
print_blue() { tput setaf 4; printf "%b\n" "$1"; tput sgr0; } # Important
print_bold() { tput bold; printf "%b\n" "$1"; tput sgr0; } # Emphasis
# tput bold == bold test
# tput sgr0; == reset all attributes, so text is not bold anymore after
# tput setaf {n}; == colour codes

# Error checking function
check_command() {
    # Check if the command provided as the first argument ($1) is NOT available in system PATH
    if ! command -v $1 &> /dev/null; then # &> redirects both stdout and stderr to the null, discarding output if this fails
        print_red "Error: $1 is not installed or not in PATH." # Print error message with first argument
        return 1 #failure
    fi #end if also if command succeeds
    return 0 #success
}

# Progress bar function
show_progress() {
    local pid=$1 # Process ID (first argument) to monitor
    local duration=${2:-3} # Default to 3 seconds if not specified
    local steps=20 # Number of steps in the progress bar
    local delay=$(bc <<< "scale=3; $duration / $steps") # Calculate delay per step
    # loop will continue to execute as long as the process with the given PID ($pid) is still running
    while kill -0 $pid 2>/dev/null; do # While process is NOT dead, 2nd argument redirects errors to /dev/null(no output)
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
    # 20 hashtags/steps
}

process_images() {
    clear # Clear the terminal first
    print_blue "\nProcessing images..." # Print message in blue
    
    # for each file in the list, check if it exists in the current directory
    for file in "q3b-mac.cpp" "q3b.cpp"; do
        if [ ! -f "$file" ]; then # if negated found/NOT found
            print_red "Error: $file not found in the current directory."
            print_yellow "Current directory contents:"
            ls -la # List all files in the current directory
            return 1 #failure
        fi
    done
    
    # Determine compiler and source file
    if check_command clang++; then
        compiler="clang++"
        source_file="q3b-mac.cpp"
    elif check_command g++; then
        compiler="g++"
        source_file="q3b.cpp"
    else
        print_red "Error: Neither clang++ nor g++ is installed or in PATH."
        return 1
    fi
    
    # Compile the program, using previously initialised veriables
    if ! $compiler -std=c++17 $source_file -o image_processor -O3 -lm; then
        print_red "Error: Compilation failed with $compiler."
        return 1 #failure
    fi
    
    print_green "Compilation successful using $compiler."
    
    # Create output directory if it doesn't exist
    mkdir -p q3-images/output_images
    
    # Run the image processor
    ./image_processor q3-images/input_images q3-images/output_images &
    show_progress $! 2 & # Start process, show progress bar for 2 seconds, and run in background(&)  
    wait $! # Wait for the process to finish
    sleep 3 

    
    # Check if any output was produced
    if [ -z "$(ls -A q3-images/output_images)" ]; then # If output directory is empty
        print_red "Warning: No output images were produced."
    else
        print_green "\nImage processing complete.\n"
    fi
    
    return 0 #success
}

view_images() {
    clear # 1. Clear the terminal first
    print_blue "\nAttempting to open output images..." # 2. Print message in blue
    output_dir="q3-images/output_images" # 3. Set output directory
    
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
# function to remove output images
remove_images() {
    clear # Clear the terminal first
    print_yellow "Removing output images..."
    
    rm -f image_processor # Remove the compiled program
    if [ -d "q3-images/output_images" ]; then # If output directory exists
        rm -f q3-images/output_images/* # Remove all files in output directory
        print_green "Output images removed."
    else
        print_yellow "Output directory not empty. Nothing to remove."
    fi
    return 0 #success
}

show_menu() {
    printf "%s\n" "$(print_bold "    🖼️  Image Processing Menu 🖼️")"
    printf "%s\n" "$(print_yellow "    1) Process images")"
    printf "%s\n" "$(print_yellow "    2) View output images")"
    printf "%s\n" "$(print_yellow "    3) Remove output images")"
    #printf "%s\n" "$(print_yellow "    4) Help")"
    printf "%s\n" "$(print_yellow "    4) Exit")"
}
# function to show help information
#show_help() {
#    clear
#    printf "%s\n" "$(print_bold "\n📘 Help Information:")"
#    printf "%s\n" "$(print_blue "1. Process images:") Compiles and runs the image processing program."
#    printf "%s\n" "$(print_blue "2. View output images:") Opens processed images and shows statistics."
#    printf "%s\n" "$(print_blue "3. Remove output images:") Clears all processed images."
#    printf "%s\n" "$(print_blue "4. Help:") Displays this help information."
#    printf "%s\n" "$(print_blue "5. Exit:") Quits the program and cleans up."
#    printf "\n%s\n" "$(print_yellow "Note:") Ensure input images are in 'q3-images/input_images' directory."
#}

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
    read -p "$(print_bold "Enter your choice (1-4): ")" choice # Read user input
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
        4)  # If user input is 4 exit while clearing images
            clear
            print_bold "\nExiting...\n"
            remove_images
            print_green "Cleaned up output images."
            sleep 3
            clear
            exit 0
            ;;
        *) # If user input is anything else apart from 1-4
            clear
            print_red "\nInvalid option. Please choose a number between 1 and 5."
            ;;
    esac # End case statement
    echo # Print newline
done

#4) show_help ;; # If user input is 4 show help