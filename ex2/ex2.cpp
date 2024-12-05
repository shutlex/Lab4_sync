#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <ctime>
#include <iostream>


void CopyFileWithC(const char* inputFile, const char* outputFile) {
    FILE* inFile = fopen(inputFile, "rb");
    FILE* outFile = fopen(outputFile, "wb");

    if (!inFile || !outFile) {
        std::cerr << "Failed to open file.\n";
        if (inFile) fclose(inFile);
        if (outFile) fclose(outFile);
        return;
    }

    const size_t bufferSize = 1024 * 1024; // 1 MB
    char* buffer = new char[bufferSize];

    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, bufferSize, inFile)) > 0) {
        fwrite(buffer, 1, bytesRead, outFile);
    }

    delete[] buffer;
    fclose(inFile);
    fclose(outFile);
}

int main() {
    const char* inputFile = "largefile.bin";
    const char* outputFile = "copy_c.bin";

    clock_t start = clock();
    CopyFileWithC(inputFile, outputFile);
    clock_t end = clock();

    std::cout << "C Standard Library Time: "
        << double(end - start) / CLOCKS_PER_SEC
        << " seconds\n";

    return 0;
}




