// mpicc mpi.c -o mpi
// mpirun -np 4 ./mpi
// mpirun -np 4 ./mpi < ../../test_cases/2.txt
// time mpirun -np 4 ./mpi < ../../test_cases/512.txt

// RUNNING SERIAL PROGRAM
// time cat ./test_cases/512.txt | ./bin/serial > 512.txt

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

void read_matrix(double *matrix, int N)
{
    // Read the matrix from stdin
    for (int i = 0; i < N; ++i)
    {
        for (int j = 0; j < N; ++j)
        {
            // printf("Reading element %d\n", i * 2 * N + j);
            if (scanf("%lf", &matrix[i * 2 * N + j]) != 1)
            {
                fprintf(stderr, "Error reading matrix element at position (%d, %d).\n", i, j);
                exit(1);
            }
        }
    }
}

void print_matrix(double *matrix, int num_row, int num_col)
{
    if (num_row > 100)
    {
        printf("Matrix too large to print\n");
        printf("Logging it instead..");
        return;
    }
    // Print the matrix
    printf("Matrix:\n");
    for (int i = 0; i < num_row; ++i)
    {
        for (int j = 0; j < num_col; ++j)
        {
            // printf("\n%d, %d\n", i, j);
            printf("%lf ", matrix[i * num_col + j]);
        }
        printf("\n");
    }
}

void initialize_identity(double *matrix, int N)
{
    // Initialize the identity matrix
    for (int i = 0; i < N; ++i)
    {
        for (int j = N; j < 2 * N; ++j)
        {
            if (i == j - N)
            {
                matrix[i * 2 * N + j] = 1;
            }
            else
            {
                matrix[i * 2 * N + j] = 0;
            }
        }
    }
}

void save_file(double *matrix, int N)
{
    char filename[100]; // Adjust the size according to your needs
    snprintf(filename, sizeof(filename), "result_inverse_matrix_%d.txt", N);

    // Open the file
    FILE *file = fopen(filename, "w");
    if (file != NULL)
    {
        fprintf(file, "%d\n", N);
        for (int i = 0; i < N; i++)
        {
            for (int j = N; j < 2 * N; j++)
            {
                fprintf(file, "%f ", matrix[i * (2 * N) + j]);
            }
            fprintf(file, "\n");
        }
        fclose(file);
        printf("Result matrix logged to file.\n");
    }
    else
    {
        printf("Failed to open file for logging.\n");
    }
}

int main(void)
{
    MPI_Init(NULL, NULL);
    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int N;
    double *matrix = NULL;

    // Flat matrix of size 2 * N * N, for ease of communication
    // Need to specify offset yourself when accessing elements
    // For example, matrix[i*2*N + j] instead of matrix[i][j]
    // 2 * N because our matrix is rectangular [N rows, 2N columns]
    if (world_rank == 0)
    {
        // Process 0 reads from stdin
        if (scanf("%d\n", &N) == 1)
        {
            printf("N Matrix: : %d\n", N);
            printf("Mallocing matrix of size %lu\n", (2 * N) * N * sizeof(double));
            matrix = malloc((2 * N) * N * sizeof(double));
            if (matrix == NULL)
            {
                fprintf(stderr, "Malloc error for matrix! Aborting.. \n");
                MPI_Abort(MPI_COMM_WORLD, 1);
            }
            read_matrix(matrix, N);
            initialize_identity(matrix, N);
            // print_matrix(matrix, N, 2 * N);
        }
    }
    // Kasih tau size ke semua rank lain
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int n_rows = N / world_size;
    int n_cols = 2 * N;
    int start_row = world_rank * n_rows;
    int end_row = start_row + n_rows;

    double *current_matrix = malloc((2 * N) * n_rows * sizeof(double));
    printf("[%d/%d] N: %d N_rows: %d Start_Row: %d End_Rows: %d\n", world_rank, world_size, N, n_rows, start_row, end_row);
    if (current_matrix == NULL)
    {
        fprintf(stderr, "Malloc error for current_matrix! in process rank %d. Aborting.. \n", world_rank);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    MPI_Scatter(matrix, 2 * N * n_rows, MPI_DOUBLE, current_matrix,
                2 * N * n_rows, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    for (int row = 0; row < N; row++) 
    {
        int mapped_rank = row / n_rows;

        if (world_rank == mapped_rank) 
        {
            int local_row = row % n_rows;

            double pivot = current_matrix[(row % n_rows) * n_cols + row];
            
            // For each column, divide by the leading pivot number
            for (int col = row; col < n_cols; col++)
            {
                current_matrix[local_row * n_cols + col] /= pivot;
            }

            // For each row below we want to remove the leading number until it gets zero
            for (int elim_row = 0; elim_row < n_rows; elim_row++)
            {
                // If the row is the local_row then skip
                if (elim_row == local_row)
                {
                    continue;
                }

                // Scale amount
                double scale = current_matrix[elim_row * n_cols + row];
                
                // For each column in the row
                for (int col = row; col < n_cols; col++)
                {
                    // For every number (col) in the elim row we minus it with
                    // The scale of the pivot row 
                    current_matrix[elim_row * n_cols + col] -=
                        current_matrix[local_row * n_cols + col] * scale;
                }
            }
        }
    }


    MPI_Gather(current_matrix, 2 * N * n_rows, MPI_DOUBLE, matrix, 2 * N * n_rows,
               MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (world_rank == 0)
    {
        save_file(matrix, N);
    }

    //  No need to free to save time lol

    MPI_Finalize();

    return 0;
}
