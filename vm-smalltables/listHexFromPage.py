#!/usr/bin/python

import sys

ERROR_MESSAGE = "Input should be a string composed of concatenated 32 bytes in hex form. like so: 7f7f7f..7f"

if __name__ == "__main__":
    if len(sys.argv) != 2:
        raise RuntimeError(ERROR_MESSAGE)

    hex_page = sys.argv[1]

    if len(hex_page) != 64:
        raise RuntimeError(ERROR_MESSAGE)

    for i in range(0, len(hex_page) - 2, 2):
        print (i//2, hex_page[i: i+2])
