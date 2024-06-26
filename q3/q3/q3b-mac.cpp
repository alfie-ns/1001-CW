#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <filesystem>

void Gaussian_Blur();
void Sobel();
void read_image(char* filename);
void write_image2(char* filename, unsigned char* output_image);
void openfile(char* filename, FILE** finput);
int getint(FILE* fp);
void allocateArrays();
void deleteArrays();

int M = -1; //cols
int N = -1; //rows

unsigned char* frame1; //input image
unsigned char* filt; //output filtered image
unsigned char* gradient; //output image

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
    if (argc != 3) {
        std::cout << "Please pass in an input and output directory name" << std::endl;
        return 1;
    }

    std::string inputDirectoryPath = argv[1];
    std::string outputDirectoryPath = argv[2];
    std::cout << "Input Images Directory: " << inputDirectoryPath << std::endl;
    std::cout << "Output Images Directory: " << outputDirectoryPath << std::endl;

    for (const auto& entry : std::filesystem::directory_iterator(inputDirectoryPath)) {
        std::string filename = entry.path().filename();

        char* inFile = new char[inputDirectoryPath.length() + filename.length() + 2];
        strcpy(inFile, (inputDirectoryPath + "/" + filename).c_str());

        char* blurFile = new char[outputDirectoryPath.length() + filename.length() + 6];
        strcpy(blurFile, (outputDirectoryPath + "/blur_" + filename).c_str());

        char* edgeFile = new char[outputDirectoryPath.length() + filename.length() + 16];
        strcpy(edgeFile, (outputDirectoryPath + "/edge_detected_" + filename).c_str());

        printf("\nProcessing image: %s\n", inFile);

        read_image(inFile);

        Gaussian_Blur();
        Sobel();

        write_image2(blurFile, filt);
        write_image2(edgeFile, gradient);

        deleteArrays();

        delete[] inFile;
        delete[] blurFile;
        delete[] edgeFile;
    }

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

void read_image(char* filename) {
    FILE* finput;
    int i, j, temp;

    finput = NULL;
    openfile(filename, &finput);

    allocateArrays();

    if ((header[0] == 'P') && (header[1] == '2')) {
        for (j = 0; j < N; j++) {
            for (i = 0; i < M; i++) {
                if (fscanf(finput, "%d", &temp) == EOF)
                    exit(EXIT_FAILURE);
                frame1[M * j + i] = (unsigned char)temp;
            }
        }
    } else if ((header[0] == 'P') && (header[1] == '5')) {
        for (j = 0; j < N; j++) {
            for (i = 0; i < M; i++) {
                frame1[M * j + i] = (unsigned char)getc(finput);
            }
        }
    } else {
        printf("\n problem with reading image");
        exit(EXIT_FAILURE);
    }

    fclose(finput);
    printf("\nimage successfully read from disc\n");
}

void write_image2(char* filename, unsigned char* output_image) {
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

void openfile(char* filename, FILE** finput) {
    int x0, y0;

    if ((*finput = fopen(filename, "rb")) == NULL) {
        fprintf(stderr, "Unable to open file %s for reading\n", filename);
        exit(-1);
    }

    if (fscanf(*finput, "%s", header) == EOF)
        exit(EXIT_FAILURE);

    x0 = getint(*finput);
    y0 = getint(*finput);

    M = x0;
    N = y0;

    printf("\nImage dimensions are M=%i,N=%i\n", M, N);

    getint(*finput);
}

void allocateArrays() {
    frame1 = new unsigned char[M * N + 1];
    filt = new unsigned char[M * N + 1];
    gradient = new unsigned char[M * N + 1];
}

void deleteArrays() {
    delete[] frame1;
    delete[] filt;
    delete[] gradient;
}

int getint(FILE* fp) {
    int c, i, firstchar;
    c = getc(fp);
    while (1) {
        if (c == '#') {
            char cmt[256], * sp;
            sp = cmt;  firstchar = 1;
            while (1) {
                c = getc(fp);
                if (firstchar && c == ' ') firstchar = 0;
                else {
                    if (c == '\n' || c == EOF) break;
                    if ((sp - cmt) < 250) *sp++ = c;
                }
            }
            *sp++ = '\n';
            *sp = '\0';
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