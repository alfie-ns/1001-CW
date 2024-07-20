/*
------------------DR VASILIOS KELEFOURAS-----------------------------------------------------
------------------COMP1001 ------------------------------------------------------------------
------------------COMPUTER SYSTEMS MODULE-------------------------------------------------
------------------UNIVERSITY OF PLYMOUTH, SCHOOL OF ENGINEERING, COMPUTING AND MATHEMATICS---
*/

// Compile using: g++ q3.cpp -o q3 -O3 -lm  

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <emmintrin.h>
#include <limits.h>
#include <pmmintrin.h>
#include <immintrin.h>
#include <iostream>
#include <filesystem>

//function declarations
void Gaussian_Blur();
void Sobel();
int initialize_kernel();
void read_image(char* filename);
void read_image_and_put_zeros_around(char* filename);
void write_image2(char* filename, unsigned char* output_image);
void openfile(char* filename, FILE** finput);
int getint(FILE* fp);
void allocateArrays();
void deleteArrays();

void db(std::string msg) {
    std::cout << msg << std::endl;
}

//CRITICAL POINT: images' paths - You need to change these paths
// #define IN "/home/wave/Desktop/comp1001/code_to_start/input_images/a15.pgm"
// #define OUT "/home/wave/Desktop/comp1001/code_to_start/output_images/blurred.pgm"
// #define OUT2 "/home/wave/Desktop/comp1001/code_to_start/output_images/edge_detection.pgm"

//IMAGE DIMENSIONS
// #define M 475 //cols
// #define N 460 //rows
int M = -1; //cols
int N = -1; //rows
// intergers because size can change for each image

//CRITICAL POINT:these arrays are defined statically. Consider creating these arrays dynamically instead.
// unsigned char frame1[N*M];//input image
// unsigned char filt[N*M];//output filtered image
// unsigned char gradient[N*M];//output image

// create pointers for dynamic arrays allocation

unsigned char* frame1; //input image
unsigned char* filt; //output filtered image
unsigned char* gradient; //output image

const signed char Mask[5][5] = {//2d gaussian mask with integers
    {2,4,5,4,2} ,
    {4,9,12,9,4},
    {5,12,15,12,5},
    {4,9,12,9,4},
    {2,4,5,4,2}
};

const signed char GxMask[3][3] = {
    {-1,0,1} ,
    {-2,0,2},
    {-1,0,1}
};

const signed char GyMask[3][3] = {
    {-1,-2,-1} ,
    {0,0,0},
    {1,2,1}
};

char header[100];

int main(int argc, char* argv[]) {

    // compile with g++ -std=c++17 main.cpp -o q3 -O3 -lm

    // make sure the user passes in the input and output directories after the executable
    if (argc != 3) {
        std::cout << "Please pass in an input and output directory name" << std::endl;
        return 1;
    }

    // loads input and output directory into strings, specifiying the position of the agruments in argv
    std::string inputDirectoryPath = argv[1];
    std::string outputDirectoryPath = argv[2];
    std::cout << "Input Images Directory: " << inputDirectoryPath << std::endl;
    std::cout << "Output Images Directory: " << outputDirectoryPath << std::endl;

    // loop through all files in the input directory
    for (const auto& entry : std::filesystem::directory_iterator(inputDirectoryPath)) {
        // begin directory iteration

        std::string filename = entry.path().filename(); // sets filename to the current file

        char* inFile = new char[inputDirectoryPath.length() + filename.length() + 2];
        strcpy(inFile, (inputDirectoryPath + "/" + filename).c_str());

        char* blurFile = new char[outputDirectoryPath.length() + filename.length() + 6];
        strcpy(blurFile, (outputDirectoryPath + "/blur_" + filename).c_str());

        char* edgeFile = new char[outputDirectoryPath.length() + filename.length() + 16];
        strcpy(edgeFile, (outputDirectoryPath + "/edge_detected_" + filename).c_str());

        sprintf(inFile, "%s/%s", inputDirectoryPath.c_str(), filename.c_str());
        sprintf(blurFile, "%s/blur_%s", outputDirectoryPath.c_str(), filename.c_str());
        sprintf(edgeFile, "%s/edge_detected_%s", outputDirectoryPath.c_str(), filename.c_str());

        printf("\nProcessing image: %s\n", inFile);

        read_image(inFile);//read image from disc

        Gaussian_Blur(); //blur the image (reduce noise)
        Sobel(); //apply edge detection

        // this writes the images to the output directory
        write_image2(blurFile, filt); //store output image to the disc
        write_image2(edgeFile, gradient); //store output image to the disc


        deleteArrays(); //free memory
    }

    return 0;

}


