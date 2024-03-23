#!/bin/bash

# Path to your MPI executable
MPI_EXECUTABLE="./bin"

# Path to the directory where you want to copy the MPI executable on remote servers
REMOTE_DIRECTORY="/home/k02-08"

# Read each line (hostname) from the hostfile
while read -r host; do
    echo "Copying to $host"
    # Copy MPI executable to the remote server using scp
    scp -r "$MPI_EXECUTABLE" "$host":"$REMOTE_DIRECTORY"
    scp "./hostfile" "$host":"$REMOTE_DIRECTORY"
    scp -r "./src" "$host":"$REMOTE_DIRECTORY"
    scp -r "./test_cases" "$host":"$REMOTE_DIRECTORY"
    scp "copy_mpi.sh" "$host":"$REMOTE_DIRECTORY"
    echo "Copying to $host"
done < hostfile
