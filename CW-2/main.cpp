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

#define EPSILON 0.00001


//function declarations
void initialize(); // init function intializes the arrays

void routine1(float alpha, float beta); // calls routine1 with alpha and beta
void routine2(float alpha, float beta); // calls routine2 with alpha and beta

void routine1_vec(float alpha, float beta); // calls routine1_vec with alpha and beta
void routine2_vec(float alpha, float beta); // calls routine2_vec with alpha and beta

unsigned short int equal(float a, float b); // function to check equality within tolerance
unsigned short int compare_arrays(float* arr1, float* arr2, unsigned int size); // function to compare two arrays

/*
    Routine1: y[i] = (alpha * y[i]) + (beta * z[i]);
    Routine2: w[i] = w[i] = (w[i] - beta) + (alpha * A[i][j] * x[j]);

    declspec(align(64)) is used to align the arrays to 64-byte boundaries

    M is the size of the arrays for routine1
    N is the size of the arrays for routine2

    [x] TODO: Make routine2_vec
    [x] TODO: TEST IF VECTORISED CALCULATIONS = NON-OPTIMISED VERSION WITH COMPARE FUNCTION
    [ ] RESULTS DO NOT MATCH. it must be the compare function because routine1 must be the same

    git still tracks my .vs folder even though i have .vs/ in my .gitignore file
*/

__declspec(align(64)) float  y[M], z[M]; // declare arrays as 64-byte aligned
__declspec(align(64)) float A[N][N], x[N], w[N]; // declare arrays as 64-byte aligned

__declspec(align(64)) float y_copy[M]; // declare array that holds cloned y array for testing


int main() {

    float alpha = 0.023f; // float value of 0.023 for beta alpha variable
    float beta = 0.045f; // float value of 0.045 for float beta variable

    double run_time; // double-type variable to store the execution time, for testing 
    double start_time; // double-type variable to store the start time, for testing

    unsigned int t; // unsigned int-type variable to store the number of times the routines are executed    

    initialize(); // initialise the arrays

    printf("\n-----------------NON-OPTIMISED------------------------------\n");

    printf("\nRoutine1:");
    start_time = omp_get_wtime(); //start timer

    for (t = 0; t < TIMES1; t++) // for loop to execute routine1 TIMES1 times
        routine1(alpha, beta); // init with alpha and beta
    // declare test-copy arrays as 64-byte aligned
    memcpy(y_copy, y, M * sizeof(float)); // copy y to test if y_copy == y

    run_time = omp_get_wtime() - start_time; //end timer
    printf("\n Time elapsed is %f secs \n %e FLOPs achieved\n", run_time, (double)(ARITHMETIC_OPERATIONS1) / ((double)run_time / TIMES1)); // print testing

    initialize(); // reinitialise the arrays 

    printf("\nRoutine2:");
    start_time = omp_get_wtime(); //start timer

    for (t = 0; t < TIMES2; t++)
        routine2(alpha, beta);

    run_time = omp_get_wtime() - start_time; //end timer
    printf("\n Time elapsed is %f secs \n %e FLOPs achieved\n", run_time, (double)(ARITHMETIC_OPERATIONS2) / ((double)run_time / TIMES2)); // print testing

    initialize(); // reinitialise arrays 

    printf("\n-----------------VECTORISED------------------------------\n");

    // [x] make copies before running vectorised versions
    

    printf("\nRoutine1_vec:");
    start_time = omp_get_wtime(); //start timer

    for (t = 0; t < TIMES1; t++) // for loop to execute routine1 TIMES1 times
        routine1_vec(alpha, beta); // init with alpha and beta

    run_time = omp_get_wtime() - start_time; //end timer
    printf("\n Time elapsed is %f secs \n %e FLOPs achieved\n", run_time, (double)(ARITHMETIC_OPERATIONS1) / ((double)run_time / TIMES1)); // print testing

    //initialize(); // reinitialise the arrays

    //printf("\nRoutine2_vec:");
    //start_time = omp_get_wtime(); //start timer

    //for (t = 0; t < TIMES2; t++)
    //    routine2_vec(alpha, beta);

    //run_time = omp_get_wtime() - start_time; //end timer
    //printf("\n Time elapsed is %f secs \n %e FLOPs achieved\n", run_time, (double)(ARITHMETIC_OPERATIONS2) / ((double)run_time / TIMES2)); // print testing

    printf("\n-----------------TESTING------------------------------\n\n");
    
    
    if (compare_arrays(y, y_copy, M)) {
        printf("Routine1_vec: Results match.\n");
    }
    else {
        printf("Routine1_vec: Results do not match!\n");
    }



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

void routine1(float alpha, float beta) { // routine1: y[i] = alpha * y[i] + beta * z[i];

    unsigned int i; // loop counter

    for (i = 0; i < M; i++)
        y[i] = (alpha * y[i]) + (beta * z[i]); // for each iteration of the array, i = (alpha * y[i]) + (beta * z[i])
}

// AVX implementation of routine1

void routine1_vec(float alpha, float beta) {

    unsigned int i; // loop counter

    // Create AVX vectors for alpha and beta
    __m256 alpha_vec = _mm256_set1_ps(alpha); // set1_ps sets all elements of alpha to alpha_vec, which holds 8 elements
    __m256 beta_vec = _mm256_set1_ps(beta); // set1_ps sets all elements of beta to beta_vec, which holds 8 elements
    // essentially there's 8 seperate float beta = 0.045f; iterations
    // beta_vec = |0.045|0.045|0.045|0.045|0.045|0.045|0.045|0.045|

    // process 8 elements at a time for each iteration, until i reaches M
    for (i = 0; i < M; i += 8) {

        // & means the address of the variable, so &y[i] means the address of the y[i] variable
        __m256 y_vec = _mm256_load_ps(&y[i]); // load 8 seperate iterated elements from y into AVX register 
        __m256 z_vec = _mm256_load_ps(&z[i]); // load 8 seperate iterated elements from z into AVX register

        // Perform the vectorized operations
        __m256 result_vec = _mm256_add_ps(_mm256_mul_ps(alpha_vec, y_vec),
            _mm256_mul_ps(beta_vec, z_vec));
        // result_vec = (alpha * y_vec) + (beta * z_vec)

    // Store the results back into the y array
        _mm256_store_ps(&y[i], result_vec);
    }

    // IS THIS NECCESSARY? [x]
    // No because M is a multiple of 8, so the loop will always be executed?

    //// Handle any remaining elements
    //for (; i < M; i++) {
    //    y[i] = alpha * y[i] + beta * z[i];
    //}
}

// Function to check equality within tolerance
//unsigned short int equal(float a, float b) {
//    float temp = a - b;
//    return (fabs(temp) / fabs(b + EPSILON)) < EPSILON;
//}
unsigned short int equal(float a, float b) {
    return fabs(a - b) < EPSILON;
}


unsigned short int compare_arrays(float* arr1, float* arr2, unsigned int size) {
    for (unsigned int i = 0; i < size; i++) {
        if (!equal(arr1[i], arr2[i])) {
            printf("Mismatch at index %u: %f != %f\n", i, arr1[i], arr2[i]); // For debugging
            return 0; // Mismatch found
        }
    }
    return 1; // No mismatches
}





void routine2(float alpha, float beta) {

    unsigned int i, j;


    for (i = 0; i < N; i++) // 2d array iteration?
        for (j = 0; j < N; j++)
            w[i] = (w[i] - beta) + (alpha * A[i][j] * x[j]);

}
























































