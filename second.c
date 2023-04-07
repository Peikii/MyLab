#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <omp.h>

// Function to generate prime numbers up to N
void genprimes(int N, int t) {
    uint8_t *primes = (uint8_t *) calloc((N+1)/8 + 1, sizeof(uint8_t)); // allocate an array of bytes to hold the bit array
    if (primes == NULL) {
        printf("Memory allocation error");
        return;
    }

    int limit = floor((N+1)/2);

    double tstart, ttaken;
    tstart = omp_get_wtime(); // measure the start time

    #pragma omp parallel for num_threads(t) schedule(dynamic)
    for (int i = 2; i <= limit; i ++) {
        int byte_index = i / 8; // calculate the index of the byte containing the bit for i
        int bit_index = i % 8; // calculate the bit index within the byte for i
        if ((primes[byte_index] & (1 << bit_index)) == 0) { // check if the bit for i is not set
            for (int j = i*i; j <= N; j += i) {
                int byte_index_j = j / 8; // calculate the index of the byte containing the bit for j
                int bit_index_j = j % 8; // calculate the bit index within the byte for j
                #pragma omp critical
                {
                    primes[byte_index_j] |= (1 << bit_index_j); // set the bit for j
                }
            }
        }
    }
    ttaken = omp_get_wtime() - tstart; // measure the total time
    printf("Time taken for the main part: %f\n", ttaken);

    FILE *outfile;
    char filename[15];
    sprintf(filename, "%d.txt", N); // generate the output filename
    outfile = fopen(filename, "w");
    if (outfile == NULL) {
        printf("Error creating file %s", filename);
        return;
    }
    fprintf(outfile, "1, 2\n"); // 2 is always the first prime
    int numprimes = 2; // Starting with the second prime
    for (int i = 3; i <= N; i += 2) { // Also, we escape all even numbers because they never prime
        int byte_index = i / 8; // calculate the index of the byte containing the bit for i
        int bit_index = i % 8; // calculate the bit index within the byte for i
        if ((primes[byte_index] & (1 << bit_index)) == 0) { // check if the bit for i is not set
            fprintf(outfile, "%d, %d\n", numprimes++, i); // write the primes to the output file
        }
    }
    fclose(outfile);
    free(primes);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: ./genprime N t\n");
        return 0;
    }
    int N = atoi(argv[1]);
    int t = atoi(argv[2]);
    if (N < 2 || t <= 0 || t > 100) {
        printf("Invalid input\n");
        return 0;
    }
    double tstart, ttaken;
    genprimes(N, t);
    return 0;
}
