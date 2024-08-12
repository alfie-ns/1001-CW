/*
------------------DR VASILIOS KELEFOURAS-----------------------------------------------------
------------------COMP1001 ------------------------------------------------------------------
------------------COMPUTER SYSTEMS MODULE-------------------------------------------------
------------------UNIVERSITY OF PLYMOUTH, SCHOOL OF ENGINEERING, COMPUTING AND MATHEMATICS---
*/


#include <stdio.h> // for input/output functions
#include <time.h> // time related predefined library
#include <pmmintrin.h> // SSE and AVX intrinsics
#include <process.h> // for Windows intrinsics
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

#define EPSILON 0.0001 // tolerance of which two floats are considered equal


//function declarations
void initialize(); // init function intialises the arrays

void routine1(float alpha, float beta); // calls routine1 with alpha and beta
void routine2(float alpha, float beta); // calls routine2 with alpha and beta

void routine1_vec(float alpha, float beta); // calls routine1_vec with alpha and beta
void routine2_vec(float alpha, float beta); // calls routine2_vec with alpha and beta

__declspec(align(64)) float  y[M], z[M]; // declare arrays as 64-byte aligned
__declspec(align(64)) float A[N][N], x[N], w[N]; // declare arrays as 64-byte aligned

__declspec(align(64)) float y_copy[M]; // declare array that holds cloned y array for testing
__declspec(align(64)) float w_copy[N]; // declare array that holds cloned w array for testing

unsigned short int equal(float a, float b); // function to check equality within tolerance


int main() {

    float alpha = 0.023f; // float value of 0.023 for beta alpha variable
    float beta = 0.045f; // float value of 0.045 for float beta variable

    double run_time; // double variable to store the execution time, for testing
    double start_time; // double variable to store the start time, for testing
   
    unsigned int t; // unsigned int variable to store the number of times the routines are executed

    initialize(); // initialise the arrays

    printf("\n-----------------NON-OPTIMISED------------------------------\n");

    printf("\nRoutine1:");
    start_time = omp_get_wtime(); //start timer

    for (t = 0; t < TIMES1; t++) // for loop to execute routine1 TIMES1 times
        routine1(alpha, beta); // init with alpha and beta
   
    run_time = omp_get_wtime() - start_time; //end timer
    printf("\n Time elapsed is %f secs \n %e FLOPs achieved\n", run_time, (double)(ARITHMETIC_OPERATIONS1) / ((double)run_time / TIMES1)); // print testing
    std::copy(y, y + M, y_copy); // copy routine1 to test comparison with vectorised version, y+M to check for last element

    initialize(); // reinitialise the arrays

    printf("\nRoutine2:");
    start_time = omp_get_wtime(); //start timer

    for (t = 0; t < TIMES2; t++)
        routine2(alpha, beta);
   
   
    run_time = omp_get_wtime() - start_time; //end timer
    printf("\n Time elapsed is %f secs \n %e FLOPs achieved\n", run_time, (double)(ARITHMETIC_OPERATIONS2) / ((double)run_time / TIMES2)); // print testing
    std::copy(w, w + N, w_copy); // copy routine2 to test comparison with vectorised version, w+N to check for last element
   
    initialize(); // reinitialise arrays

    printf("\n-----------------VECTORISED------------------------------\n");

    printf("\nRoutine1_vec:");
    start_time = omp_get_wtime(); //start timer

    for (t = 0; t < TIMES1; t++) // for loop to execute routine1 TIMES1 times
        routine1_vec(alpha, beta); // init with alpha and beta


    run_time = omp_get_wtime() - start_time; //end timer
    printf("\n Time elapsed is %f secs \n %e FLOPs achieved\n", run_time, (double)(ARITHMETIC_OPERATIONS1) / ((double)run_time / TIMES1)); // print testing
   
    initialize(); // reinitialise the arrays

    printf("\nRoutine2_vec:");
    start_time = omp_get_wtime(); //start timer

    for (t = 0; t < TIMES2; t++)
        routine2_vec(alpha, beta);

    // w an array of size M, w_copy is a copy of w, w_copy+M to know when to stop copying

    run_time = omp_get_wtime() - start_time; //end timer
    printf("\n Time elapsed is %f secs \n %e FLOPs achieved\n", run_time, (double)(ARITHMETIC_OPERATIONS2) / ((double)run_time / TIMES2)); //

    printf("\n");

    printf("-----------------TESTING------------------------------\n\n");

    bool arraysAreEqual; // switch to check if arrays are equal

    arraysAreEqual = false;
    for (int i = 0; i < M; i++) {
        if (!equal(y_copy[i], y[i])) { // Use the equal function to compare elements
            printf("Routine1: Results do not match!\n");
            break;
        }
        else {
            arraysAreEqual = true;
        }
    }
    if (arraysAreEqual) {
        printf("Routine1 <-> Routine1_vec. Results match.\n");
    }
    arraysAreEqual = false;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) { // 2d array iteration
            if (equal(w_copy[i], w[i])) { // Use the equal function to compare elements
                arraysAreEqual = true;
                break;
            }
           
        }
    }
    if (arraysAreEqual) {
        printf("Routine2 <-> Routine2_vec: Results match.\n");
    }
    else {
        printf("Routine2_vec: Results do not match!\n");
    }

    return 0; // return 0 to indicate that program has finished successfully
}
/*
    NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW

    Routine1: y[i] = (alpha * y[i]) + (beta * z[i]);
    Routine2: w[i] = w[i] = (w[i] - beta) + (alpha * A[i][j] * x[j]);

    declspec(align(64)) is used to align the arrays to 64-byte boundaries

    M is the size of the arrays for routine1
    N is the size of the arrays for routine2

    [X] Make routine1_vec
    [X] Make routine2_vec
    [X] Make copies of y and w arrays before running vectorised versions, for testing
    [X] RESULTS DO NOT MATCH.
    [X] Make NEW routine2_vec

    https://github.com/alfie-ns/1001-CW

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

 */
