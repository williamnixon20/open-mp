# Paralel Inverse Matrix

Parallelizing matrix inverse using OPEN_MPI, OPEN_MP, and CUDA.

## How to Run

### Compiling
1. Be at root folder
2. Run `make all`
3. Program should be compiled and put into /bin. Verify that /bin/open-mpi and /bin/serial exists.

### Running

#### Serial
1. To run serial program, run `make time_run_serial`
2. To modify the input, either change the `INPUT_FILE` parameter in makefile, or change through the arguments like so `make time_run_serial INPUT_FILE=./test_cases/1024.txt OUTPUT_FILE=output.txt`
3. Output should be in the root folder.

#### Parallel
1. To run the parallel program, run `make time_run_open_mpi`
2. To modify the input, either change the `INPUT_FILE` parameter in makefile, or change through the arguments like in Serial.
3. Output should be in the root folder.

#### Open-MP
1. To run the open-mp program, run `make time_run_open_mp`
2. To modify the input, either change the `INPUT_FILE` parameter in makefile, or change through the arguments like in Serial.
3. Output should be in the root folder.

### Running in server
1. Go through setup of SSH-ing to a server with `k02-08`'s key. SSH to VM 1 with public IP 4.145.183.206.
2. Ls to the project's root dir, then run `git pull`.
3. Compile the program there using `make all`. This will compile both the serial + parallel program.
3. Running the serial program is straight forward, follow directions above.
4. Distribute the compiled parallel program by running `bash ./copy_mpi.sh`. This will copy the mpi executable to all hosts.
5. Run the parallel program by running `make time_run_open_mpi_server`
6. The serial program does not have enough cores. `add NUM_OF_PROCESSOR=1` when running serial.

If there's any difficulty in running the program, raise an issue and tag us! <3

Made with <3 by NVIDIA STOOONKS
