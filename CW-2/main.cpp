/*
------------------DR VASILIOS KELEFOURAS-----------------------------------------------------
------------------COMP1001 ------------------------------------------------------------------
------------------COMPUTER SYSTEMS MODULE-------------------------------------------------
------------------UNIVERSITY OF PLYMOUTH, SCHOOL OF ENGINEERING, COMPUTING AND MATHEMATICS---
*/


#include <stdio.h> // for input/output functions
#include <time.h> // time related predefined library
#include <pmmintrin.h> // SSE and AVX intrinsics
#include <process.h> // for system() function
#include <chrono> //for high_resolution_clock
#include <iostream> // for input/output functions
#include <immintrin.h> // AVX intrinsics
#include <omp.h> //openmp library



#define M 1024*512 // size of arrays for routine1
#define ARITHMETIC_OPERATIONS1 3*M // number of arithmetic operations in routine1
#define TIMES1 1 // number of times routine1 is executed

#define N 8192 // size of arrays for routine2
#define ARITHMETIC_OPERATIONS2 4*N*N // number of arithmetic operations in routine2
#define TIMES2 1 // number of times routine2 is executed


//function declaration
void initialize(); // init function intializes the arrays
void routine1(float alpha, float beta); // calls routine1 with alpha and beta
void routine2(float alpha, float beta); // calls routine2 with alpha and beta

void routine1_vec(float alpha, float beta); // calls routine1_vec with alpha and beta
void routine2_vec(float alpha, float beta); // calls routine2_vec with alpha and beta
/*
    Routine1: y[i] = alpha * y[i] + beta * z[i];
    Routine2: w[i] = w[i] - beta + alpha * A[i][j] * x[j];

    declspec(align(64)) is used to align the arrays to 64-byte boundaries

    M is the size of the arrays for routine1
    N is the size of the arrays for routine2
*/

__declspec(align(64)) float  y[M], z[M]; // declare arrays as 64-byte aligned
__declspec(align(64)) float A[N][N], x[N], w[N]; // declare arrays as 64-byte aligned

int main() {

    float alpha = 0.023f; // float value of 0.023 for beta alpha variable
    float beta = 0.045f; // float value of 0.045 for float beta variable

    double run_time; // double-type variable to store the execution time, for testing 
    double start_time; // double-type variable to store the start time, for testing

    unsigned int t; // unsigned int-type variable to store the number of times the routines are executed    

    initialize(); // initialize the arrays

    printf("\nRoutine1:");
    start_time = omp_get_wtime(); //start timer

    for (t = 0; t < TIMES1; t++) // for loop to execute routine1 TIMES1 times
        routine1(alpha, beta); // init with alpha and beta

    run_time = omp_get_wtime() - start_time; //end timer
    printf("\n Time elapsed is %f secs \n %e FLOPs achieved\n", run_time, (double)(ARITHMETIC_OPERATIONS1) / ((double)run_time / TIMES1)); // print testing

    printf("\nRoutine2:");
    start_time = omp_get_wtime(); //start timer

    for (t = 0; t < TIMES2; t++)
        routine2(alpha, beta);

    run_time = omp_get_wtime() - start_time; //end timer
    printf("\n Time elapsed is %f secs \n %e FLOPs achieved\n", run_time, (double)(ARITHMETIC_OPERATIONS2) / ((double)run_time / TIMES2)); // print testing

    printf("\n-----------------------------------------------\n");

    printf("\nRoutine1_vec:");
    start_time = omp_get_wtime(); //start timer

    for (t = 0; t < TIMES1; t++) // for loop to execute routine1 TIMES1 times
		routine1_vec(alpha, beta); // init with alpha and beta

    run_time = omp_get_wtime() - start_time; //end timer
    printf("\n Time elapsed is %f secs \n %e FLOPs achieved\n", run_time, (double)(ARITHMETIC_OPERATIONS1) / ((double)run_time / TIMES1)); // print testing

    return 0; // return 0 to indicate that program has finished successfully
}

