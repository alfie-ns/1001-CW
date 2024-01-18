






/*

    for (i = 0; i < N; i++) // 2d array iteration
    for (j = 0; j < N; j++)
        w[i] = (w[i] - beta) + (alpha * A[i][j] * x[j]);




            w[i] = (w[i] - beta) + (alpha * A[i][j] * x[j]);
            w's iteration = (w's iteration - beta) +
            (alpha times higher loop iteration of A as
            well as 'j', finally times x's j iterations
*/







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







/*

    Routine2: Updates each iterated element of the array w[i]
    by overlapping i and j, and for each one, first subtracting beta and adding the product of alpha
    times matrix-vector product of row i and j for A,  finally times by x[j]

*/

  /*
        w[i] = (w[i] - beta) + (alpha * A[i][j] * x[j]);
        w's iteration = (w's iteration - beta) +
        (alpha times higher loop iteration of A as
        well as 'j', finally times x's j iterations
    */
} // [x] TODO: Make routine2_vec

/*

     256-bit register is divided into eight 32-bit slots.
     Each slot can hold one single-precision floating-point number (float).
     So process eight float values in parallel.

*/

    /*
      ** AVX intrinsic Functions: **

        __m256: means a 256-bit variable that can hold eight 32-bit single-precision floating-point values.
        _mm256_load_ps: loads eight 32-bit single-precision floating-point values.
        _mm256_store_ps: stores eight 32-bit single-precision floating-point values into memory.
        _mm256_add_ps: adds eight 32-bit single-precision floating-point values.
        _mm256_mul_ps: multiplies eight 32-bit single-precision floating-point values
        _mm256_setzero_ps: initializes all elements of this 256-bit vector to zero.
        _mm256_set1_ps: initializes all elements of this 256-bit vector with the same single-precision floating-point value.
    /*

         "i" is the row of the matrix, while "j" is the column of the matrix.

          for (i = 0; i < N; i++)
            for (j = 0; j < N; j++)
                w[i] = (w[i] - beta) + (alpha * A[i][j] * x[j]);

            replicates the value of the alpha variable and fills
            all eight 32-bit slots within alpha_vec. Each slot is
            a different location within the register, and all slots
            will contain the exact same value, allowing for parallel
            computation
        */



















        void routine2_vec(float alpha, float beta) {
//
//    unsigned int i, j;
//
//    __m256 alpha_vec = _mm256_set1_ps(alpha);
//    // essentially there's 8 32-bit seperate float alpha = 0.023f; iterated elements
//    __m256 beta_vec = _mm256_set1_ps(beta);
//    // essentially there's 8 32-bit seperate float beta = 0.045f; iterated elements
//
//
//
//    for (i = 0; i < N; i++) { // Note: Not processing 8 w[i] values at a time here
//        __m256 sum_vec = _mm256_setzero_ps(); // To accumulate sums
//
//        for (j = 0; j < N; j += 8) { // Process 8 elements of A and x at a time
//            __m256 a_vec = _mm256_load_ps(&A[i][j]); // Load 8 elements from A[i][j...j+7]
//            __m256 x_vec = _mm256_load_ps(&x[j]);   // Load 8 elements from x[j...j+7]
//
//            // Multiply and accumulate
//            sum_vec = _mm256_add_ps(sum_vec, _mm256_mul_ps(alpha_vec, _mm256_mul_ps(a_vec, x_vec)));
//        }
//
//        // Horizontal sum of sum_vec to get a single float result
//        float partialSums[8];
//        _mm256_store_ps(partialSums, sum_vec);
//        float dotProduct = partialSums[0] + partialSums[1] + partialSums[2] + partialSums[3] +
//            partialSums[4] + partialSums[5] + partialSums[6] + partialSums[7];
//
//        // Now modify w[i]
//        w[i] = (w[i] - beta) + dotProduct; // Adjust this line as per actual computation
//    }
//
//}


// SSE implementation of routine1

//void routine1_vec(float alpha, float beta) {
//
//	unsigned int i; // loop counter
//
//	// Create SSE vectors for alpha and beta
//	__m128 alpha_vec = _mm_set1_ps(alpha); // set1_ps sets all elements of alpha to alpha_vec, which holds 4 elements
//	// essentially there's 4 seperate float alpha = 0.023f; iterations
//	// alpha_vec = |0.023f|0.023f|0.023f|0.023f|
//
//	__m128 beta_vec = _mm_set1_ps(beta); // set1_ps sets all elements of beta to beta_vec, which holds 4 elements
//	// essentially there's 4 seperate float beta = 0.045f; iterations
//	// beta_vec = |0.045|0.045|0.045|0.045|
//
//	// process 4 elements at a time for each iteration, until i reaches M
//    for (i = 0; i < M; i += 4) {
//
//		// & means the address of the variable, so &y[i] means the address of the y[i] variable
//		__m128 y_vec = _mm_load_ps(&y[i]); // load 4 seperate iterated elements from y into AVX register 
//		__m128 z_vec = _mm_load_ps(&z[i]); // load 4 seperate iterated elements from z into AVX register
//
//		// Perform the vectorized operations
//		__m128 result_vec = _mm_add_ps(_mm_mul_ps(alpha_vec, y_vec), _mm_mul_ps(beta_vec, z_vec));
//		// result_vec = alpha * y_vec + beta * z_vec
//
//		// Store the results back into the y array
//		_mm_store_ps(&y[i], result_vec);
//	}
//
//	// Handle any remaining elements
//    for (; i < M; i++) {
//		y[i] = alpha * y[i] + beta * z[i];
//	}
//}

