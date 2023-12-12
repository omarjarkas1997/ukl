#!/bin/bash

# # Change to the seed/ directory
# cd erun/
# Clean previous build
make clean

# Return to the parent directory
cd ..

# Run autoreconf with the -i flag
autoreconf -i

# Configure with the specified program
./configure --with-program=hello

# Compile using all available cores
make -j$(nproc)

# Boot
make boot-priv
