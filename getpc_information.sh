#!/bin/bash

echo "CPU Information:"
lscpu | grep -E 'Model name|CPU\(s\)|MHz|Thread|Core'
echo

echo "Memory Information:"
free -h
echo

echo "Operating System Information:"
lsb_release -a

