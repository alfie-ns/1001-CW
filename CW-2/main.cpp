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

#define EPSILON 0.0001 // tolerance of which two floats are considered equal


//function declarations
void initialize(); // init function intializes the arrays

void routine1(float alpha, float beta); // calls routine1 with alpha and beta
void routine2(float alpha, float beta); // calls routine2 with alpha and beta

void routine1_vec(float alpha, float beta); // calls routine1_vec with alpha and beta
void routine2_vec(float alpha, float beta); // calls routine2_vec with alpha and beta

unsigned short int equal(float a, float b); // function to check equality within tolerance
/*
    Routine1: y[i] = (alpha * y[i]) + (beta * z[i]);
    Routine2: w[i] = w[i] = (w[i] - beta) + (alpha * A[i][j] * x[j]);

    declspec(align(64)) is used to align the arrays to 64-byte boundaries

    M is the size of the arrays for routine1
    N is the size of the arrays for routine2

    [x] TODO: Make routine2_vec
    [x] TODO: TEST IF VECTORISED CALCULATIONS = NON-OPTIMISED VERSION WITH EQUAL FUNCTION
    [x] RESULTS DO NOT MATCH. 

    git still tracks my .vs folder even though i have .vs/ in my .gitignore file, maybe as it was there from the start?
*/

__declspec(align(64)) float  y[M], z[M]; // declare arrays as 64-byte aligned
__declspec(align(64)) float A[N][N], x[N], w[N]; // declare arrays as 64-byte aligned

__declspec(align(64)) float y_copy[M]; // declare array that holds cloned y array for testing
__declspec(align(64)) float w_copy[N]; // declare array that holds cloned w array for testing

int main() {

    float alpha = 0.023f; // float value of 0.023 for beta alpha variable
    float beta = 0.045f; // float value of 0.045 for float beta variable

    double run_time; // double-type variable to store the execution time, for testing 
    double start_time; // double-type variable to store the start time, for testing
    
    unsigned int t; // unsigned int-type variable to store the number of times the routines are executed

    // double-type speedup variables to store the speedup of the vectorised versions vs the routines
    double non_optimized_time1, optimized_time1, speedup1;
    double non_optimized_time2, optimized_time2, speedup2;

    initialize(); // initialise the arrays




    /*

    EXAMPLE FROM LAB-SESSION:
    
    unsigned short int ConstAdd_AVX() {

	    __m256  ymm1, ymm2, ymm3;
	    int i;

	    ymm1 = _mm256_set_ps(2.1234f, 2.1234f, 2.1234f, 2.1234f, 2.1234f, 2.1234f, 2.1234f, 2.1234f); //set num1 values
	    for (i = 0; i < M; i += 8) { //IMPORTANT: M MUST BE A MULTIPLE OF 8, OTHERWISE IT DOES NOT WORK
		    ymm2 = _mm256_loadu_ps(&V2[i]); //load 8 elements of X2[]
		    ymm3 = _mm256_add_ps(ymm1, ymm2); //num3 = num1 + num2
		    _mm256_storeu_ps(&V1[i], ymm3); //store num3 to Y[i]. num3 has 8 FP values which they are stored into Y[i], Y[i+1], Y[i+2], Y[i+3], .. Y[i+7]
	    }


	    return 2;
    }
    
    */



    printf("\n-----------------NON-OPTIMISED------------------------------\n");

    printf("\nRoutine1:");
    start_time = omp_get_wtime(); //start timer

    for (t = 0; t < TIMES1; t++) // for loop to execute routine1 TIMES1 times
        routine1(alpha, beta); // init with alpha and beta
    
    run_time = omp_get_wtime() - start_time; //end timer
    printf("\n Time elapsed is %f secs \n %e FLOPs achieved\n", run_time, (double)(ARITHMETIC_OPERATIONS1) / ((double)run_time / TIMES1)); // print testing
    std::copy(y, y + M, y_copy); // copy routine1 to test comparison with vectorised version, y+M to check for last element
    non_optimized_time1 = omp_get_wtime() - start_time; // store the time-of non-optimised version of routine1

    initialize(); // reinitialise the arrays 

    printf("\nRoutine2:");
    start_time = omp_get_wtime(); //start timer

    for (t = 0; t < TIMES2; t++)
        routine2(alpha, beta);
    
    
    run_time = omp_get_wtime() - start_time; //end timer
    printf("\n Time elapsed is %f secs \n %e FLOPs achieved\n", run_time, (double)(ARITHMETIC_OPERATIONS2) / ((double)run_time / TIMES2)); // print testing
    std::copy(w, w + N, w_copy); // copy routine2 to test comparison with vectorised version, w+N to check for last element
    non_optimized_time2 = omp_get_wtime() - start_time; // store the time-of non-optimised version of routine2

    initialize(); // reinitialise arrays 

    printf("\n-----------------VECTORISED------------------------------\n");

    // [x] make copies before running vectorised versions


    printf("\nRoutine1_vec:");
    start_time = omp_get_wtime(); //start timer

    for (t = 0; t < TIMES1; t++) // for loop to execute routine1 TIMES1 times
        routine1_vec(alpha, beta); // init with alpha and beta

    // y, a global declared 64-bit float, now becomes what the vectorised version of routine1 has calculated

    run_time = omp_get_wtime() - start_time; //end timer
    printf("\n Time elapsed is %f secs \n %e FLOPs achieved\n", run_time, (double)(ARITHMETIC_OPERATIONS1) / ((double)run_time / TIMES1)); // print testing
    optimized_time1 = omp_get_wtime() - start_time; // store the time taken for the vectorised version of routine1

    initialize(); // reinitialise the arrays

    printf("\nRoutine2_vec:");
    start_time = omp_get_wtime(); //start timer

    for (t = 0; t < TIMES2; t++)
        routine2_vec(alpha, beta);

    // w now becomes what the vectorised version of routine2 has calculated

    run_time = omp_get_wtime() - start_time; //end timer
    printf("\n Time elapsed is %f secs \n %e FLOPs achieved\n", run_time, (double)(ARITHMETIC_OPERATIONS2) / ((double)run_time / TIMES2)); // print testing
    optimized_time2 = omp_get_wtime() - start_time; // store the time taken for the vectorised version of routine2

    printf("\n-----------------RESULTS------------------------------\n");
    speedup1 = non_optimized_time1 / optimized_time1; // calculate speedup for routine1
    printf("\nSpeedup for Routine1: %f times", speedup1);
    speedup2 = non_optimized_time2 / optimized_time2; // calculate speedup for routine2
    printf("\nSpeedup for Routine2: %f times\n\n", speedup2);

    bool arraysAreEqual; // bool value to check if arrays are equal

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
	arraysAreEqual = true;
    if (arraysAreEqual) {
        printf("Routine1_vec: Results match.\n");
    }
    else {
        printf("Routine1_vec: Results do not match!\n");
    }
    arraysAreEqual = false;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
			if (equal(w_copy[i], w[i])) { // Use the equal function to compare elements
            	arraysAreEqual = true;
            	break;
            }
            
		}
    }
    if (arraysAreEqual) {
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

 */


void routine1(float alpha, float beta) { // routine1: y[i] = alpha * y[i] + beta * z[i];

    unsigned int i; // loop counter

    for (i = 0; i < M; i++)
        y[i] = (alpha * y[i]) + (beta * z[i]); // for each iteration of the array, i = (alpha * y[i]) + (beta * z[i])
}

// AVX implementation of routine1

void routine1_vec(float alpha, float beta) {

    unsigned int i; // loop counter, usigned so the computer knows it's not a negative number

    // Create AVX vectors for alpha and beta
    __m256 alpha_vec = _mm256_set1_ps(alpha); // set1_ps sets all elements of alpha to alpha_vec, which holds 8 elements
    __m256 beta_vec = _mm256_set1_ps(beta); // set1_ps sets all elements of beta to beta_vec, which holds 8 elements

    // there's 8 seperate floats, alpha_vec and beta_vec hold 8 elements each:
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

        // Store the results back into the y array
        _mm256_store_ps(&y[i], result_vec);
    }

    // IS THIS NECCESSARY? [x]
   /* 
        for (i = M - (M % 8); i < M; i++) 
		    y[i] = (alpha * y[i]) + (beta * z[i]); 

        this was not neccesary because: M is a multiple of 8, so the loop will always end perfectly
   */
}




