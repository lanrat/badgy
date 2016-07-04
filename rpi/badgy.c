/*
 * Wiegand API Raspberry Pi
 * By Kyle Mallory
 * 12/01/2013
 * Based on previous code by Daniel Smith (www.pagemac.com) and Ben Kent (www.pidoorman.com)
 * Depends on the wiringPi library by Gordon Henterson: https://projects.drogon.net/raspberry-pi/wiringpi/
 *
 * The Wiegand interface has two data lines, DATA0 and DATA1.  These lines are normall held
 * high at 5V.  When a 0 is sent, DATA0 drops to 0V for a few us.  When a 1 is sent, DATA1 drops
 * to 0V for a few us. There are a few ms between the pulses.
 *   *************
 *   * IMPORTANT *
 *   *************
 *   The Raspberry Pi GPIO pins are 3.3V, NOT 5V. Please take appropriate precautions to bring the
 *   5V Data 0 and Data 1 voltges down. I used a 330 ohm resistor and 3V3 Zenner diode for each
 *   connection. FAILURE TO DO THIS WILL PROBABLY BLOW UP THE RASPBERRY PI!
 */
 // -lwiringPi
#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <time.h>
#include <unistd.h>
#include <memory.h>

#define D0_PIN 0 //11
#define D1_PIN 1 //12

#define WIEGANDMAXDATA 32
#define WIEGANDTIMEOUT 3000000
#define WIEGANDMINBITDELAY 1000000 //ns (2ms avg)

static unsigned char __wiegandData[WIEGANDMAXDATA];    // can capture upto 32 bytes of data -- FIXME: Make this dynamically allocated in init?
static unsigned long __wiegandBitCount;            // number of bits currently captured
static struct timespec __wiegandBitTime;        // timestamp of the last bit received (used for timeouts)


/* Subtract the ‘struct timespec values X and Y,
   storing the result in RESULT.
   Return 1 if the difference is negative, otherwise 0.
   From: http://www.gnu.org/software/libc/manual/html_node/Elapsed-Time.html
*/
int timespec_subtract(struct timespec* result, struct timespec* x, struct timespec* y)
{
    /* Perform the carry for the later subtraction by updating y. */
    if (x->tv_nsec < y->tv_nsec) {
        int nsec = (y->tv_nsec - x->tv_nsec) / 1000000000 + 1;
        y->tv_nsec -= 1000000000 * nsec;
        y->tv_sec += nsec;
    }
    if (x->tv_nsec - y->tv_nsec > 1000000000) {
        int nsec = (x->tv_nsec - y->tv_nsec) / 1000000000;
        y->tv_nsec += 1000000000 * nsec;
        y->tv_sec -= nsec;
    }

    /* Compute the time remaining to wait. tv_nsec is certainly positive. */
    result->tv_sec = x->tv_sec - y->tv_sec;
    result->tv_nsec = x->tv_nsec - y->tv_nsec;

    /* Return 1 if result is negative. */
    return x->tv_sec < y->tv_sec;
}

int tooSoon(void) {
    struct timespec now, delta;
    // check to not skip first bit after previous read/delay
    if (__wiegandBitCount == 0) {
        return 0;
    }
    // get delta
    clock_gettime(CLOCK_MONOTONIC, &now);
    timespec_subtract(&delta, &now, &__wiegandBitTime);
    //printf("delta %ld\n", delta.tv_nsec);
    // too soon if less that delay
    return (delta.tv_nsec < WIEGANDMINBITDELAY || delta.tv_sec > 1);
}

void data0Pulse(void) {
    //printf("0\n");
    if (!tooSoon()) {
        if (__wiegandBitCount / 8 < WIEGANDMAXDATA) {
            __wiegandData[__wiegandBitCount / 8] <<= 1;
            __wiegandBitCount++;
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &__wiegandBitTime);
}

void data1Pulse(void) {
    //printf("1\n");
    if (!tooSoon()) {
        if (__wiegandBitCount / 8 < WIEGANDMAXDATA) {
            __wiegandData[__wiegandBitCount / 8] <<= 1;
            __wiegandData[__wiegandBitCount / 8] |= 1;
            __wiegandBitCount++;
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &__wiegandBitTime);
}

void wiegandInit(int d0pin, int d1pin) {
    // Setup wiringPi
    wiringPiSetup() ;
    pinMode(d0pin, INPUT);
    pinMode(d1pin, INPUT);

    wiringPiISR(d0pin, INT_EDGE_FALLING, data0Pulse);
    wiringPiISR(d1pin, INT_EDGE_FALLING, data1Pulse);
}

void wiegandReset() {
    memset((void *)__wiegandData, 0, WIEGANDMAXDATA);
    __wiegandBitCount = 0;
}

int wiegandGetPendingBitCount() {
    struct timespec now, delta;
    clock_gettime(CLOCK_MONOTONIC, &now);
    delta.tv_sec = now.tv_sec - __wiegandBitTime.tv_sec;
    delta.tv_nsec = now.tv_nsec - __wiegandBitTime.tv_nsec;

    if ((delta.tv_sec > 1) || (delta.tv_nsec > WIEGANDTIMEOUT))
        return __wiegandBitCount;

    return 0;
}

/*
 * wiegandReadData is a simple, non-blocking method to retrieve the last code
 * processed by the API.
 * data : is a pointer to a block of memory where the decoded data will be stored.
 * dataMaxLen : is the maximum number of -bytes- that can be read and stored in data.
 * Result : returns the number of -bits- in the current message, 0 if there is no
 * data available to be read, or -1 if there was an error.
 * Notes : this function clears the read data when called. On subsequent calls,
 * without subsequent data, this will return 0.
 */
int wiegandReadData(void* data, int dataMaxLen) {
    if (wiegandGetPendingBitCount() > 0) {
        int bitCount = __wiegandBitCount;
        int byteCount = (__wiegandBitCount / 8) + 1;
        memcpy(data, (void *)__wiegandData, ((byteCount > dataMaxLen) ? dataMaxLen : byteCount));

        wiegandReset();
        return bitCount;
    }
    return 0;
}

void printCharAsBinary(unsigned char ch) {
    int i;
    for (i = 0; i < 8; i++) {
        printf("%d", (ch & 0x80) ? 1 : 0);
        ch <<= 1;
    }
}

int main(void) {
    int i;

    wiegandInit(D0_PIN, D1_PIN);

    while(1) {
        int bitLen = wiegandGetPendingBitCount();
        if (bitLen == 0) {
            usleep(5000);
        } else {
            char data[100];
            bitLen = wiegandReadData((void *)data, 100);
            int bytes = bitLen / 8 + 1;
            //printf("Read %d bits (%d bytes): ", bitLen, bytes);
            printf("Badge  ");
            for (i = 0; i < bytes; i++)
                printf("%02X ", (int)data[i]);
                //printf("%02X", (int)data[i]);
            /*printf(" : ");
            for (i = 0; i < bytes; i++)
                printCharAsBinary(data[i]);*/
            printf("\n");
        }
    }
    return 0;
}
