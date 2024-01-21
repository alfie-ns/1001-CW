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

#define EPSILON 0.00001 // tolerance of which two floats are considered equal


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
    [x] RESULTS DO NOT MATCH. it must be the compare function because routine1 must be the same

    git still tracks my .vs folder even though i have .vs/ in my .gitignore file
*/

__declspec(align(64)) float  y[M], z[M]; // declare arrays as 64-byte aligned
__declspec(align(64)) float A[N][N], x[N], w[N]; // declare arrays as 64-byte aligned

__declspec(align(64)) float y_copy[M]; // declare array that holds cloned y array for testing
__declspec(align(64)) float w_copy[N]; // declare array that holds cloned w array for testing

unsigned short int equal(float a, float b) {
    return fabs(a - b) < EPSILON; // fabs returns the absolute value of a-b and compares to see if less than EPSILON
}


unsigned short int compare_arrays(float* arr1, float* arr2, unsigned int size) {
    for (unsigned int i = 0; i < size; i++) {
        if (!equal(arr1[i], arr2[i])) {
            printf("Mismatch at index %u: %f != %f\n", i, arr1[i], arr2[i]); // debugging
            return 0; // Mismatch found
        }
    }
    return 1; // No mismatches
}

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
    memcpy(y_copy, y, M * sizeof(float)); // copy routine1 to test comparison with vectorised version

    run_time = omp_get_wtime() - start_time; //end timer
    printf("\n Time elapsed is %f secs \n %e FLOPs achieved\n", run_time, (double)(ARITHMETIC_OPERATIONS1) / ((double)run_time / TIMES1)); // print testing

    initialize(); // reinitialise the arrays 

    printf("\nRoutine2:");
    start_time = omp_get_wtime(); //start timer

    for (t = 0; t < TIMES2; t++)
        routine2(alpha, beta);

    memcpy(w_copy, w, M * sizeof(float));
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

    if (compare_arrays(y, y_copy, M)) {
        printf("Routine1_vec: Results match.\n");
    }
    else {
        printf("Routine1_vec: Results do not match!\n");
    }

    initialize(); // reinitialise the arrays

    printf("\nRoutine2_vec:");
    start_time = omp_get_wtime(); //start timer

    for (t = 0; t < TIMES2; t++)
        routine2_vec(alpha, beta);

    
    run_time = omp_get_wtime() - start_time; //end timer
    printf("\n Time elapsed is %f secs \n %e FLOPs achieved\n", run_time, (double)(ARITHMETIC_OPERATIONS2) / ((double)run_time / TIMES2)); // print testing
    
    // printf("\n-----------------TESTING------------------------------\n\n");
    if (compare_arrays(w, w_copy, N)) {
        printf("Routine2_vec: Results match.\n");
    }
    else {
        printf("Routine2_vec: Results do not match!\n");
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

    // routine1: y[i] = (alpha * y[i]) + (beta * z[i]);

    unsigned int i; // loop counter

    // Create AVX vectors for alpha and beta
    __m256 alpha_vec = _mm256_set1_ps(alpha); // set1_ps sets all elements of alpha to alpha_vec, which holds 8 elements
    __m256 beta_vec = _mm256_set1_ps(beta); // set1_ps sets all elements of beta to beta_vec, which holds 8 elements
    /* 
       
       essentially there's 8 seperate float alpha = 0.023f, beta = 0.045f iterations in one iteration of the loop
    
       alpha_vec = |0.023|0.023|0.023|0.023|0.023|0.023|0.023|0.023|
       beta_vec = |0.045|0.045|0.045|0.045|0.045|0.045|0.045|0.045|

    */
    // process 8 elements at a time for each iteration, until i reaches M
    for (i = 0; i < M; i += 8) {

        // & means the address of the variable, so &y[i] means the address of the y[i] variable
        __m256 y_vec = _mm256_load_ps(&y[i]); // load 8 seperate iterated elements from y into AVX register 
        __m256 z_vec = _mm256_load_ps(&z[i]); // load 8 seperate iterated elements from z into AVX register

        // Perform the vectorized operations
        __m256 result_vec = _mm256_add_ps(_mm256_mul_ps(alpha_vec, y_vec), _mm256_mul_ps(beta_vec, z_vec));

        // Store the results back into the y array
        _mm256_store_ps(&y[i], result_vec);
    }

    // IS THIS NECCESSARY? [x]
    // No because M is a multiple of 8, so the loop will always be executed?

    // Handle any remaining elements
    for (; i < M; i++) {
        y[i] = alpha * y[i] + beta * z[i];
    }
}