void routine2(float alpha, float beta) {

    unsigned int i, j;


    for (i = 0; i < N; i++) // 2d array iteration?
        for (j = 0; j < N; j++)
            w[i] = (w[i] - beta) + (alpha * A[i][j] * x[j]);

}


void routine2_vec(float alpha, float beta) {

    unsigned int i = 0, j = 0; // init loop counters with 0 for i and j, 

    /*

        set1_ps sets all elements of alpha and beta to a vector
        which each hold 8 elements. It could also be said to broadcast the values
        to all elements of the 256-bit vectors, you essentially replicate a
        single float value across all elements of each 256 - bit vector

    */

    __m256 alpha_vec = _mm256_set1_ps(alpha);
    __m256 beta_vec = _mm256_set1_ps(beta);

    /*
        
        init before asigning values to vec_A, vec_B, vec_C,
        these values are used for the calculations, so should
        be initialised before the loop

    */

    for (i = 0; i < N; i++) {
        /*
            Initializes three 256-bit vectors, each vector set to its respective current
            values: A[i][j], x[j], and w[j], respectively, for parallel processing.
        */

        __m256 a_vec = _mm256_set1_ps(A[i][j]);
        __m256 x_vec = _mm256_set1_ps(x[j]);
        __m256 w_vec = _mm256_set1_ps(w[j]);

        // outer loop for rows of matrix A and vector. 
        __m256 sum_vec = _mm256_setzero_ps(); // Initialise the accumulator vector as zero

        for (j = 0; j < N; j += 8) {

            __m256 a_vec = _mm256_load_ps(&A[i][j]); // Load elements from A
            __m256 w_vec = _mm256_load_ps(&w[i]); // Load elements from w
            __m256 x_vec = _mm256_load_ps(&x[j]); // Load elements from x

            /*
             
               (w[i] - beta) + ((alpha * A[i][j]) * x[j]);
                Order of Operations:

                1. A = alpha*A[i][j] 
                2. B = A*x[j] 
                3. C = (w[i]-beta) 
                4. sum_vec = B+C

                ^^This is process happens 8-times in a single-iteration^^

            */

            __m256 vec_A = _mm256_mul_ps(alpha_vec, a_vec); // Compute (alpha*A[i][j])
            __m256 vec_B = _mm256_mul_ps(vec_A, x_vec); // Compute (vec_A) * x[j]
            __m256 vec_C = _mm256_sub_ps(w_vec, beta_vec); // Compute (w[i] - b)

            // sum up elements 8 times in a single-iteration 
            sum_vec = _mm256_add_ps(vec_B, vec_C);
        }

        // Store calculations that're complete for 8 single-precision, floating-point values
        _mm256_store_ps(&w[i], sum_vec);
    }
}

// from lab-session:
unsigned short int equal(float a, float b) {
    float temp = a - b;
    //printf("\n %f  %f", a, b);
    if ((fabs(temp) / fabs(b)) < EPSILON)
        return 0; //success
    else
        return 1; //wrong result
}

















































