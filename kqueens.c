#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int n, k, my_rank, num_procs, count;
int* board;

int is_safe(int row, int col) {
    int i, j;
    for (i = 0; i < row; i++) {
        if (board[i] == col) return 0;
        if (abs(board[i] - col) == abs(i - row)) return 0;
    }
    return 1;
}

void solve(int row) {
    int i, j;
    if (row == n) {
        count++;
    } else {
        for (i = my_rank; i < n; i += num_procs) {
            for (j = 0; j < n; j++) {
                if (is_safe(row, j)) {
                    board[row] = j;
                    solve(row + 1);
                }
            }
        }
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    if (argc != 2) {
        if (my_rank == 0) {
            printf("Usage: %s <n>\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    n = atoi(argv[1]);
    k = n / num_procs;
    board = (int*) malloc(n * sizeof(int));

    MPI_Barrier(MPI_COMM_WORLD);
    double start_time = MPI_Wtime();

    solve(0);

    MPI_Reduce(&count, &count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    double end_time = MPI_Wtime();

    if (my_rank == 0) {
        printf("%d-Queens solution count: %d\n", n, count);
        printf("Execution time: %f seconds\n", end_time - start_time);
    }

    MPI_Finalize();
    return 0;
}
