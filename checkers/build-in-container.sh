#!/bin/bash

# Step 1: Build the Docker image
docker build -t checkers-client-builder .

# Step 2: Run the Docker container
docker run --name checkers-client-builder-container checkers-client-builder

# Step 3: Copy the compiled binary to the host
docker cp checkers-client-builder-container:/usr/src/app/build/build/bin/checkers_client ./checkers_client

# Step 4: Remove the Docker container
docker rm checkers-client-builder-container
