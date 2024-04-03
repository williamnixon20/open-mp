/**
 * On mac
 * run brew install llvm libomp
 *
 * Compiling: use clang from llvm
 * opt/homebrew/opt/llvm/bin/clang -fopenmp mp.c
 * Running time ./a.out < ../../test_cases/512.txt
https://docs.google.com/document/d/1BS7HkoFVYfCC6CQI_lWaLgAxgTip7U44yUf2FIAV0JY/edit
*/

#include <omp.h>
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
        printf("Result matrix logged to file named result_inverse_matrix_%d.txt.\n", N);
    }
    else
    {
        printf("Failed to open file for logging.\n");
    }
}

int main(void)
{
    int N;
    double *matrix = NULL;

    if (scanf("%d\n", &N) == 1)
    {
        matrix = malloc((2 * N) * N * sizeof(double));
        if (matrix == NULL)
        {
            fprintf(stderr, "Malloc error for matrix! Aborting.. \n");
            exit(1);
        }
        read_matrix(matrix, N);
        initialize_identity(matrix, N);
    }

    for (int i = 0; i < N; i++)
    {
        int curr_pivot_offset = i * 2 * N;
        double pivot_value = matrix[curr_pivot_offset + i];
        if ((pivot_value) < 0.00001 && (pivot_value) > -0.00001)
        {
            printf("Pivot value is already normalized %d\n", i);
        }
        else
        {
            #pragma omp parallel for
            for (int j = 0; j < 2 * N; j++)
            {
                matrix[curr_pivot_offset + j] /= pivot_value;
            }
        }

        #pragma omp parallel for
        for (int j = 0; j < N; j++)
        {
            int curr_row_offset = j * 2 * N;
            double factor = matrix[curr_row_offset + i];
            if ((factor - 1) < 0.00001 && (factor - 1) > -0.00001)
            {
                continue;
            }
            // printf("Factor: %f\n", factor);
            for (int k = 0; k < 2 * N; k++)
            {
                matrix[curr_row_offset + k] -= factor * matrix[curr_pivot_offset + k];
            }
        }
    }

    save_file(matrix, N);

    return 0;
}
