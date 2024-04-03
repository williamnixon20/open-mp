# Variables
OUTPUT_FOLDER := bin
TEST_CASE_FOLDER := test_cases
INPUT_FILE ?= $(TEST_CASE_FOLDER)/1024.txt
OUTPUT_FILE ?= output_1024.txt
NUM_OF_PROCESSOR ?= 4

# USAGE
# To run serial program:
# make run_serial INPUT_FILE=./test_cases/1024.txt OUTPUT_FILE=output.txt
# make time_run_serial INPUT_FILE=./test_cases/1024.txt OUTPUT_FILE=output.txt

# Default target
all: serial open_mpi open_mp

# Compile serial
serial: $(OUTPUT_FOLDER)/serial

# Ensure output folder exists (bin) before compiling
$(OUTPUT_FOLDER)/serial: src/serial/serial.cpp | $(OUTPUT_FOLDER)
	g++ $< -o $@

# Run serial program
run_serial: serial
	./$(OUTPUT_FOLDER)/serial < $(INPUT_FILE) > $(OUTPUT_FILE)

time_run_serial: serial
	time ./$(OUTPUT_FOLDER)/serial < ${INPUT_FILE} > ${OUTPUT_FILE}

# Compile Open-MPI
open_mpi: $(OUTPUT_FOLDER)/open-mpi
$(OUTPUT_FOLDER)/open-mpi: src/open-mpi/mpi.c | $(OUTPUT_FOLDER)
	mpicc $< -o $@

# Run open-mpi program
run_open_mpi: open_mpi
	mpirun -np ${NUM_OF_PROCESSOR} ./$(OUTPUT_FOLDER)/open-mpi < ${INPUT_FILE}

time_run_open_mpi: open_mpi
	time mpirun -np ${NUM_OF_PROCESSOR} ./$(OUTPUT_FOLDER)/open-mpi < ${INPUT_FILE}

time_run_open_mpi_server: open_mpi
	time mpirun -np ${NUM_OF_PROCESSOR} --hostfile hostfile ./${OUTPUT_FOLDER}/open-mpi < ${INPUT_FILE}

# Compile Open-MP
open_mp: $(OUTPUT_FOLDER)/open-mp
$(OUTPUT_FOLDER)/open-mp: src/open-mp/mp.c | $(OUTPUT_FOLDER)
	gcc -fopenmp $< -o $@

# Run open-mp program
run_open_mp: open_mp
	$(OUTPUT_FOLDER)/open-mp < ${INPUT_FILE} > ${OUTPUT_FILE}
	
time_run_open_mp: open_mp
	time ./$(OUTPUT_FOLDER)/open-mp < ${INPUT_FILE}
	
# Make the output folder
$(OUTPUT_FOLDER):
	mkdir -p $@

# Clean build
clean:
	rm -f $(OUTPUT_FOLDER)/serial $(OUTPUT_FOLDER)/open-mpi $(OUTPUT_FILE)
