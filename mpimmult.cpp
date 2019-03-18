#include <mpi.h>

#define _POSIX_C_SOURCE 2
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "matrix.h"

#ifndef _POSIX_C_SOURCE
#error "No POSIX C source"
#endif

#if _POSIX_C_SOURCE < 2
#error "old POSIX"
#endif

void MMult(double *a, double *b, double *c, int a_cols, int a_rows, int b_cols);

static int g_a_rows = 3;
static int g_a_cols = 2;
static int g_b_cols = 3;
static int g_n_threads = 0;
static int g_print_level = 0;
static int g_alg = 0;
static int g_rank = 0;
// Get the number of processes
static int world_size = 0;

static void proc_args(int argc, char **argv)
{
    int opt;

    while ((opt = getopt(argc, argv, "ha:r:c:b:t:p:")) != -1)
    {
        switch (opt)
        {
        case 'h':
            printf("%s -a <alg> -r <rows> -c <cols> -b <b cols> "
                   "-t <threads> -p <print level>\n",
                   argv[0]);
            exit(1);
            break;
        case 'a':
            g_alg = atoi(optarg);
            break;
        case 'r':
            g_a_rows = atoi(optarg);
            break;
        case 'c':
            g_a_cols = atoi(optarg);
            break;
        case 'b':
            g_b_cols = atoi(optarg);
            break;
        case 't':
            g_n_threads = atoi(optarg);
            break;
        case 'p':
            g_print_level = atoi(optarg);
            break;
        }
    }
}

void Matrix_Print(FILE *output, double *mat, int rows, int cols,
                  const char *format)
{
    if (g_print_level > 1)
    {
        printf("in print\n");
    }

    double *value = mat;

    for (int ii = 0; ii < rows; ii++)
    {
        for (int jj = 0; jj < cols; jj++)
        {
            fprintf(output, format, *value);
            value++;
        }
        fprintf(output, "\n");
    }
}

int main(int argc, char **argv)
{
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &g_rank);
    if (g_rank == 0)
    {
        int a_cols, a_rows;
        int b_cols, b_rows;
        int c_cols, c_rows;

        proc_args(argc, argv);

        a_cols = g_a_cols;
        a_rows = g_a_rows;
        b_cols = g_b_cols;

        b_rows = a_cols;
        c_rows = a_rows;
        c_cols = b_cols;

        if (g_alg == 3)
        {
            c_cols = a_rows;
            c_rows = a_cols;
        }

        typedef double a_type[a_cols];
        typedef double b_type[b_cols];
        typedef double c_type[c_cols];
        a_type *a;
        b_type *b;
        c_type *c;

        a = (a_type *)malloc(a_rows * sizeof(a_type));
        b = (b_type *)malloc(b_rows * sizeof(b_type));
        c = (c_type *)malloc(c_rows * sizeof(c_type));

        double value = 0;
        for (int ii = 0; ii < a_rows; ii++)
        {
            for (int jj = 0; jj < a_cols; jj++)
            {
                a[ii][jj] = value++;
            }
        }

        for (int ii = 0; ii < b_rows; ii++)
        {
            for (int jj = 0; jj < b_cols; jj++)
            {
                b[ii][jj] = value++;
            }
        }

        if (g_print_level > 1 && g_rank == 0)
            Matrix_Print(stdout, &a[0][0], a_rows, a_cols, "%7.2f ");
        if (g_print_level > 1 && g_rank == 0)
            printf("\n");

        if (g_print_level > 1 && g_rank == 0)
            Matrix_Print(stdout, &b[0][0], b_rows, b_cols, "%7.2f ");
        if (g_print_level > 1 && g_rank == 0)
            printf("\n");

        // do the stuff here

        switch (g_alg)
        {
        default:
            if (g_rank == 0)
                printf("Mpi Multiply\n");
            MMult(&a[0][0], &b[0][0], &c[0][0], a_cols, a_rows, b_cols);
            break;
        }
        if (g_print_level > 0)
            if (g_rank == 0)
            {
                Matrix_Print(stdout, &c[0][0], c_rows, c_cols, "%7.2f ");
            }
    }
    return 0;
}

void MMult(double *a, double *b, double *c, int a_cols, int a_rows, int b_cols)
{
    double *aa, *cc;
    aa = (double *)malloc(a_cols * b_cols * sizeof(double));
    cc = (double *)malloc(a_cols * b_cols * sizeof(double));
    // scatter a
    // int MPI_Scatter(void *sendbuf, int sendcount,
    //     MPI_Datatype sendtype,
    //     void *recvbuf, int recvcnt,
    //     MPI_Datatype recvtype,
    //     int root, MPI_Comm comm);
    MPI_Scatter(a, a_cols * b_cols / world_size,
                MPI_DOUBLE,
                aa, sizeof(aa) / sizeof(double),
                MPI_DOUBLE,
                0, MPI_COMM_WORLD);
    if (g_print_level > 1 && g_rank == 0)
        printf("scatter worked\n");
    // broadcast b
    MPI_Bcast(b, (a_rows * a_cols),
              MPI_DOUBLE,
              0, MPI_COMM_WORLD);
    if (g_print_level > 1 && g_rank == 0)
        printf("broadcast worked\n");
    // do the stuff
    double temp_value;
    double *a_value, *b_value, *c_value;

    c_value = cc;

    for (int arow = 0; arow < (sizeof(aa) / sizeof(double)) / world_size; arow++)
    {
        for (int bcol = 0; bcol < b_cols; bcol++)
        {
            a_value = aa + arow * a_cols;
            b_value = b + bcol;
            temp_value = 0;

            for (int acol = 0; acol < a_cols; acol++)
            {
                temp_value += *a_value * (*b_value);
                if (g_print_level > 3 && g_rank == 0)
                    printf("%d %d %d %f %f %f\n", arow, bcol, acol,
                           temp_value, *a_value, *b_value);
                a_value++;
                if (g_print_level > 3 && g_rank == 0)
                    printf("\tjust did a_value\n");
                b_value += b_cols;
                if (g_print_level > 3 && g_rank == 0)
                    printf("just did b_value\n");
            }
            if (g_print_level > 2 && g_rank == 0)
                printf("\tout of inner loop\n");
            *c_value = temp_value;
            c_value++;
            if (g_print_level > 2 && g_rank == 0)
                printf("\tout of middle loop\n");
        }
    }
    if (g_print_level > 1 && g_rank == 0)
        printf("mathing worked\n");
    // gather into c
    MPI_Gather(c, a_cols * b_cols,
               MPI_DOUBLE,
               cc, sizeof(cc) / sizeof(double),
               MPI_DOUBLE,
               0, MPI_COMM_WORLD);
    if (g_print_level > 1 && g_rank == 0)
        printf("gather worked\n");

    free(aa);
    free(cc);
    MPI_Finalize();
}
