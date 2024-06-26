# 1001 q3 [ ]

### A) 7-mark[x]:

- MAIN_LINUX.CPP IS USED FOR BASH SCRIPT

### B) 30-mark[ ]:

- Use ‘sprintf’ and ‘strcat’ functions. Note that the input images are of different sizes. To process multiple images, either statically define separate arrays for each image (not recommended) or allocate the arrays dynamically (good practice). You can process multiple images by either creating multiple copies of the code provided (not recommended) or by using a loop (good practice). All solutions are acceptable, but your mark will be based on how efficient your implementation is. For a non-loop approach, process only three images (mark will be the same regardless of the number of images processed).
  - Recommended: Dynamic allocated arrays

### C) 12-mark[ ]:

#### Original Function:

```C

Void Sobel() {
    int row, col, rowOffset, colOffset;
    int Gx, Gy;
    // Determine edge directions and gradient strengths
    for (row = 1; row < N - 1; row++) {
        for (col = 1; col < M - 1; col++) {
            Gx = 0;
            Gy = 0;
            // Calculate the sum of the Sobel mask times the nine surrounding pixels in the x and y direction
            for (rowOffset = -1; rowOffset <= 1; rowOffset++) {
                for (colOffset = -1; colOffset <= 1; colOffset++) {
                    Gx += filt[M * (row + rowOffset) + col + colOffset] * GxMask[rowOffset + 1][colOffset + 1];
                    Gy += filt[M * (row + rowOffset) + col + colOffset] * GyMask[rowOffset + 1][colOffset + 1];
                }
            }
            gradient[M * row + col] = (unsigned char)sqrt(Gx * Gx + Gy * Gy); // Calculate gradient strength
            //gradient[row][col] = abs(Gx) + abs(Gy); // optimized version
        }   
    }
}
```

### 12-mark Intrinsic Sobel() Function step-to-step:

- `gradient[M*row + col] = (unsigned char) sqrt(Gx*Gx+Gy*Gy)`
1. Gx*Gx: `Gx mm256_mul_ps Gx`
2. Gy*Gy: `Gy mm256_mul_ps Gy`
3. A+B: `A mm256_add_ps B`
4. sqrt C: `mm_csqrt_ps C`
5. `(unsigned char) * D`: `unisgned char mm256_mull_ps D`
6. `gradient[M*row + col] = E`

**Broadly speaking**
  1. Take an input in C++ from the command line.
  2. Store the variables (and their values) in a way that makes them accessible by your function (i.e., store it globally or have a way of getting it from other functions).
  3. Within your function, access these global variables and process them.