void initialize() {

    unsigned int i, j;

    //initialize routine2 arrays with random values
    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++) {
            A[i][j] = (i % 99) + (j % 14) + 0.013f;
        }

    //init routine1 arrays with random values
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

// from lab-session:
unsigned short int equal(float a, float b) {
    float temp = a - b;
    //printf("\n %f  %f", a, b);
    if ((fabs(temp) / fabs(b)) < EPSILON) // if the difference between the two given floats is less than the tolerance/epsilon, return 0(considered equal)
        return 0; //success
    else
        return 1; //wrong result
}




// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ROUTINE1

// ROUTINE1 | NON-OPTIMISED
void routine1(float alpha, float beta) { // routine1: y[i] = alpha * y[i] + beta * z[i];

    unsigned int i; // loop counter

    for (i = 0; i < M; i++)
        y[i] = (alpha * y[i]) + (beta * z[i]); // for each iteration of the array, i = (alpha * y[i]) + (beta * z[i])
}

// OLD ROUTINE1 | VECTORISED
/*
void routine1_vec(float alpha, float beta) {

    unsigned int i; // loop counter, usigned so the computer knows it's not a negative number

    // Create AVX vectors for alpha and beta
    __m256 alpha_vec = _mm256_set1_ps(alpha); // set1_ps sets all elements of alpha to alpha_vec, which holds 8 elements
    __m256 beta_vec = _mm256_set1_ps(beta); // set1_ps sets all elements of beta to beta_vec, which holds 8 elements

    // alpha_vec = |0.023|0.023|0.023|0.023|0.023|0.023|0.023|0.023|
    // beta_vec = |0.045|0.045|0.045|0.045|0.045|0.045|0.045|0.045|

    // process 8 elements at a time for each iteration, until i reaches M
    for (i = 0; i < M; i += 8) {

        // & means the address of the variable, so &y[i] means the address of the y[i] variable
        __m256 y_vec = _mm256_load_ps(&y[i]); // load 8 seperate iterated elements from y into AVX register
        __m256 z_vec = _mm256_load_ps(&z[i]); // load 8 seperate iterated elements from z into AVX register

        // Perform the vectorized operations
        __m256 result_vec = _mm256_add_ps(_mm256_mul_ps(alpha_vec, y_vec),_mm256_mul_ps(beta_vec, z_vec));
        // result_vec = (alpha * y_vec) + (beta * z_vec)
        // This is done for 8 elements at a time^

        // Store the results back into the y array
        _mm256_store_ps(&y[i], result_vec);
    }
}
*/

