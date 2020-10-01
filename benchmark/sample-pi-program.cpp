// Michael Larabel.
// Written as a quick demo for the Phoronix Test Suite.

#include <iostream>
#include <math.h>

int main()
{
    double pi = 0;

    for(long int i = 1; i <= 87654321; i++)
        pi += (double) pow(-1, i + 1) / (2 * i - 1);

    pi *= 4;
    //std::cout << "Done Calculating Pi...\n\n";
    return 0;
}