void initialize() {

    unsigned int i, j;

    //initialize routine2 arrays with random values
    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++) {
            A[i][j] = (i % 99) + (j % 14) + 0.013f;
        }

    //initialize routine1 arrays with random values
    for (i = 0; i < N; i++) {
        x[i] = (i % 19) - 0.01f;
        w[i] = (i % 5) - 0.002f;
    }

    //initialize routine1 arrays
    for (i = 0; i < M; i++) {
        z[i] = (i % 9) - 0.08f;
        y[i] = (i % 19) + 0.07f;
    }


}




void routine1(float alpha, float beta) {

    unsigned int i;


    for (i = 0; i < M; i++)
        y[i] = alpha * y[i] + beta * z[i];

}

void routine1_vec(float alpha, float beta) {

    unsigned int i; // loop counter

    // Create AVX vectors for alpha and beta
    __m256 alpha_vec = _mm256_set1_ps(alpha); // set1_ps sets all 8 elements of the alpha_vec to the same value
    __m256 beta_vec = _mm256_set1_ps(beta); // set1_ps sets all 8 elements of the beta_vec to the same value

    // Process 8 elements at a time
    for (i = 0; i < M; i += 8) {
        // Load 8 elements from y and z into AVX registers
        __m256 y_vec = _mm256_load_ps(&y[i]);
        __m256 z_vec = _mm256_load_ps(&z[i]);

        // Perform the vectorized operations
        __m256 result_vec = _mm256_add_ps(_mm256_mul_ps(alpha_vec, y_vec),
            _mm256_mul_ps(beta_vec, z_vec));

        // Store the results back into the y array
        _mm256_store_ps(&y[i], result_vec);
    }

    // Handle any remaining elements
    for (; i < M; i++) {
        y[i] = alpha * y[i] + beta * z[i];
    }
}




void routine2(float alpha, float beta) {

    unsigned int i, j;


    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
            w[i] = w[i] - beta + alpha * A[i][j] * x[j];


}

















































































