#!/usr/bin/env python
import os
import sys
import subprocess

if __name__ == '__main__':
    import argparse
    # Argument parser.
    description = '''Build packages locally'''
    parser = argparse.ArgumentParser(description=description)
    parser.add_argument('--build-locally', '-d', metavar='design_directory'
            , nargs=1
            , action='append'
            , required = True
            , help = 'A generic option'
            )
    parser.add_argument('--compiler', '-c'
            , metavar='compiler'
            , required = False
            , help = "Optional: Specify a compiler or I'll try to guess on system"
            )
    class Args: pass 
    args = Args()
    parser.parse_args(namespace=args)


    main()
