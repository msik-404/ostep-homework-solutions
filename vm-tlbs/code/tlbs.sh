#!/bin/bash

# Set the name of your compiled C program
program_name="./../../cmake-build-debug/vm-tlbs-tlb"

output_file="output.txt"

# Check if output_file exists
if [ -e "$output_file" ]; then
    rm "$output_file"
fi

trials=100000

# Check if the program executable exists
if [ -x "$program_name" ]; then
    for ((pages = 1; pages <= 4096; pages=pages*2)); do
        $program_name "$pages" "$trials" >> $output_file
    done
else
    echo "Error: $program_name not found."
fi