void Gaussian_Blur() {

    int row, col, rowOffset, colOffset;
    int newPixel;
    unsigned char pix;
    //const unsigned short int size=filter_size/2;
    const unsigned short int size = 2;

    /*---------------------- Gaussian Blur ---------------------------------*/
    for (row = 0; row < N; row++) {
        for (col = 0; col < M; col++) {
            newPixel = 0;
            for (rowOffset = -size; rowOffset <= size; rowOffset++) {
                for (colOffset = -size; colOffset <= size; colOffset++) {

                    if ((row + rowOffset < 0) || (row + rowOffset >= N) || (col + colOffset < 0) || (col + colOffset >= M))
                        pix = 0;
                    else
                        pix = frame1[M * (row + rowOffset) + col + colOffset];

                    newPixel += pix * Mask[size + rowOffset][size + colOffset];

                }
            }
            filt[M * row + col] = (unsigned char)(newPixel / 159);

        }
    }

}

// Sobel() NON-OPTIMISED
// ----------------------
//void Sobel() {
//
//    int row, col, rowOffset, colOffset;
//    int Gx, Gy;
//
//    /*---------------------------- Determine edge directions and gradient strengths -------------------------------------------*/
//    for (row = 1; row < N - 1; row++) {
//        for (col = 1; col < M - 1; col++) {
//
//            Gx = 0;
//            Gy = 0;
//
//            /* Calculate the sum of the Sobel mask times the nine surrounding pixels in the x and y direction */
//            for (rowOffset = -1; rowOffset <= 1; rowOffset++) {
//                for (colOffset = -1; colOffset <= 1; colOffset++) {
//
//                    Gx += filt[M * (row + rowOffset) + col + colOffset] * GxMask[rowOffset + 1][colOffset + 1];
//                    Gy += filt[M * (row + rowOffset) + col + colOffset] * GyMask[rowOffset + 1][colOffset + 1];
//                }
//            }
//
//            gradient[M * row + col] = (unsigned char)sqrt(Gx * Gx + Gy * Gy); /* Calculate gradient strength       */
//            //gradient[row][col] = abs(Gx) + abs(Gy); // this is an optimized version of the above
//
//        }
//    }
//
//
//}
// Sobel() VECTORISED
// ----------------------
void Sobel() {
    const int SIMD_WIDTH = 8; // Number of 32-bit integers in a 256-bit AVX2 register

    // Load Sobel masks into 256-bit registers
    // _mm256_set_epi32: Sets 8 32-bit integers in a 256-bit register
    __m256i GxMask_row1 = _mm256_set_epi32(-1, 0, 1, -1, 0, 1, -1, 0);
    __m256i GxMask_row2 = _mm256_set_epi32(-2, 0, 2, -2, 0, 2, -2, 0);
    __m256i GxMask_row3 = _mm256_set_epi32(-1, 0, 1, -1, 0, 1, -1, 0);

    __m256i GyMask_row1 = _mm256_set_epi32(-1, -2, -1, -1, -2, -1, -1, -2);
    __m256i GyMask_row2 = _mm256_set_epi32(0, 0, 0, 0, 0, 0, 0, 0);
    __m256i GyMask_row3 = _mm256_set_epi32(1, 2, 1, 1, 2, 1, 1, 2);

    for (int row = 1; row < N - 1; row++) {
        for (int col = 1; col < M - 1; col += SIMD_WIDTH) {
            // _mm256_setzero_si256: Sets a 256-bit register to zero
            __m256i Gx = _mm256_setzero_si256();
            __m256i Gy = _mm256_setzero_si256();

            for (int rowOffset = -1; rowOffset <= 1; rowOffset++) {
                // _mm256_loadu_si256: Loads 256 bits of unaligned memory into a register
                __m256i pixels = _mm256_loadu_si256((__m256i*)&filt[M * (row + rowOffset) + col - 1]);
                
                __m256i GxMask, GyMask;
                if (rowOffset == -1) {
                    GxMask = GxMask_row1;
                    GyMask = GyMask_row1;
                } else if (rowOffset == 0) {
                    GxMask = GxMask_row2;
                    GyMask = GyMask_row2;
                } else {
                    GxMask = GxMask_row3;
                    GyMask = GyMask_row3;
                }

                // _mm256_mullo_epi32: Multiplies 8 pairs of 32-bit integers
                // _mm256_add_epi32: Adds 8 pairs of 32-bit integers
                Gx = _mm256_add_epi32(Gx, _mm256_mullo_epi32(pixels, GxMask));
                Gy = _mm256_add_epi32(Gy, _mm256_mullo_epi32(pixels, GyMask));
            }

            // Calculate gradient strength
            __m256i Gx_squared = _mm256_mullo_epi32(Gx, Gx);
            __m256i Gy_squared = _mm256_mullo_epi32(Gy, Gy);
            __m256i sum_squared = _mm256_add_epi32(Gx_squared, Gy_squared);

            // _mm256_cvtepi32_ps: Converts 8 32-bit integers to 8 32-bit vector of floats
            __m256 sum_squared_float = _mm256_cvtepi32_ps(sum_squared);
            // _mm256_sqrt_ps: Computes square root of 8 32-bit floats
            __m256 gradient_float = _mm256_sqrt_ps(sum_squared_float);

            // _mm256_cvtps_epi32: Converts 8 32-bit floats to 8 32-bit integers
            __m256i gradient_int = _mm256_cvtps_epi32(gradient_float);
            // _mm256_packus_epi32: Packs 8 32-bit integers into 16 16-bit integers with unsigned saturation
            __m256i gradient_byte = _mm256_packus_epi32(gradient_int, _mm256_setzero_si256());
            // _mm256_permute4x64_epi64: Rearranges 4 64-bit integers in a 256-bit register
            gradient_byte = _mm256_permute4x64_epi64(gradient_byte, 0xD8);
            // _mm256_castsi256_si128: Casts upper 128 bits of a 256-bit integer to a 128-bit integer
            // _mm_storeu_si128: Stores 128 bits of integer data to unaligned memory location
            _mm_storeu_si128((__m128i*)&gradient[M * row + col], _mm256_castsi256_si128(gradient_byte));
        }
    }
}

