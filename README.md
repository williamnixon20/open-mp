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

#### CUDA
1. To run the cuda program, copy the ipynb file to google colab
2. Run all the codes from google colab

#### Performance Benchmark (Bonus)
- Running Environment (Non-CUDA): Linux - Ubuntu 22.04 LTS - Intel® Core™ i3-1115G4 - RAM 12 GB

- Running Environment (CUDA): Google Colab - T4 GPU

| Metric     | Serial   | OpenMPI | OpenMP   | CUDA     |
|------------|----------|---------|----------|----------|
| N          |          |         |          |          |
| 64         | 0m0.01s  | 0m0.01s | 0m0.01s  | 0m0.024s |
| 128        | 0m0.06s  | 0m0.02s | 0m0.10s  | 0m0.038s |
| 256        | 0m0.250s | 0m0.03s | 0m0.31s  | 0m0.074s |
| 512        | 0m1.770s | 0m0.11s | 0m1.54s  | 0m0.227s |
| 1024       | 0m12.93s | 0m0.38s | 0m12.16s | 0m1.041s |
| 2048       | 1m8.845s | 0m1.36s | 1m45.22s | 0m5.772s |



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
