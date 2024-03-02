#!/usr/bin/env python3

import os
import sys
import argparse


def main():

    import time

    # First word
    while(1):

        start = time.time()

        while(time.time() - start < 0.5):
            print("THE ", flush=True, end='')

        # Second word
        while(time.time() - start < 1):
            print("TIME ", flush=True, end='')

        # Third word
        while(time.time() - start < 1.5):
            print("IS ", flush=True, end='')

        # Fourth word
        while(time.time() - start < 2):
            print("UP ", flush=True, end='')

        # Fifth word
        while(time.time() - start < 2.5):
            print("MAN ", flush=True, end='')

        # Sixth word
        while(time.time() - start < 3):
            print("YOU ", flush=True, end='')

        # Seventh word
        while(time.time() - start < 3.5):
            print("HAVE ", flush=True, end='')

        # Eighth word
        while(time.time() - start < 4):
            print("TO ", flush=True, end='')

        # Ninth word
        while(time.time() - start < 4.5):
            print("STOP ", flush=True, end='')

        # Tenth word
        while(time.time() - start < 5):
            print("NOW ", flush=True, end='')

if __name__ == '__main__':
    main()