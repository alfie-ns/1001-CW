#!/bin/bash

# to run this menu script clone: https://github.com/alfie-ns/1001-CW
# ---------------------------------------------------------------

# Formatting functions
print_bold() { tput bold; echo -e "$1"; tput sgr0; } # tput sgr0 resets text formatting
# tput setaf = color codes
print_blue() { tput setaf 4; echo -e "$1"; tput sgr0; } 
print_green() { tput setaf 2; echo -e "$1"; tput sgr0; }
print_red() { tput setaf 1; echo -e "$1"; tput sgr0; }
print_yellow() { tput setaf 3; echo -e "$1"; tput sgr0; }

# Error checking function
check_command() {
    # Check if the command provided as the first argument ($1) is NOT available in system PATH
    if ! command -v $1 &> /dev/null; then # &> redirects both stdout and stderr
        print_red "Error: $1 is not installed or not in PATH." # Print error message with first argument
        return 1 # Return 1 to indicate failure
    fi # if command succeeds,
    return 0 # Return 0 to indicate success
}

# Progress bar function
show_progress() {
    local pid=$1 # Process ID (first argument) to monitor
    local duration=${2:-3}  # Default to 3 seconds if not specified
    local steps=20 # Number of steps in the progress bar
    local delay=$(bc <<< "scale=3; $duration / $steps") # Calculate delay per step
    
    while kill -0 $pid 2>/dev/null; do # While process is NOT dead
        for ((i=0; i<=steps; i++)); do # Loop through steps one-by-one
            printf "\rProcessing: [" # Print start of progress bar
            for ((j=0; j<i; j++)); do printf "#"; done # Print # for each step
            for ((j=i; j<steps; j++)); do printf " "; done # Print space for remaining steps
            printf "] %d%%" $(( i * 100 / steps )) # Print percentage
            sleep $delay # Sleep for delay each iteration
        done
        printf "\r"  # Move cursor back to start of line for next step/iteration
    done
    printf "Processing: [####################] 100%%\n"
}

process_images() {
    print_blue "\nProcessing images..."
    
    # Check if the source file exists
    if [ ! -f "q3b-mac.cpp" ]; then
        print_red "Error: q3b-mac.cpp not found in the current directory."
        return 1
    fi
    
    # Check if clang++ is installed
    if ! check_command clang++; then
        return 1
    fi
    
    # Compile the program
    if ! clang++ -std=c++17 q3b-mac.cpp -o image_processor -O3 -lm; then
        print_red "Error: Compilation failed."
        return 1
    fi
    
    # Make sure input directory exists and is not empty
    if [ ! -d "q3-images/input_images" ] || [ -z "$(ls -A q3-images/input_images)" ]; then
        print_red "Error: Input directory is missing or empty."
        return 1
    fi
    
    # Create output directory if it doesn't exist
    mkdir -p q3-images/output_images # if q3-images/putput_images DOESN'T exist, don't through an error and create it
    
    # Run the image processor
    ./image_processor q3-images/input_images q3-images/output_images &
    show_progress $image_processor_pid 2 & 
    wait $image_processor_pid # wait for image_processor to finish
    
    # Check if any output was produced
    if [ -z "$(ls -A q3-images/output_images)" ]; then
        print_yellow "Warning: No output images were produced."
    else
        print_green "\nImage processing complete.\n"
    fi
    
    return 0
}

view_images() {
    print_blue "\nAttempting to open output images..."
    output_dir="q3-images/output_images"
    
    if [ ! -d "$output_dir" ]; then
        print_red "Error: Output directory $output_dir does not exist."
        return 1
    fi
    
    image_count=$(find "$output_dir" -name "*.pgm" | wc -l) # count number of PGM images in output_images
    if [ "$image_count" -eq 0 ]; then
        print_yellow "No PGM images found in $output_dir."
        return 1
    fi
    
    # Check if 'open' command is available (for macOS)
    if ! check_command open; then
        print_yellow "Warning: 'open' command not available. Images won't be displayed automatically."
    else
        open "$output_dir"/*.pgm
    fi
    
    print_green "Found $image_count image(s)."
    
    return 0 # success
}

remove_images() {
    print_yellow "\nRemoving output images..."
    
    if [ ! -f "./clear.sh" ]; then
        print_red "Error: clear.sh script not found in the current directory."
        return 1 # failure
    fi
    
    if ! ./clear.sh; then # if script failts to run
        print_red "Error: Failed to remove images."
        return 1 # failure
    fi
    
    print_green "Output images removed."
    return 0 # success
}

show_menu() {
    cat << EOF
$(print_bold "    🖼️  Image Processing Menu 🖼️")
$(print_yellow "    1) Process images")
$(print_yellow "    2) View output images")
$(print_yellow "    3) Remove output images")
$(print_yellow "    4) Help")
$(print_yellow "    5) Exit")
EOF
}

show_help() {
    cat << EOF
$(print_bold "\n📘 Help Information:")
$(print_blue "1. Process images:") Compiles and runs the image processing program.
$(print_blue "2. View output images:") Opens processed images and shows statistics.
$(print_blue "3. Remove output images:") Clears all processed images.
$(print_blue "4. Help:") Displays this help information.
$(print_blue "5. Exit:") Quits the program and cleans up.

$(print_yellow "Note:") Ensure input images are in 'q3-images/input_images' directory.
EOF
}

# ASCII Art
print_ascii_art() {
    cat << "EOF"
-----------------------------------------
| ⚙️ Image Processor ⚙️                    |
-----------------------------------------
|         _  __ _                        |
|   __ _ | |/ _(_) ___       _ __  ___   | 
|  / _` || | |_| |/ _ \_____| '_ \/ __|  |
| | (_| || |  _| |  __/_____| | | \__ \  |
|  \__,_||_|_| |_|\___|     |_| |_|___/  |
 ----------------------------------------
EOF
}

# Main
clear # clear terminal
print_ascii_art # print title
print_bold "\nWelcome to the Image Processing Script"
while true; do
    echo
    show_menu
    read -p "$(print_bold "\nEnter your choice (1-5): ")" choice
    case $choice in
        1) 
            if ! process_images; then
                print_red "Image processing encountered an error."
            fi
            ;;
        2) 
            if ! view_images; then
                print_red "Unable to view images."
            fi
            ;;
        3) 
            if ! remove_images; then
                print_red "Failed to remove images."
            fi
            ;;
        4) show_help ;;
        5) 
            print_bold "\nExiting...\n"
            if [ -f "./clear.sh" ]; then
                ./clear.sh
                print_green "q3-images/output_images cleaned."
            else
                print_yellow "Warning: clear.sh not found. Skipping cleanup."
            fi
            exit 0
            ;;
        *)
            print_red "\nInvalid option. Please choose a number between 1 and 5."
            ;;
    esac
    echo # newline for formatting
done