void Sobel() {
    const int SIMD_WIDTH = 8; // Number of 32-bit integers in a 256-bit AVX2 register

    // Load Sobel masks into 256-bit registers
    // _mm256_set_epi32: Sets 8 32-bit integers in a 256-bit register
    __m256i GxMask_row1 = _mm256_set_epi32(-1, 0, 1, -1, 0, 1, -1, 0);
    __m256i GxMask_row2 = _mm256_set_epi32(-2, 0, 2, -2, 0, 2, -2, 0);
    __m256i GxMask_row3 = _mm256_set_epi32(-1, 0, 1, -1, 0, 1, -1, 0);

    __m256i GyMask_row1 = _mm256_set_epi32(-1, -2, -1, -1, -2, -1, -1, -2);
    __m256i GyMask_row2 = _mm256_set_epi32(0, 0, 0, 0, 0, 0, 0, 0);
    __m256i GyMask_row3 = _mm256_set_epi32(1, 2, 1, 1, 2, 1, 1, 2);

    for (int row = 1; row < N - 1; row++) {
        for (int col = 1; col < M - SIMD_WIDTH; col += SIMD_WIDTH) {
            // _mm256_setzero_si256: Sets a 256-bit register to zero
            __m256i Gx = _mm256_setzero_si256();
            __m256i Gy = _mm256_setzero_si256();

            for (int rowOffset = -1; rowOffset <= 1; rowOffset++) {
                // _mm256_loadu_si256: Loads 256 bits of unaligned memory into a register
                __m256i pixels = _mm256_loadu_si256((__m256i*)&filt[M * (row + rowOffset) + col - 1]);
                
                __m256i GxMask, GyMask;
                if (rowOffset == -1) {
                    GxMask = GxMask_row1;
                    GyMask = GyMask_row1;
                } else if (rowOffset == 0) {
                    GxMask = GxMask_row2;
                    GyMask = GyMask_row2;
                } else {
                    GxMask = GxMask_row3;
                    GyMask = GyMask_row3;
                }

                // _mm256_mullo_epi32: Multiplies 8 pairs of 32-bit integers
                // _mm256_add_epi32: Adds 8 pairs of 32-bit integers
                Gx = _mm256_add_epi32(Gx, _mm256_mullo_epi32(pixels, GxMask));
                Gy = _mm256_add_epi32(Gy, _mm256_mullo_epi32(pixels, GyMask));
            }

            // Calculate gradient strength
            __m256i Gx_abs = _mm256_abs_epi32(Gx);
            __m256i Gy_abs = _mm256_abs_epi32(Gy);
            __m256i gradient_sum = _mm256_add_epi32(Gx_abs, Gy_abs);

            // _mm256_cvtepi32_ps: Converts 8 32-bit integers to 8 32-bit vector of floats
            __m256 gradient_float = _mm256_cvtepi32_ps(gradient_sum);
            // _mm256_sqrt_ps: Computes square root of 8 32-bit floats
            gradient_float = _mm256_sqrt_ps(gradient_float);

            // _mm256_cvtps_epi32: Converts 8 32-bit floats to 8 32-bit integers
            __m256i gradient_int = _mm256_cvtps_epi32(gradient_float);
            // _mm256_packus_epi16: Packs 8 32-bit integers into 16 16-bit integers with unsigned saturation
            __m256i gradient_byte = _mm256_packus_epi16(_mm256_packs_epi32(gradient_int, _mm256_setzero_si256()), _mm256_setzero_si256());
            // _mm256_permute4x64_epi64: Rearranges 4 64-bit integers in a 256-bit register
            gradient_byte = _mm256_permute4x64_epi64(gradient_byte, 0xD8);
            // _mm256_castsi256_si128: Casts upper 128 bits of a 256-bit integer to a 128-bit integer
            // _mm_storeu_si128: Stores 128 bits of integer data to unaligned memory location
            _mm_storeu_si128((__m128i*)&gradient[M * row + col], _mm256_castsi256_si128(gradient_byte));
        }

        // Handle edge cases for each row
        for (int col = 0; col < 1; ++col) {
            gradient[M * row + col] = 0;
        }
        for (int col = M - SIMD_WIDTH; col < M; ++col) {
            gradient[M * row + col] = 0;
        }
    }

    // Handle top and bottom rows
    for (int col = 0; col < M; ++col) {
        gradient[col] = 0; // top row: col = 0 to M because we're setting all columns in the first row to 0
        gradient[(N - 1) * M + col] = 0; // bottom row: calculated by N-1 * M + col because N-1 is the index of the last row, and we multiply by M to get to that row's start, then add col to move across the row
    }
}

