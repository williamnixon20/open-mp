# Variables
OUTPUT_FOLDER := bin
TEST_CASE_FOLDER := test_cases
INPUT_FILE ?= $(TEST_CASE_FOLDER)/1024.txt
OUTPUT_FILE ?= output.txt

# USAGE
# To run serial program:
# make run_serial INPUT_FILE=./test_cases/1024.txt OUTPUT_FILE=output.txt
# make time_run_serial INPUT_FILE=./test_cases/1024.txt OUTPUT_FILE=output.txt

# Default target
all: serial parallel

parallel:
    @echo "TODO: Add commands for parallel compilation here"

# Compile serial
serial: $(OUTPUT_FOLDER)/serial

# Ensure output folder exists (bin) before compiling
$(OUTPUT_FOLDER)/serial: src/serial/serial.cpp | $(OUTPUT_FOLDER)
	g++ $< -o $@

$(OUTPUT_FOLDER):
	mkdir -p $@

# Run serial program
run_serial: serial
	./$(OUTPUT_FOLDER)/serial < $(INPUT_FILE) > $(OUTPUT_FILE)

time_run_serial: serial
	@time ./${OUTPUT_FOLDER}/serial < ${INPUT_FILE} > ${OUTPUT_FILE}

# Clean build
clean:
	rm -f $(OUTPUT_FOLDER)/serial $(OUTPUT_FOLDER)/parallel $(OUTPUT_FILE)
