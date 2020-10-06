// Michael Larabel.
// Written as a quick demo for the Phoronix Test Suite.

#include <iostream>
#include <math.h>

const long N_ITERATIONS = 87654321;

int main()
{
    double pi = 0;

    for (long int i = 1; i <= N_ITERATIONS; ++i) {
        pi += (double)pow(-1, i + 1) / (2 * i - 1);
    }
    pi *= 4;

    return 0;
}

