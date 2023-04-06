#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <omp.h>

// Function to generate prime numbers up to N
void genprimes(int N) {
    bool *primes = (bool *) calloc(N+1, sizeof(bool)); // allocate an array of size N+1 and initialize it with false
    if (primes == NULL) {
        printf("Memory allocation error");
        return;
    }
    int i, j = 0;

    #pragma omp parallel for private(i,j) schedule(dynamic)
    for (i = 2; i <= floor((N+1)/2); i += 1) {
        if (primes[i] == false) { // false means prime!!!
            for (j = i*i; j <= N; j += i) {
                #pragma omp critical
                {
                    primes[j] = true; // cross out the multiple of current prime number "i"
                }
            }
        }
    }

    FILE *outfile;
    char filename[15];
    sprintf(filename, "%d.txt", N); // generate the output filename
    outfile = fopen(filename, "w");
    if (outfile == NULL) {
        printf("Error creating file %s", filename);
        return;
    }
    fprintf(outfile, "1 2\n"); // 2 is always the first prime
    int numprimes = 2; // Starting with the second prime
    for (i = 3; i <= N; i += 2) { // Also, we escape all even numbers because they never prime
        if (primes[i] == false) {
            fprintf(outfile, "%d %d\n", numprimes++, i); // write the primes to the output file
        }
    }
    fclose(outfile);
    free(primes);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: ./genprime N\n");
        return 0;
    }
    int N = atoi(argv[1]);
    if (N < 2) {
        printf("Invalid input\n");
        return 0;
    }
    double tstart = 0.0, tend=0.0, ttaken;
    tstart = omp_get_wtime(); // measure the start time
    genprimes(N);
    ttaken = omp_get_wtime() - tstart; // measure the total time
    printf("Time taken for the main part: %f\n", ttaken);
    return 0;
}
