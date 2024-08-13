#include <stdio.h>
#include <chrono>
#include <iostream>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <arm_neon.h>

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

alignas(16) float y[M], z[M];
alignas(16) float A[N][N], x[N], w[N];
alignas(16) float y_copy[M];
alignas(16) float w_copy[N];

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
        if (!equal(y_copy[i], y[i])) { // if equal function from the lab session fails
            printf("Routine1: Results do not match!\n");
            arraysAreEqual = false;
            break;
        }
    }
    if (arraysAreEqual) {
        printf("Routine1 <-> Routine1_vec. Results match.\n");
    }

    arraysAreEqual = true;
    for (int i = 0; i < N; i++) { // loop through all instances of the w[i] iteration
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

// 32-bit vectorisation

void routine1(float alpha, float beta) {
    unsigned int i;
    for (i = 0; i < M; i++)
        y[i] = (alpha * y[i]) + (beta * z[i]);
}

void routine1_vec(float alpha, float beta) {
    unsigned int i;
    float32x4_t alpha_vec = vdupq_n_f32(alpha);
    float32x4_t beta_vec = vdupq_n_f32(beta);

    for (i = 0; i < M; i += 4) { // process 4 elements at a time
        float32x4_t y_vec = vld1q_f32(&y[i]); // load 4 elements from y (32-bit vector)
        float32x4_t z_vec = vld1q_f32(&z[i]); // load 4 elements from z (32-bit vector)
        float32x4_t result_vec = vmlaq_f32(vmulq_f32(alpha_vec, y_vec), beta_vec, z_vec);
        vst1q_f32(&y[i], result_vec);
    }

    for (; i < M; i++) { // catch remaining elements
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
    float32x4_t alpha_vec = vdupq_n_f32(alpha);
    float32x4_t beta_vec = vdupq_n_f32(beta);

    for (i = 0; i < N; i++) {
        float32x4_t sum_vec = vdupq_n_f32(0.0f);
        float32x4_t w_vec = vdupq_n_f32(w[i]);
        float32x4_t w_minus_beta_vec = vsubq_f32(w_vec, beta_vec);

        for (j = 0; j < N - 3; j += 4) {
            float32x4_t a_vec = vld1q_f32(&A[i][j]);
            float32x4_t x_vec = vld1q_f32(&x[j]);
            float32x4_t vec_A = vmulq_f32(alpha_vec, a_vec);
            float32x4_t vec_B = vmulq_f32(vec_A, x_vec);
            sum_vec = vaddq_f32(sum_vec, vec_B);
        }

        float sum = vgetq_lane_f32(sum_vec, 0) + vgetq_lane_f32(sum_vec, 1) +
                    vgetq_lane_f32(sum_vec, 2) + vgetq_lane_f32(sum_vec, 3);

        for (; j < N; j++) {
            sum += alpha * A[i][j] * x[j];
        }

        w[i] = (w[i] - beta) + sum;
    }
}