// NEW ROUTINE1 | VECTORISED
void routine1_vec(float alpha, float beta) {

    unsigned int i; // loop counter, usigned so the computer knows it's not a negative number

    // Create AVX vectors for alpha and beta
    __m256 alpha_vec = _mm256_set1_ps(alpha); // set1_ps sets all elements of alpha to alpha_vec, which holds 8 elements
    __m256 beta_vec = _mm256_set1_ps(beta); // set1_ps sets all elements of beta to beta_vec, which holds 8 elements

    // alpha_vec = |0.023|0.023|0.023|0.023|0.023|0.023|0.023|0.023|
    // beta_vec = |0.045|0.045|0.045|0.045|0.045|0.045|0.045|0.045|

    // process 8 elements at a time for each iteration, until i reaches M
    for (i = 0; i < M; i += 8) {

        // & means the address of the variable, so &y[i] means the address of the y[i] variable
        __m256 y_vec = _mm256_load_ps(&y[i]); // load 8 seperate iterated elements from y into AVX register
        __m256 z_vec = _mm256_load_ps(&z[i]); // load 8 seperate iterated elements from z into AVX register

        // Perform the vectorized operations
        __m256 result_vec = _mm256_add_ps(_mm256_mul_ps(alpha_vec, y_vec),_mm256_mul_ps(beta_vec, z_vec));
        // result_vec = (alpha * y_vec) + (beta * z_vec)
        // This is done for 8 elements at a time^

        // Store the results back into the y array
        _mm256_store_ps(&y[i], result_vec);
    }

    // Catch any remaining instances (continues from where previous loop left off)
    for (; i < M; i++) {
        y[i] = (alpha * y[i]) + (beta * z[i]);
    }

}
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ROUTINE2

// ROUTINE2 | NON-OPTIMISED
void routine2(float alpha, float beta) {

    unsigned int i, j;


    for (i = 0; i < N; i++) // 2d array iteration?
        for (j = 0; j < N; j++)
            w[i] = (w[i] - beta) + (alpha * A[i][j] * x[j]);

}

/*
// OLD ROUTINE2 | VECTORISED
void routine2_vec(float alpha, float beta) {

    unsigned int i = 0, j = 0; // init loop counters with 0 for i and j,

        set1_ps sets all elements of alpha and beta to a vector
        which each hold 8 elements. It could also be said to broadcast the values
        to all elements of the 256-bit vector, you replicate a
        single float value across all elements of each 256-bit vector

   

    __m256 alpha_vec = _mm256_set1_ps(alpha);
    __m256 beta_vec = _mm256_set1_ps(beta);

    for (i = 0; i < N; i++) { // outer-loop for rows of matrix A and vector.

   
            Initializes three 256-bit vectors, each vector set to its respective current
            values: A[i][j], x[j], and w[j], for parallel processing.
   

        __m256 a_vec = _mm256_set1_ps(A[i][j]);
        __m256 x_vec = _mm256_set1_ps(x[j]);
        __m256 w_vec = _mm256_set1_ps(w[j]);

        // outer loop for rows of matrix A and vector.
        __m256 sum_vec = _mm256_setzero_ps(); // Initialise the accumulator vector as zero

        for (j = 0; j < N; j += 8) { // vectorisation beins, process 8 elements at a time

            __m256 a_vec = _mm256_load_ps(&A[i][j]); // Load elements from A
            __m256 w_vec = _mm256_load_ps(&w[i]); // Load elements from w
            __m256 x_vec = _mm256_load_ps(&x[j]); // Load elements from x

           
             
               (w[i] - beta) + ((alpha * A[i][j]) * x[j]);
                Order of Operations:

                1. A = alpha*A[i][j]
                2. B = A*x[j]
                3. C = (w[i]-beta)
                4. sum_vec = B+C

                ^^This is process happens 8-times in a single-iteration^^

           
         
            __m256 vec_A = _mm256_mul_ps(alpha_vec, a_vec); // (alpha*A[i][j])
            __m256 vec_B = _mm256_mul_ps(vec_A, x_vec); // (vec_A) * x[j]
            __m256 vec_C = _mm256_sub_ps(w_vec, beta_vec); // Compute (w[i] - b)

            // sum up elements 8 times in a single-iteration
            sum_vec = _mm256_add_ps(vec_B, vec_C);
        }

        // Store calculations that're complete for 8 single-precision, floating-point values
        _mm256_store_ps(&w[i], sum_vec);
    }
}
*/

