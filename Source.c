//mpiexec -np N
#define N 250
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <mpi.h>
#include <time.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    int i, j, rank, size = 0;
    int A[N][N], B[N][N], C[N][N], C_rows[N], A_rows[N];
    double time_start, cur_time, time_total;

    //инициализируем MPI
    MPI_Init(&argc, &argv);
    time_start = MPI_Wtime();
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    srand((unsigned)time(NULL));

    //строим матрицы
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            A[i][j] = rand() % 10;
            B[i][j] = rand() % 10;
        }
    }

    //раскидываем матрицы по процессам
    MPI_Scatter(A, N * N / size, MPI_INT, A_rows, N * N / size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(B, N * N, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    //перемножаем
    for (i = 0; i < N; i++)
    {
        C_rows[i] = 0;
        for (j = 0; j < N; j++)
            C_rows[i] = C_rows[i] + A_rows[j] * B[j][i];
    }

    //отправляем результат в матрицу C
    MPI_Gather(C_rows, N * N / size, MPI_INT, C, N * N / size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    //считаем время
    cur_time = MPI_Wtime() - time_start;
    MPI_Reduce(&cur_time, &time_total, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    //выводим время
    if (rank == 0) {
        printf("\ntime: %.6f sec\n", time_total);
    }
    MPI_Finalize();
    return 0;
}