/*
void routine1_vec(float alpha, float beta) {

    __m256 alpha_vec = _mm256_set1_ps(alpha); // set1_ps sets all 8 elements of the alpha_vec to the same value, using 256-bit registers
    __m256 beta_vec = _mm256_set1_ps(beta); // set1_ps sets all 8 elements of the beta_vec to the same value, using 256-bit registers

    unsigned int i; // loop counter

    for (i = 0; i < M; i += 8) { // for each iteration of the loop process 8 elements at a time
        __m256 y_vec = _mm256_load_ps(&y[i]); // load 8 elements from y into AVX register
        __m256 z_vec = _mm256_load_ps(&z[i]); // load 8 elements from z into AVX register

        __m256 result_vec = _mm256_add_ps(_mm256_mul_ps(alpha_vec, y_vec), _mm256_mul_ps(beta_vec, z_vec)); // perform the vectorized operations, when alpha and beta get added, after the multiplication, they get broadcasted to all 8 elements

        _mm256_store_ps(&y[i], result_vec); // store the results back into the y array
    }


}

void routine2_vec(float alpha, float beta) {
    unsigned int i;
    unsigned int j;

    // Create AVX vectors for alpha and beta
    __m256 alpha_vec = _mm256_set1_ps(alpha);
    __m256 beta_vec = _mm256_set1_ps(beta);

    // Process 8 elements at a time for the i-loop
    for (i = 0; i < N; i += 8) {
        // Load 8 elements from w into AVX register
        __m256 w_vec = _mm256_load_ps(&w[i]);

        // Initialize a vector to accumulate the results
        __m256 sum_vec = _mm256_setzero_ps();

        for (j = 0; j < N; j++) {
            // Load 8 elements from A[i][j] and x[j] into AVX registers
            __m256 a_vec = _mm256_load_ps(&A[i][j]);
            __m256 x_vec = _mm256_set1_ps(x[j]);

            // Perform the vectorized operation for the matrix-vector multiplication
            sum_vec = _mm256_add_ps(sum_vec, _mm256_mul_ps(alpha_vec, _mm256_mul_ps(a_vec, x_vec)));
        }

        // Subtract beta and add to the initial value of w[i]
        w_vec = _mm256_sub_ps(w_vec, beta_vec);
        w_vec = _mm256_add_ps(w_vec, sum_vec);

        // Store the results back into the w array
        _mm256_store_ps(&w[i], w_vec);
    }

    // Handle any remaining elements
    for (; i < N; i++) {
        for (j = 0; j < N; j++) {
            w[i] = w[i] - beta + alpha * A[i][j] * x[j];
        }
    }
}

/*
	Routine1: y[i] = alpha * y[i] + beta * z[i];
    Routine2: w[i] = w[i] - beta + alpha * A[i][j] * x[j];

    AVX 
    {
        Example:
        {

            unsigned short int ConstAdd_AVX() { // AVX implementation of the function

                __m256  ymm1, ymm2, ymm3; // declare 3 variables of type __m256, which are 256-bit registers
                int i; // loop counter

                ymm1 = _mm256_set_ps(2.1234f, 2.1234f, 2.1234f, 2.1234f, 2.1234f, 2.1234f, 2.1234f, 2.1234f); //set num1 values
                for (i = 0; i < M; i += 8) { // for each element in the array, incrementing by 8 because 8 floats fit in a 256-bit register
                    ymm2 = _mm256_loadu_ps(&V2[i]); // load 8 elements of X2[]
                    ymm3 = _mm256_add_ps(ymm1, ymm2); // num3 = num1 + num2
                    _mm256_storeu_ps(&V1[i], ymm3); // store num3 to Y[i]. num3 has 8 FP values which they are stored into Y[i], Y[i+1], Y[i+2], Y[i+3], .. Y[i+7]
                }


                return 2;
            }
        }

        M:
		{
            unsigned short int Routine1_AVX() {

                __m256 alpha_vec = _mm256_set1_ps(alpha); // set1_ps sets all 8 elements of the alpha_vec to the same value
                __m256 beta_vec = _mm256_set1_ps(beta); // set1_ps sets all 8 elements of the beta_vec to the same value

                unsigned int i; // loop counter

                for (i = 0; i < M; i += 8) {
					__m256 y_vec = _mm256_load_ps(&y[i]); // load 8 elements from y into AVX register
					__m256 z_vec = _mm256_load_ps(&z[i]); // load 8 elements from z into AVX register

					__m256 result_vec = _mm256_add_ps(_mm256_mul_ps(alpha_vec, y_vec), _mm256_mul_ps(beta_vec, z_vec)); // perform the vectorized operations

					_mm256_store_ps(&y[i], result_vec); // store the results back into the y array
				}

                return 2;
			}
		}
	}
		}
    }

    SSE:
    {
        Example:
        {

            unsigned short int ConstAdd_SSE() { // SSE implementation of the function

                __m128 num1, num2, num3; // declare 3 variables of type __m128, which are 128-bit registers
                int i; // loop counter

                num1 = _mm_set_ps(2.1234f, 2.1234f, 2.1234f, 2.1234f); //set num1 values

                for (i = 0; i < M; i += 4) { // for each element in the array, incrementing by 4 because 4 floats fit in a 128-bit register
                    num2 = _mm_loadu_ps(&V2[i]); //load 4 elements of V2[]
                    num3 = _mm_add_ps(num1, num2); //num3 = num1 + num2
                    _mm_storeu_ps(&V1[i], num3); //store num3 to Y[i]. num3 has 4 FP values which they are stored into Y[i], Y[i+1], Y[i+2], Y[i+3], respectively
                }


                return 2;
            }
        }
        
        M:
        {

        }
    }

    Compare:
    {

        compare checker:
        {
            unsigned short int Compare_ConstAdd() {
            int j;

            for (j = 0; j < M; j++) {
                test3[j] = V2[j] + 2.1234f;
            }


            for (j = 0; j < M; j++)
                if (equal(V1[j], test3[j]) == 1) {
                    //printf("\n j=%d\n", j);
                    return 1;
                }

            return 0;
        }

        unsigned short int equal(float a, float b) {
            float temp = a - b;
            //printf("\n %f  %f", a, b);
            if ((fabs(temp) / fabs(b)) < EPSILON)
                return 0; //success
            else
                return 1; //wrong result
        }
      }
    }




    Routine1: y[i] = alpha * y[i] + beta * z[i];
    Routine2: w[i] = w[i] - beta + alpha * A[i][j] * x[j];
*/






















































