void routine2(float alpha, float beta) {

    unsigned int i, j;


    for (i = 0; i < N; i++) // 2d array iteration?
        for (j = 0; j < N; j++)
            w[i] = (w[i] - beta) + (alpha * A[i][j] * x[j]);

}


// Routine2:  2d i+j loop -> w[i] = (w[i] - beta) + (alpha * A[i][j] * x[j]);

//void routine2_vec(float alpha, float beta) {
//
//    unsigned int i = 0, j = 0;
//
//    // broardcast alpha and beta to all elements of 256-bit vectors
//    __m256 alpha_vec = _mm256_set1_ps(alpha);
//    __m256 beta_vec = _mm256_set1_ps(beta);
//
//    __m256 a_vec = _mm256_set1_ps(A[i][j]);
//    __m256 x_vec = _mm256_set1_ps(x[i]);
//    __m256 w_vec = _mm256_set1_ps(w[i]);
//
//        for (i = 0; i < N; i++)
//        {
//            // init vector to accumulate results
//            __m256 sum_vec = _mm256_setzero_ps(); // setzero_ps sets all elements of sum_vec to start at 0
//
//            for (j = 0; j < N; j += 8) {
//                // load 8 consecutive values from the matrix row and vector x
//                __m256 a_vec = _mm256_load_ps(&A[i][j]);
//                __m256 x_vec = _mm256_load_ps(&x[j]);
//                __m256 w_vec = _mm256_load_ps(&w[i]);
//                
//                // __m256 w_vec = _mm256_broadcast_ss(&w[i]);
//
//
//                // perform calculation
//                w_vec = _mm256_add_ps(_mm256_sub_ps(w_vec, beta_vec), _mm256_mul_ps(alpha_vec, _mm256_mul_ps(a_vec, x_vec)));
//                // ^^^ w[i] = (w[i] - beta) + (alpha * A[i][j] * x[j]); ^^^
//
//                // store the results back into the w array
//                _mm256_store_ps(&w[i], w_vec);
//
//            }
//        }
//}

void routine2_vec(float alpha, float beta) {

    unsigned int i = 0, j = 0; // init loop counters

    // Broadcast variables to all elements of 256-bit vectors
    __m256 alpha_vec = _mm256_set1_ps(alpha);
    __m256 beta_vec = _mm256_set1_ps(beta);

    __m256 a_vec = _mm256_set1_ps(A[i][j]);
    __m256 x_vec = _mm256_set1_ps(x[j]);
    __m256 w_vec = _mm256_set1_ps(w[j]);

    __m256 aSide_vec;
    __m256 bSide_vec;

    for (i = 0; i < N; i++) { 
        __m256 sum_vec = _mm256_setzero_ps(); // Initialise the accumulator vector as zero

        for (j = 0; j < N; j += 8) {

            alpha_vec = _mm256_load_ps(&alpha);
            beta_vec = _mm256_load_ps(&beta);

            __m256 a_vec = _mm256_load_ps(&A[i][j]); // Load elements from A
            __m256 x_vec = _mm256_load_ps(&x[j]);    // Load elements from x
            __m256 w_vec = _mm256_load_ps(&w[i]);    // Load elements from w


            // Fused multiply-add: (alpha_vec * a_vec) * x_vec + sum_vec
            bSide_vec = _mm256_sub_ps(w_vec, beta_vec); // Compute (w[i] - beta)
            aSide_vec = _mm256_fmadd_ps(alpha_vec, a_vec, _mm256_mul_ps(x_vec, sum_vec)); // Compute (alpha*A[i][ij]*x[j]) + sum_vec

            // Horizontal add to sum up elements of sum_vec
            sum_vec = _mm256_hadd_ps(aSide_vec, bSide_vec);

            // Store the result back into the w array
            _mm256_store_ps(&w[j], sum_vec);

            // (w[i] - beta) + ((alpha * A[i][j]) * x[j]);

            /*
                Order of Operations:

                1. alpha*A[i][ij]*x[j]
                2. (w[i] - beta)
                3. (w[i] - beta) + (alpha*A[i][ij]*x[j])

            */
        }
    }
}

