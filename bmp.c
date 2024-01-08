#include <stdio.h>
#include <stdlib.h>

void read_BMP(const char* filename,  char** data,  char header[54], int* width, int* height, int* row_padded, int * file_size) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }

    fread(header, sizeof( char), 54, file);
    *width = *(int*)&header[18];
    *height = *(int*)&header[22];
    *file_size = *(int*)&header[2];
    *row_padded = (*width * 3 /*width * 3 because of RGB */ + 3) & (~3); // bitwise AND between width*3+3 and 11111100 (3 = 00000011) to make sure that row_padded is a multiple of 4
    *data = (char*)malloc(*row_padded * *height); // row_padded * height because we need to read the whole image with rgb values and padding
    //read the rest of data from position 54 (header size) to the end of the file
    fread(*data, sizeof( char), *row_padded * *height, file);
    fclose(file);
}

void rotate_BMP( char* data, int width, int height, int row_padded,  char** rotated_data, int* new_row_padded) {
    //change dimensions
    int new_width = height; //change 
    int new_height = width;
    *new_row_padded = (new_width * 3 + 3) & (~3);
    *rotated_data = (char*)malloc(*new_row_padded * new_height);
    //rotate image
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int old_idx = (height - 1 - i) * row_padded + j * 3;
            int new_idx = j * *new_row_padded + (new_width - 1 - i) * 3;
            (*rotated_data)[new_idx] = data[old_idx];
            (*rotated_data)[new_idx + 1] = data[old_idx + 1];
            (*rotated_data)[new_idx + 2] = data[old_idx + 2];
        }
    }
}

void write_BMP(const char* filename, char* data,  char header[54], int width, int height, int row_padded, int file_size) {
    FILE *outFile = fopen(filename, "wb");
    if (outFile == NULL) {
        perror("Error opening file");
        exit(1);
    }

    *(int*)&header[18] = width; 
    *(int*)&header[22] = height; 
    *(int*)&header[2] = file_size; //file size is the same

    fwrite(header, sizeof(char), 54, outFile);
    fwrite(data, sizeof(char), row_padded * height, outFile);
    fclose(outFile);
}


void name_output_file(const char* inputFilename, char** outputFilename) {

    int inputLength = 0;
    while (inputFilename[inputLength] != '\0') { // '\0' is the null-terminator
        inputLength++;
    }
    const char prefix[] = "rotated_";
    int prefixLength = 0;
    while (prefix[prefixLength] != '\0') {
        prefixLength++;
    }
    int outputFilenameLength = inputLength + prefixLength + 1; // +1 for the null-terminator
    *outputFilename = (char*)malloc(outputFilenameLength * sizeof(char));
    if (*outputFilename == NULL) {
        perror("Memory allocation failed");
        exit(1);
    }
    //Copies the prefix, which is "rotated_", into the output filename
    int i; //is outside the for loop because we need it after for the input filename
    for (i = 0; i < prefixLength; i++) {
        (*outputFilename)[i] = prefix[i]; 
    }

    // Append the input filename to the output filename
    for (int j = 0; j < inputLength; j++) {
        (*outputFilename)[i + j] = inputFilename[j];
    }

    // Null-terminate the output filename
    (*outputFilename)[i + inputLength] = '\0';
}

int main() {
    const char* inputFilename = "sample.bmp";
    char* outputFilename;
    name_output_file(inputFilename, &outputFilename); // outputFilename = "rotated_sample.bmp"
    char *data, *rotated_data;
    char header[54];
    int width, height, row_padded, new_row_padded, file_size;

    read_BMP(inputFilename, &data, header, &width, &height, &row_padded,&file_size); //
    rotate_BMP(data, width, height, row_padded, &rotated_data, &new_row_padded);
    write_BMP(outputFilename, rotated_data, header, height, width, new_row_padded, file_size);

    free(data);
    free(rotated_data);

    return 0;
}
