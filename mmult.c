//********************************************
// Matrix multiply test harness
//
// Author: Phil Howard

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

static int g_a_rows = 3;
static int g_a_cols = 2;
static int g_b_cols = 3;
static int g_n_threads = 0;
static int g_print_level = 0;
static int g_alg = 0;

static void proc_args(int argc, char **argv)
{
    int opt;

    while ((opt = getopt(argc, argv, "ha:r:c:b:t:p:")) != -1)
    {
        switch (opt)
        {
            case 'h':
                printf("%s -a <alg> -r <rows> -c <cols> -b <b cols> "
                       "-t <threads> -p <print level>\n", argv[0]);
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

int main(int argc, char **argv)
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

    a = (a_type*)malloc(a_rows * sizeof(a_type));
    b = (b_type*)malloc(b_rows * sizeof(b_type));
    c = (c_type*)malloc(c_rows * sizeof(c_type));

    double value = 0;
    for (int ii=0; ii<a_rows; ii++)
    {
        for (int jj=0; jj<a_cols; jj++)
        {
            a[ii][jj] = value++;
        }
    }

    for (int ii=0; ii<b_rows; ii++)
    {
        for (int jj=0; jj<b_cols; jj++)
        {
            b[ii][jj] = value++;
        }
    }

    if (g_print_level > 1) 
        Matrix_Print(stdout, &a[0][0], a_rows, a_cols, "%7.2f ");
    if (g_print_level > 1) printf("\n");

    if (g_print_level > 1) 
        Matrix_Print(stdout, &b[0][0], b_rows, b_cols, "%7.2f ");
    if (g_print_level > 1) printf("\n");

    switch (g_alg)
    {
        case 0:
            printf("Matrix Multiply\n");
            Matrix_Multiply(&a[0][0],&b[0][0],&c[0][0], 
                            a_cols, a_rows, b_cols);
            break;
        case 1:
            printf("Array Indexing\n");
            Matrix_Multiply_Array(&a[0][0],&b[0][0],&c[0][0], 
                            a_cols, a_rows, b_cols);
            break;
        case 2:
            printf("Multiply with Transpose\n");
            Matrix_Multiply_Transpose(&a[0][0],&b[0][0],&c[0][0], 
                            a_cols, a_rows, b_cols);
            break;
        case 3:
            printf("Transpose\n");
            Matrix_Transpose(&a[0][0],&c[0][0], 
                            a_cols, a_rows);
            break;
        case 4:
            printf("Threads\n");
            Matrix_Multiply_Threads(&a[0][0],&b[0][0],&c[0][0], 
                            a_cols, a_rows, b_cols, g_n_threads);
            break;
        default:
            printf("Unknown algirithm\n");
            break;
    }


    if (g_print_level > 0) Matrix_Print(stdout, &c[0][0], c_rows, c_cols, "%7.2f ");

    return 0;
}