//void routine2_vec(float alpha, float beta) {
//    for (unsigned int i = 0; i < N; i++) {
//        __m256 alpha_vec = _mm256_set1_ps(alpha);
//        __m256 sum_vec = _mm256_setzero_ps();
//
//        for (unsigned int j = 0; j < N; j += 8) {
//            __m256 a_vec = _mm256_load_ps(&A[i][j]); // Load 8 elements from A[i]
//            __m256 x_vec = _mm256_load_ps(&x[j]);    // Load 8 elements from x
//
//            // Fused multiply-add: (alpha_vec * a_vec) * x_vec + sum_vec
//            sum_vec = _mm256_fmadd_ps(alpha_vec, a_vec, _mm256_mul_ps(x_vec, sum_vec));
//        }
//
//        // Horizontal add to sum up elements of sum_vec
//        sum_vec = _mm256_hadd_ps(sum_vec, sum_vec);
//        sum_vec = _mm256_hadd_ps(sum_vec, sum_vec);
//        float temp[8];
//        _mm256_store_ps(temp, sum_vec);
//        float asum_scalar = temp[0] + temp[4]; // Sum the horizontal adds
//
//        // Combine the alpha sum and beta part
//        __m256 w_vec = _mm256_set1_ps(w[i]);
//        __m256 beta_vec = _mm256_set1_ps(beta);
//        __m256 bsum_vec = _mm256_sub_ps(w_vec, beta_vec); // Compute (w[i] - beta)
//        __m256 result_vec = _mm256_add_ps(_mm256_set1_ps(asum_scalar), bsum_vec);
//
//        // Extract the scalar value from result_vec and assign it to w[i]
//        float result_scalar[8];
//        _mm256_store_ps(result_scalar, result_vec);
//        w[i] = result_scalar[0]; // Assuming the first element holds the desired result
//    }
//}








/*
     ** AVX intrinsic Functions: **

       __m256: means a 256-bit variable that can hold eight 32-bit single-precision floating-point values.
       _mm256_load_ps: loads eight 32-bit single-precision floating-point values.
       _mm256_store_ps: stores eight 32-bit single-precision floating-point values into memory.
       _mm256_add_ps: adds eight 32-bit single-precision floating-point values.
       _mm256_sub_ps: subtracts eight 32-bit single-precision floating-point values.
       _mm256_mul_ps: multiplies eight 32-bit single-precision floating-point values
       _mm256_setzero_ps: initializes all elements of this 256-bit vector to zero.
       _mm256_set1_ps: initializes all elements of this 256-bit vector with the same single-precision floating-point value.
       _mm256_fmad_ps: performs a fused multiply-add operation on eight 32-bit single-precision floating-point values.
       _mm256_hadd_ps: horizontally adds adjacent pairs of 32-bit single-precision floating-point values.

   /*



/* 
    Routine2:  2d i + j loop->w[i] = (w[i] - beta) + (alpha * A[i][j] * x[j]);

    // decleration of arrays and align as 64-bit
    __declspec(align(64)) float  y[M], z[M]; 
    __declspec(align(64)) float A[N][N], x[N], w[N];



*/






//void routine2_vec(float alpha, float beta) {
//    unsigned int i, j;
//
//    // Broadcast alpha to all elements of 256-bit vectors
//    __m256 alpha_vec = _mm256_set1_ps(alpha);
//
//    for (i = 0; i < N; i++) {
//        // Initialize a vector to accumulate the results
//        __m256 sum_vec = _mm256_setzero_ps();
//
//        for (j = 0; j < N; j += 8) {
//            // Load 8 consecutive values from the matrix row and vector x
//            __m256 a_vec = _mm256_load_ps(&A[i][j]);
//            __m256 x_vec = _mm256_load_ps(&x[j]);
//
//            // Perform the multiplication and accumulate the results
//            sum_vec = _mm256_add_ps(sum_vec, _mm256_mul_ps(alpha_vec, _mm256_mul_ps(a_vec, x_vec)));
//        }
//
//        // Reduce the vector sum_vec to a single scalar sum
//        sum_vec = _mm256_hadd_ps(sum_vec, sum_vec); // Horizontally add pairs of elements
//        sum_vec = _mm256_hadd_ps(sum_vec, sum_vec); // Repeat to sum all elements
//
//        // Move the high 128 bits of sum_vec to low 128 bits and add them to the low 128 bits
//        __m256 permuted = _mm256_permute2f128_ps(sum_vec, sum_vec, 0x1);
//        sum_vec = _mm256_add_ps(sum_vec, permuted);
//
//        // Now the total sum is in the first element of sum_vec
//        __m128 low128 = _mm256_castps256_ps128(sum_vec);
//        __m128 high64 = _mm_movehl_ps(low128, low128); // Move high 64 bits to low 64 bits and add
//        low128 = _mm_add_ps(low128, high64);
//        high64 = _mm_movehdup_ps(low128); // Duplicate high 32 bits in low 64 bits and add
//        low128 = _mm_add_ss(low128, high64);
//
//        // The horizontal add above has summed all the elements, now subtract beta and store the result
//        float final_result = _mm_cvtss_f32(low128) - beta;
//        w[i] = final_result;
//    }
//}



































