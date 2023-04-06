#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <omp.h>

// Function to generate prime numbers up to N
void genprimes(int N, int t) {
    bool *primes = (bool *) calloc(N+1, sizeof(bool)); // allocate an array of size N+1 and initialize it with false
    if (primes == NULL) {
        printf("Memory allocation error");
        return;
    }

    int limit = floor((N+1)/2);

    #pragma omp parallel for num_threads(t) schedule(dynamic)
    for (int i = 2; i <= limit; i ++) {
        if (primes[i] == false) { // false means prime!!!
            for (int j = i*i; j <= N; j += i) {
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
    for (int i = 3; i <= N; i += 2) { // Also, we escape all even numbers because they never prime
        if (primes[i] == false) {
            fprintf(outfile, "%d %d\n", numprimes++, i); // write the primes to the output file
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
    tstart = omp_get_wtime(); // measure the start time
    genprimes(N, t);
    ttaken = omp_get_wtime() - tstart; // measure the total time
    printf("Time taken for the main part: %f\n", ttaken);
    return 0;
}
