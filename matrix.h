#pragma once
//*****************************************************
// Define various matrix multiply routines
//
// Author: Phil Howard
//
#include <stdio.h>

void Matrix_Multiply(double *a, double *b, double *c, 
                    int a_cols, int a_rows, int b_cols);

void Matrix_Multiply_Array(double *a, double *b, double *c, 
                    int a_cols, int a_rows, int b_cols);

void Matrix_Multiply_Transpose(double *a, double *b, double *c, 
                    int a_cols, int a_rows, int b_cols);

void Matrix_Multiply_Threads(double *a, double *b, double *c, 
                    int a_cols, int a_rows, int b_cols, int n_threads);

void Matrix_Transpose(double *matrix, double *transpose, int cols, int rows);

void Matrix_Print(FILE *output, double *mat, int rows, int cols, 
                  const char *format);

