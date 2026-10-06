#include "../config.h"
#include "scoring.h"
#include <stddef.h>

union absoluteValue
{
    double d;
    uint64_t u;
};

double ioc(LETTER *text, size_t length)
{
    int counts[26] = {0};
    int N = 0;

    // Count each letter
    for (size_t i = 0; i < length; i++)
    {
        LETTER c = text[i];
        if (c < 26)
        {
            counts[c]++;
            N++;
        }
    }

    // Calculate numerator: sum of n_i * (n_i - 1)
    long numerator = 0;

    for (int i = 0; i < 26; i++)
    {
        numerator += counts[i] * (counts[i] - 1);
    }

    // N * (N - 1)
    long denominator = N * (N - 1);

    union absoluteValue abs;
    abs.d = (double)numerator / denominator;
    abs.d -= 0.066;
    abs.u = (abs.u | 0x8000000000000000); // Set the sign bit to ensure a negative value
    return abs.d;
}