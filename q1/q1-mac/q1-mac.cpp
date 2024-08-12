#include <stdio.h>
#include <time.h>
#include <pmmintrin.h>
#include <chrono>
#include <iostream>
#include <immintrin.h>
#include <cstring>
#include <cmath>

#define M 1024*512
#define ARITHMETIC_OPERATIONS1 3*M
#define TIMES1 1

#define N 8192
#define ARITHMETIC_OPERATIONS2 4*N*N
#define TIMES2 1

#define EPSILON 0.0001

void initialize();
void routine1(float alpha, float beta);
void routine2(float alpha, float beta);
void routine1_vec(float alpha, float beta);
void routine2_vec(float alpha, float beta);

alignas(64) float y[M], z[M];
alignas(64) float A[N][N], x[N], w[N];
alignas(64) float y_copy[M];
alignas(64) float w_copy[N];

unsigned short int equal(float a, float b);

int main() {
    float alpha = 0.023f;
    float beta = 0.045f;

    double run_time;
    std::chrono::high_resolution_clock::time_point start_time;
   
    unsigned int t;

    initialize();

    printf("\n-----------------NON-OPTIMISED------------------------------\n");

    printf("\nRoutine1:");
    start_time = std::chrono::high_resolution_clock::now();

    for (t = 0; t < TIMES1; t++)
        routine1(alpha, beta);
   
    run_time = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - start_time).count();
    printf("\n Time elapsed is %f secs \n %e FLOPs achieved\n", run_time, (double)(ARITHMETIC_OPERATIONS1) / ((double)run_time / TIMES1));
    std::copy(y, y + M, y_copy);

    initialize();

    printf("\nRoutine2:");
    start_time = std::chrono::high_resolution_clock::now();

    for (t = 0; t < TIMES2; t++)
        routine2(alpha, beta);
   
    run_time = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - start_time).count();
    printf("\n Time elapsed is %f secs \n %e FLOPs achieved\n", run_time, (double)(ARITHMETIC_OPERATIONS2) / ((double)run_time / TIMES2));
    std::copy(w, w + N, w_copy);
   
    initialize();

    printf("\n-----------------VECTORISED------------------------------\n");

    printf("\nRoutine1_vec:");
    start_time = std::chrono::high_resolution_clock::now();

    for (t = 0; t < TIMES1; t++)
        routine1_vec(alpha, beta);

    run_time = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - start_time).count();
    printf("\n Time elapsed is %f secs \n %e FLOPs achieved\n", run_time, (double)(ARITHMETIC_OPERATIONS1) / ((double)run_time / TIMES1));
   
    initialize();

    printf("\nRoutine2_vec:");
    start_time = std::chrono::high_resolution_clock::now();

    for (t = 0; t < TIMES2; t++)
        routine2_vec(alpha, beta);

    run_time = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - start_time).count();
    printf("\n Time elapsed is %f secs \n %e FLOPs achieved\n", run_time, (double)(ARITHMETIC_OPERATIONS2) / ((double)run_time / TIMES2));

    printf("\n");

    printf("-----------------TESTING------------------------------\n\n");

    bool arraysAreEqual;

    arraysAreEqual = true;
    for (int i = 0; i < M; i++) {
        if (!equal(y_copy[i], y[i])) {
            printf("Routine1: Results do not match!\n");
            arraysAreEqual = false;
            break;
        }
    }
    if (arraysAreEqual) {
        printf("Routine1 <-> Routine1_vec. Results match.\n");
    }

    arraysAreEqual = true;
    for (int i = 0; i < N; i++) {
        if (!equal(w_copy[i], w[i])) {
            printf("Routine2_vec: Results do not match!\n");
            arraysAreEqual = false;
            break;
        }
    }
    if (arraysAreEqual) {
        printf("Routine2 <-> Routine2_vec: Results match.\n");
    }

    return 0;
}

void initialize() {
    unsigned int i, j;

    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++) {
            A[i][j] = (i % 99) + (j % 14) + 0.013f;
        }

    for (i = 0; i < N; i++) {
        x[i] = (i % 19) - 0.01f;
        w[i] = (i % 5) - 0.002f;
    }

    for (i = 0; i < M; i++) {
        z[i] = (i % 9) - 0.08f;
        y[i] = (i % 19) + 0.07f;
    }
}

unsigned short int equal(float a, float b) {
    float temp = a - b;
    if ((fabs(temp) / fabs(b)) < EPSILON)
        return 0;
    else
        return 1;
}

void routine1(float alpha, float beta) {
    unsigned int i;
    for (i = 0; i < M; i++)
        y[i] = (alpha * y[i]) + (beta * z[i]);
}

void routine1_vec(float alpha, float beta) {
    unsigned int i;
    __m256 alpha_vec = _mm256_set1_ps(alpha);
    __m256 beta_vec = _mm256_set1_ps(beta);

    for (i = 0; i < M; i += 8) {
        __m256 y_vec = _mm256_load_ps(&y[i]);
        __m256 z_vec = _mm256_load_ps(&z[i]);
        __m256 result_vec = _mm256_add_ps(_mm256_mul_ps(alpha_vec, y_vec),_mm256_mul_ps(beta_vec, z_vec));
        _mm256_store_ps(&y[i], result_vec);
    }

    for (; i < M; i++) {
        y[i] = (alpha * y[i]) + (beta * z[i]);
    }
}

void routine2(float alpha, float beta) {
    unsigned int i, j;
    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
            w[i] = (w[i] - beta) + (alpha * A[i][j] * x[j]);
}

void routine2_vec(float alpha, float beta) {
    unsigned int i = 0, j = 0;
    __m256 alpha_vec = _mm256_set1_ps(alpha);
    __m256 beta_vec = _mm256_set1_ps(beta);

    for (i = 0; i < N; i++) {
        __m256 sum_vec = _mm256_setzero_ps();
        __m256 w_vec = _mm256_set1_ps(w[i]);
        __m256 w_minus_beta_vec = _mm256_sub_ps(w_vec, beta_vec);

        for (j = 0; j < N - 7; j += 8) {
            __m256 a_vec = _mm256_load_ps(&A[i][j]);
            __m256 x_vec = _mm256_load_ps(&x[j]);
            __m256 vec_A = _mm256_mul_ps(alpha_vec, a_vec);
            __m256 vec_B = _mm256_mul_ps(vec_A, x_vec);
            __m256 vec_C = _mm256_add_ps(w_minus_beta_vec, vec_B);
            sum_vec = _mm256_add_ps(sum_vec, vec_C);
        }

        float sum_remainder = 0.0f;
        for (; j < N; j++) {
            sum_remainder += (w[i] - beta) + (alpha * A[i][j] * x[j]);
        }

        __m128 sum_lo = _mm256_castps256_ps128(sum_vec);
        __m128 sum_hi = _mm256_extractf128_ps(sum_vec, 1);
        __m128 sum_128 = _mm_add_ps(sum_lo, sum_hi);
        sum_128 = _mm_hadd_ps(sum_128, sum_128);
        sum_128 = _mm_hadd_ps(sum_128, sum_128);

        float sum = _mm_cvtss_f32(sum_128);
        sum += sum_remainder;
        w[i] = sum;
    }
}