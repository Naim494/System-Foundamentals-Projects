/*
 * DO NOT MODIFY THE CONTENTS OF THIS FILE.
 * IT WILL BE REPLACED DURING GRADING
 */
#include "myrand.h"

/*
 * This pseudorandom number generator was taken from the Ubuntu man page
 * for rand(3), where it is attributed to POSIX.1-2001.  We are using this
 * here for definiteness, because if we use the C library random number
 * generator, then we don't know that the same sequence of values will be
 * produced on all platforms.  It is not very convenient for our purposes
 * here, because it produces only a 15-bit random number on each call
 * (and we really want 32 bits), but it doesn't really matter that much:
 * we can just call it four times, keeping only the most-significant 8 bits
 * of the 15 obtained in each call.
 */

static unsigned long next = 1;

/* RAND_MAX assumed to be 32767 */
static int myrand(void) {
    next = next * 1103515245 + 12345;
    return((unsigned)(next/65536) % 32768);
}

/*
 * Sets the seed to be used as a starting point for the
 * pseudorandom number generator.
 */
void mysrand(unsigned int seed) {
    next = seed;
}

/*
 * Returns a random number from the full range of integers
 * representable in 32-bit, twos-complement encoding.
 */
int myrand32(void) {
    unsigned int r = 0;
    for(int i = 0; i < 4; i++) {
	r = (r << 8) | ((myrand() & 0x7fff) >> 7);
    }
    return r;
}
