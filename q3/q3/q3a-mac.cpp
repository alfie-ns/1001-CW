#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <limits>
#include <string>
#include <iostream>

// Function declarations
void Gaussian_Blur();
void Sobel();
void read_image(const char* filename);
void write_image2(const char* filename, unsigned char* output_image);
void openfile(const char* filename, FILE** finput);
int getint(FILE* fp);

// Image dimensions
#define M 512  // cols
#define N 512  // rows

// Global variables
unsigned char frame1[N * M]; // input image
unsigned char filt[N * M]; // output filtered image
unsigned char gradient[N * M]; // output image

const signed char Mask[5][5] = {
    {2,4,5,4,2},
    {4,9,12,9,4},
    {5,12,15,12,5},
    {4,9,12,9,4},
    {2,4,5,4,2}
};

const signed char GxMask[3][3] = {
    {-1,0,1},
    {-2,0,2},
    {-1,0,1}
};

const signed char GyMask[3][3] = {
    {-1,-2,-1},
    {0,0,0},
    {1,2,1}
};

char header[100];

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <input_image> <output_blur> <output_edge>" << std::endl;
        return 1;
    }

    char* input = argv[1];
    char* output_blur = argv[2];
    char* output_edge = argv[3];

    read_image(input); // read image from disc

    Gaussian_Blur(); // blur the image (reduce noise)
    Sobel(); // apply edge detection

    write_image2(output_blur, filt); // store output image to the disc
    write_image2(output_edge, gradient); // store output image to the disc

    return 0;
}

void Gaussian_Blur() {
    int row, col, rowOffset, colOffset;
    int newPixel;
    unsigned char pix;
    const unsigned short int size = 2;

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

void Sobel() {
    int row, col, rowOffset, colOffset;
    int Gx, Gy;

    for (row = 1; row < N - 1; row++) {
        for (col = 1; col < M - 1; col++) {
            Gx = 0;
            Gy = 0;

            for (rowOffset = -1; rowOffset <= 1; rowOffset++) {
                for (colOffset = -1; colOffset <= 1; colOffset++) {
                    Gx += filt[M * (row + rowOffset) + col + colOffset] * GxMask[rowOffset + 1][colOffset + 1];
                    Gy += filt[M * (row + rowOffset) + col + colOffset] * GyMask[rowOffset + 1][colOffset + 1];
                }
            }

            gradient[M * row + col] = (unsigned char)sqrt(Gx * Gx + Gy * Gy);
        }
    }
}

void read_image(const char* filename) {
    FILE* finput;
    int i, j, temp;

    std::cout << "\nReading " << filename << " image from disk ..." << std::endl;
    finput = NULL;
    openfile(filename, &finput);

    if ((header[0] == 'P') && (header[1] == '5')) { // if P5 image
        for (j = 0; j < N; j++) {
            for (i = 0; i < M; i++) {
                temp = getc(finput);
                frame1[M * j + i] = (unsigned char)temp;
            }
        }
    }
    else if ((header[0] == 'P') && (header[1] == '2')) { // if P2 image
        for (j = 0; j < N; j++) {
            for (i = 0; i < M; i++) {
                if (fscanf(finput, "%d", &temp) == EOF)
                    exit(EXIT_FAILURE);
                frame1[M * j + i] = (unsigned char)temp;
            }
        }
    }
    else {
        std::cerr << "\nProblem with reading the image" << std::endl;
        exit(EXIT_FAILURE);
    }

    fclose(finput);
    std::cout << "Image successfully read from disc" << std::endl;
}

void write_image2(const char* filename, unsigned char* output_image) {
    FILE* foutput;
    int i, j;

    std::cout << "Writing result to disk ..." << std::endl;

    foutput = fopen(filename, "wb");
    if (foutput == NULL) {
        std::cerr << "Unable to open file " << filename << " for writing" << std::endl;
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

void openfile(const char* filename, FILE** finput) {
    int x0, y0, x;

    *finput = fopen(filename, "rb");
    if (*finput == NULL) {
        std::cerr << "Unable to open file " << filename << " for reading" << std::endl;
        exit(-1);
    }

    fscanf(*finput, "%99s", header);

    x0 = getint(*finput); // this is M
    y0 = getint(*finput); // this is N
    std::cout << "Header is " << header << ", while x=" << x0 << ", y=" << y0 << std::endl;

    x = getint(*finput); // read and throw away the range info
}

int getint(FILE* fp) {
    int c, i, firstchar;

    c = getc(fp);
    while (1) {
        if (c == '#') {
            char cmt[256];
            int sp = 0;
            firstchar = 1;
            while (1) {
                c = getc(fp);
                if (firstchar && c == ' ') firstchar = 0;
                else {
                    if (c == '\n' || c == EOF) break;
                    if (sp < 250) cmt[sp++] = c;
                }
            }
            cmt[sp++] = '\n';
            cmt[sp] = '\0';
        }

        if (c == EOF) return 0;
        if (c >= '0' && c <= '9') break;

        c = getc(fp);
    }

    i = 0;
    while (1) {
        i = (i * 10) + (c - '0');
        c = getc(fp);
        if (c == EOF) return i;
        if (c < '0' || c > '9') break;
    }
    return i;
}