#!/bin/bash

# Set the name of your compiled C program
program=$1
output_file=$2

path="./../cmake-build-debug/"
program_name="$path$program"

# Check if output_file exists
if [ -e "$output_file" ]; then
    rm "$output_file"
fi

work=100000000

# Check if the program executable exists
if [ -x "$program_name" ]; then
    for ((threads = 1; threads <= 16; threads++)); do
        $program_name "$threads" "$work" >> $output_file
    done
else
    echo "Error: $program_name not found."
fi