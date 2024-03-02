#!/bin/bash

# Check if at least two arguments are provided
if [ "$#" -lt 2 ]; then
    echo "Usage: $0 arg1 arg2 [additional arguments...]"
    exit 1
fi

# Assign arguments to variables for clarity
arg1="$1"
shift 1  # Shift to the next argument (arg2)
arg2="$@"  # Collect all remaining arguments

# Execute the command with the provided arguments
bin/sw -c "${arg1}" "${arg2}" && python3 after_countdown/boom.py
