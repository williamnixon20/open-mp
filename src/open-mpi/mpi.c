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
        int curr_offset = i * 2 * N;
        for (int j = 0; j < N; ++j)
        {
            // printf("Reading element %d\n", i * 2 * N + j);
            if (scanf("%lf", &matrix[curr_offset + j]) != 1)
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
        int curr_offset = i * 2 * N;
        for (int j = N; j < 2 * N; ++j)
        {
            if (i == j - N)
            {
                matrix[curr_offset + j] = 1;
            }
            else
            {
                matrix[curr_offset + j] = 0;
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
            int curr_offset = i * 2 * N;
            for (int j = N; j < 2 * N; j++)
            {
                fprintf(file, "%f ", matrix[curr_offset + j]);
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

void iterate_and_eliminate(double *matrix, double *pivot_row, int n_rows, int n_cols, int row)
{
    for (int iter_row = 0; iter_row < n_rows; iter_row++)
    {
        int curr_offset = iter_row * n_cols;
        double scale = matrix[curr_offset + row];
        double epsilon = scale - 1;
        // fp imprecision
        if (epsilon < 0.0000001 && epsilon > -0.0000001)
        {
            continue;
        }

        // Bounded di kiri dan di kanan, di kiri by row and di kanan by stop_col.
        // Bound kiri: Asumsi kiri pivot udah kosong (0 semua)
        // Bound kanan: Di kanan pivot masih kosong (0 semua), gausah di kurang.
        int stop_col = (n_cols / 2) + (row + 1);
        for (int col = row; col < stop_col; col++)
        {
            matrix[curr_offset + col] -=
                pivot_row[col] * scale;
        }
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
            // printf("N Matrix: : %d\n", N);
            // printf("Mallocing matrix of size %lu\n", (2 * N) * N * sizeof(double));
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
    // printf("[%d/%d] N: %d N_rows: %d Start_Row: %d End_Rows: %d\n", world_rank, world_size, N, n_rows, start_row, end_row);
    if (current_matrix == NULL)
    {
        fprintf(stderr, "Malloc error for current_matrix! in process rank %d. Aborting.. \n", world_rank);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    MPI_Scatter(matrix, 2 * N * n_rows, MPI_DOUBLE, current_matrix,
                2 * N * n_rows, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    double *pivot_row = malloc(n_cols * sizeof(double));

    // Loop setiap row
    // Ada 2 kasus, either:
    // : row ini tanggung jawab dia sendiri (sesuai sama start_row, end_row).
    //   normalize rownya sendiri then broadcast sbg pivot, then eliminate.
    // : row ini diluar range dia
    //   Dia cuma nerima broadcast dari thread lain, terus dia eliminate row2nya sendiri.
    for (int row = 0; row < N; row++)
    {
        int curr_rank = row / n_rows;

        if (world_rank == curr_rank)
        {
            int curr_row = row % n_rows;

            int curr_offset = curr_row * n_cols;

            double pivot = current_matrix[curr_offset + row];

            for (int col = row; col < n_cols; col++)
            {
                current_matrix[curr_offset + col] /= pivot;
            }

            // Send the pivot row to the other processes, and eliminate column [row] from our rows..
            MPI_Bcast(current_matrix + n_cols * curr_row, n_cols, MPI_DOUBLE, curr_rank, MPI_COMM_WORLD);

            iterate_and_eliminate(current_matrix, current_matrix + n_cols * curr_row, n_rows, n_cols, row);
        }
        else
        {
            // Receive pivot from responsible process, and eliminate column [row] from our rows...
            MPI_Bcast(pivot_row, n_cols, MPI_DOUBLE, curr_rank, MPI_COMM_WORLD);

            iterate_and_eliminate(current_matrix, pivot_row, n_rows, n_cols, row);
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