// NEW ROUTINE2 | VECTORISED

    /*
    [[X]]
    [X] First, you needed to use hadd instruction to add all the values in sum_vec.
    [X] Second, you needed to load/store one element of w[i] not eight. Right now you store
        8 elements in memory and you overwrite the output; this is problematic in the last
        7 iterations as you store outside of the array's bounds.
    [X] Furthermore, you did not take into account the case where N%8 is not zero.

    (w[i] - beta) + ((alpha * A[i][j]) * x[j]);
    */

   void routine2_vec(float alpha, float beta) {
        unsigned int i = 0, j = 0; // init loop counters with 0 for i and j
        // Create AVX vectors for alpha and beta
        __m256 alpha_vec = _mm256_set1_ps(alpha); // set1_ps sets all elements of alpha to alpha_vec, which holds 8 elements
        __m256 beta_vec = _mm256_set1_ps(beta); // set1_ps sets all elements of beta to beta_vec, which holds 8 elements

        // Outer loop: ensure we're only loading/storing one element of w[i] at a time, thus not overwriting output
        for (i = 0; i < N; i++) {
            // (w[i] - beta) + ((alpha * A[i][j]) * x[j]);
            __m256 sum_vec = _mm256_setzero_ps(); // init sum_vec before accumulation
            __m256 w_vec = _mm256_set1_ps(w[i]); // Broadcast w[i] into all 8 elements of the AVX vector w_vec
            __m256 w_minus_beta_vec = _mm256_sub_ps(w_vec, beta_vec); // init w_minus_beta_vec as (w[i] - beta)

            for (j = 0; j < N - 7; j += 8) { // begin vectorisation, process 8 elements at a time
                __m256 a_vec = _mm256_load_ps(&A[i][j]); // load 8 elements from A
                __m256 x_vec = _mm256_load_ps(&x[j]); // load 8 elements from x

                // 1. A = alpha * A[i][j]
                __m256 vec_A = _mm256_mul_ps(alpha_vec, a_vec);
                // 2. B = A * x[j]
                __m256 vec_B = _mm256_mul_ps(vec_A, x_vec);
                // 3. C = (w[i] - beta) + B
                __m256 vec_C = _mm256_add_ps(w_minus_beta_vec, vec_B);
                // 4. complete the formula BEFORE horizontal addition
                sum_vec = _mm256_add_ps(sum_vec, vec_C);
            }

            // Catch any remaining instances (continues from where previous loop left off)
            float sum_remainder = 0.0f;
            for (; j < N; j++) { // process remaining 'j' instances
                sum_remainder += (w[i] - beta) + (alpha * A[i][j] * x[j]);
            } // catch remaining instances after 8-element vectorisation; store in sum_remainder for later addition to total sum

            /* Perform horizontal addition to sum up the elements in sum_vec
            -----------------------------------------------------------------
            1. Extract the lower 128 bits of the 256-bit vector
            2. Extract the upper 128 bits of the 256-bit vector
            3. Add the two 128-bit vectors

            _mm246_castps256_ps128: Returns the lower 128 bits of a 256-bit floating-point vector of [8 x float] as a 128-bit floating-point vector of [4 x float].
            _mm256_extractf128_ps: Extract 128-bit from the upper half, 1 specifies the upper half

            _mm256_extractf128_ps could be used to extract the lower-half, however it has an additional argument,
            where as _mm256_castps256_ps128 immediately expects the lower-half thus is more efficient.

            sum_lo = lower 128 bits of sum_vec
            sum_hi = upper 128 bits of sum_vec
            sum_128 = adds the two 128-bit vectors (sum_lo + sum_hi) using horizontal addition

            */
            __m128 sum_lo = _mm256_castps256_ps128(sum_vec);
            __m128 sum_hi = _mm256_extractf128_ps(sum_vec, 1); // 1 to specify the upper half
            __m128 sum_128 = _mm_add_ps(sum_lo, sum_hi);
            sum_128 = _mm_hadd_ps(sum_128, sum_128);
            sum_128 = _mm_hadd_ps(sum_128, sum_128);

            // Extract the final sum from the vector to a scalar float
            float sum = _mm_cvtss_f32(sum_128); // Convert the lower 32 bits (first element) of the 128-bit vector to a scalar float
            sum += sum_remainder; // add the remainder sum
            /*
            _mm_cvtss_f32 extracts the lowest 32 bits (first float) from the 128-bit vector.
            After the horizontal additions, this lowest 32-bit float contains the total sum of the original vector elements.
            We extract this first float as it contains the desired sum, since all 4 floats are the same it doesn't
            matter which one is extracted, however extracting the first is the most efficient because
            it uses the least instructions, similar to using _mm256_castps256_ps128 to extract the lower 128 bits.

            Finally, remember to add the remainder sum to the total sum; then store the result in w[i] for each iteration
           
            */
            // Store result
            w[i] = sum;
           
        }
    }
    
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------