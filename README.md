
## 1001-q1 Vectorisation of Code [x]

**Routine1**:  ``` y[i] = alpha * y[i] + beta * z[i]; ```

**Routine2**: ``` w[i] = w[i] - beta + alpha * A[i][j] * x[j]; ```

**Checklist**
- [x] make routine1_vec
- [x] make routine2_vec
- [x] make comparison-functionality
- [x] get copy from normal functions and use that in comparison-funciton

## AVX

### Example Function: ConstAdd_AVX
```
unsigned short int ConstAdd_AVX() {
    __m256  ymm1, ymm2, ymm3;
    int i;

    ymm1 = _mm256_set_ps(2.1234f, 2.1234f, 2.1234f, 2.1234f, 2.1234f, 2.1234f, 2.1234f, 2.1234f);
    for (i = 0; i < M; i += 8) {
        ymm2 = _mm256_loadu_ps(&V2[i]);
        ymm3 = _mm256_add_ps(ymm1, ymm2);
        _mm256_storeu_ps(&V1[i], ymm3);
    }
    return 2;
}

```

## AVX Version of Routine1 [x]
```
unsigned short int Routine1_AVX() {
    __m256 alpha_vec = _mm256_set1_ps(alpha);
    __m256 beta_vec = _mm256_set1_ps(beta);
    unsigned int i;

    for (i = 0; i < M; i += 8) {
        __m256 y_vec = _mm256_load_ps(&y[i]);
        __m256 z_vec = _mm256_load_ps(&z[i]);
        __m256 result_vec = _mm256_add_ps(_mm256_mul_ps(alpha_vec, y_vec), _mm256_mul_ps(beta_vec, z_vec));
        _mm256_store_ps(&y[i], result_vec);
    }
    return 2;
}
```