void read_image(char* filename)
{

    int c;
    FILE* finput;
    int i, j, temp;

    //   printf("\nReading %s image from disk ...\n",filename);
    finput = NULL;
    openfile(filename, &finput);

    // Allocate dynamic arrays for images
    allocateArrays();

    if ((header[0] == 'P') && (header[1] == '2')) {
        for (j = 0; j < N; j++) {
            for (i = 0; i < M; i++) {

                if (fscanf(finput, "%d", &temp) == EOF)
                    exit(EXIT_FAILURE);

                frame1[M * j + i] = (unsigned char)temp;
            }
        }
    }
    else if ((header[0] == 'P') && (header[1] == '5')) {
        for (j = 0; j < N; j++) {
            for (i = 0; i < M; i++) {
                c = getc(finput);
                frame1[M * j + i] = (unsigned char)c;
            }
        }
    }
    else {
        printf("\n problem with reading image");
        exit(EXIT_FAILURE);
    }


    fclose(finput);
    printf("\nimage successfully read from disc\n");

}

void write_image2(char* filename, unsigned char* output_image)
{

    FILE* foutput;
    int i, j;

    printf("  Writing result to disk: %s\n", filename);

    if ((foutput = fopen(filename, "wb")) == NULL) {
        fprintf(stderr, "Unable to open file %s for writing\n", filename);
        exit(-1);
    }

    fprintf(foutput, "P2\n");
    fprintf(foutput, "%d %d\n", M, N);
    fprintf(foutput, "%d\n", 255);

    for (j = 0; j < N; ++j) {
        for (i = 0; i < M; ++i) {
            fprintf(foutput, "%3d ", output_image[M * j + i]);
            if (i % 32 == 31) fprintf(foutput, "\n");
        }
        if (M % 32 != 0) fprintf(foutput, "\n");
    }
    fclose(foutput);


}

