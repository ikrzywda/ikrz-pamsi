# Generated with ChatGPT

FROM ubuntu:22.04

# Set the working directory in the container
WORKDIR /usr/src/app

# Copy the current directory contents into the container at /usr/src/app
COPY . .

# Install CMake
RUN apt-get update && apt-get install -y cmake && apt-get install -y g++

# Create a build directory
RUN mkdir -p build

# Ensure the build directory is clean
RUN rm -rf build && mkdir -p build

# Change the working directory to build
WORKDIR /usr/src/app/build

# Configure the project
RUN cmake ..

# Build the project
RUN make