void openfile(char* filename, FILE** finput)
{
    int x0, y0, x;

    //int aa;

    if ((*finput = fopen(filename, "rb")) == NULL) {
        fprintf(stderr, "Unable to open file %s for reading\n", filename);
        exit(-1);
    }

    if (fscanf(*finput, "%s", header) == EOF)
        exit(EXIT_FAILURE);

    x0 = getint(*finput);//this is M
    y0 = getint(*finput);//this is N
    //   printf("\t header is %s, while x=%d,y=%d\n",header,x0,y0);

     //CRITICAL POINT: AT THIS POINT YOU CAN ASSIGN x0,y0 to M,N 

    M = x0; // set M to size of image
    N = y0; // set N to size of image

    printf("\nImage dimensions are M=%i,N=%i\n", M, N);

    x = getint(*finput); /* read and throw away the range info */
    //printf("\n range info is %d",x);

}

void allocateArrays()
{
    /*
        This function creates an array, M*N = is size,
        add one incase array is not loaded from positoon 0,
        goes from 0-10, not 1-10
    */
    frame1 = new unsigned char[M * N + 1];
    filt = new unsigned char[M * N + 1];
    gradient = new unsigned char[M * N + 1];
}

void deleteArrays()
{
    // free memory
    delete[] frame1;
    delete[] filt;
    delete[] gradient;
}

//CRITICAL POINT: you can define your routines here that create the arrays dynamically; now, 
//the arrays are defined statically.



int getint(FILE* fp) /* adapted from "xv" source code */
{
    int c, i, firstchar;//, garbage;

    /* note:  if it sees a '#' character, all characters from there to end of
       line are appended to the comment string */

       /* skip forward to start of next number */
    c = getc(fp);
    while (1) {
        /* eat comments */
        if (c == '#') {
            /* if we're at a comment, read to end of line */
            char cmt[256], * sp;

            sp = cmt;  firstchar = 1;
            while (1) {
                c = getc(fp);
                if (firstchar && c == ' ') firstchar = 0;  /* lop off 1 sp after # */
                else {
                    if (c == '\n' || c == EOF) break;
                    if ((sp - cmt) < 250) *sp++ = c;
                }
            }
            *sp++ = '\n';
            *sp = '\0';
        }

        if (c == EOF) return 0;
        if (c >= '0' && c <= '9') break;   /* we've found what we were looking for */

        /* see if we are getting garbage (non-whitespace) */
       // if (c!=' ' && c!='\t' && c!='\r' && c!='\n' && c!=',')
        //  garbage=1;

        c = getc(fp);
    }

    /* we're at the start of a number, continue until we hit a non-number */
    i = 0;
    while (1) {
        i = (i * 10) + (c - '0');
        c = getc(fp);
        if (c == EOF) return i;
        if (c < '0' || c>'9') break;
    }
    return i;
}