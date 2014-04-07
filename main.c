#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <getopt.h>
#include <errno.h>
#include <time.h>

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

EXTERNC int PHS(void *out, size_t outlen, const void *in, size_t inlen, const void *salt, size_t saltlen,
    unsigned int t_cost, unsigned int m_cost);

static char *exeName = NULL;

static void usage(const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    vfprintf(stderr, (char *)format, ap);
    va_end(ap);
    fprintf(stderr, "\nUsage: %s [OPTIONS] t_cost m_cost\n"
        "    -D               -- Output binary to stdout for use in dieharder testing, using:\n"
        "                        %s -twocats -d | dieharder -g 200 -a | tee diehard.out\n"
        "    -d num-calls     -- Output text to stdout for use in dieharder testing, using:\n"
        "                        %s -twocats -d > foo; dieharder -g 202 -f foo\n"
        "    -G               -- Generate test vectors for various t_cost/m_cost values\n"
        "    -p password      -- provide a string as the password\n"
        "    -T repeats       -- Output only run time for repeated call\n",
        exeName, exeName, exeName);
    exit(1);
}

static uint32_t readUint32(char flag, char *arg) {
    char *endPtr;
    char *p = arg;
    uint32_t value = strtol(p, &endPtr, 0);
    if(*p == '\0' || *endPtr != '\0') {
        if(flag != '\0') {
            usage("Invalid integer for parameter -%c: %s", flag, arg);
        }
        usage("Invalid integer: %u", arg);
    }
    return value;
}

/* Print a value out in hex - from Catena. */
static void printHex(const char *message, uint8_t *x, uint32_t len) {
    puts(message);
    uint32_t i;
    for(i = 0; i < len; i++) {
        if(i != 0 && i % 8 == 0) {
            puts("");
        }
        printf("%02x ", x[i]);
    }
    printf("     %d (octets)\n\n", len);
}

/* Decode a little-endian length len vector of (unsigned char) into a length
   len/4 vector of (uint32_t).  Assumes len is a multiple of 4. */
static void decodeLittleEndian(uint32_t *dst, const uint8_t *src, uint32_t len) {
    const uint8_t *p = src;
    uint32_t i;
    for(i = 0; i < len / 4; i++) {
        dst[i] = ((uint32_t)(p[0]) + ((uint32_t)(p[1]) << 8) + ((uint32_t)(p[2]) << 16) +
            ((uint32_t)(p[3]) << 24));
        p += 4;
    }
}

/* Encode a length len/4 vector of (uint32_t) into a length len vector of
   (unsigned char) in little-endian form.  Assumes len is a multiple of 4. */
static void encodeLittleEndian(uint8_t *dst, const uint32_t *src, uint32_t len) {
    uint8_t *p = dst;
    uint32_t i;
    for (i = 0; i < len / 4; i++) {
        *p++ = *src;
        *p++ = *src >> 8;
        *p++ = *src >> 16;
        *p++ = *src++ >> 24;
    }
}

static void time_ms(struct timespec *start, struct timespec *end, double *ms)
{
    double start_ms, end_ms;

    start_ms = start->tv_sec * 1000.0 + start->tv_nsec / (1000.0 * 1000);
    end_ms   = end->tv_sec * 1000.0 + end->tv_nsec / (1000.0 * 1000);

    *ms = end_ms - start_ms;
}

static int time_PHS(int repeat, void *out, size_t outlen, const void *in, size_t inlen,
                    const void *salt, size_t saltlen, unsigned int t_cost, unsigned int
                    m_cost, double *ms)
{
    int i, r;
    struct timespec start, end;
    char dout[outlen];

    r = PHS(dout, outlen, in, inlen, salt, saltlen, t_cost, m_cost);
    if (r)
        return r;

    if (clock_gettime(CLOCK_MONOTONIC, &start) < 0)
        return -EINVAL;

    for (i = 0; i < repeat; i++) {
        r = PHS(out, outlen, in, inlen, salt, saltlen, t_cost, m_cost);
        if (r)
            return r;
        if (memcmp(dout, out, outlen)) {
            printf("Output differs in iteration %d.\n", i);
            return -EINVAL;
        }
    }

    if (clock_gettime(CLOCK_MONOTONIC, &end) < 0)
        return -EINVAL;

    time_ms(&start, &end, ms);

    return 0;
}

// Print the hash value in hex.
static void printCompactHex(const char *message, uint8_t *data, uint8_t len) {
    printf("%s", message);
    for(uint32_t i = 0; i < len; i++) {
        printf("%02x", data[i]);
    }
}

// Print a test vector.
static bool printTest(uint8_t *password, uint32_t passwordSize, uint8_t *salt, uint32_t saltSize,
        uint32_t t_cost, uint32_t m_cost) {
    uint8_t hash[32];
    int r;
    double ms;
    if((r = time_PHS(1, hash, 32, password, passwordSize, salt, saltSize, t_cost, m_cost, &ms))) {
        printf("Password hashing for %s failed with code %d!\n", exeName, r);
        return 1;
    }
    printCompactHex("password:", password, passwordSize);
    printCompactHex(" salt:", salt, saltSize);
    printf(" t_cost:%u m_cost:%u ", t_cost, m_cost);
    printCompactHex("-> ", hash, 32);
    printf("\n");
    return ms < 0.1;
}

// Print test vectors.
static void printTestVectors(void) {
    // Verify password and salt from 0 to 255 for t_cost = 0 .. 7 and m_cost = 0 .. 7
    for(uint32_t t_cost = 0; t_cost < 8; t_cost++) {
        bool tooLong = false;
        for(uint32_t m_cost = 0; m_cost < 8 && !tooLong; m_cost++) {
            for(uint32_t i = 0; i < 256; i++) {
                uint8_t v = i;
                printTest(&v, 1, NULL, 0, t_cost, m_cost);
                printTest(NULL, 0, &v, 1, t_cost, m_cost);
                tooLong = !printTest(&v, 1, &v, 1, t_cost, m_cost);
                if(tooLong && m_cost == 0) {
                    return;
                }
            }
            if(tooLong) {
                break;
            }
        }
    }
}

int main(int argc, char **argv) {
    exeName = argv[0];

    uint8_t out[32];
    char *password = (char *)"password";
    uint32_t passwordlen = 12;
    uint8_t *salt = (uint8_t *)"salt\0\0\0\0\0\0\0\0\0\0\0\0";
    uint32_t numCalls = 1;
    uint32_t outlen = 32;
    uint32_t saltlen = 16;
    uint32_t repeat = 1;
    bool outputDieharderText = false;
    bool outputDieharderBinary = false;
    bool outputTime = false;
    int r;

    char c;
    while((c = getopt(argc, argv, "d:DGp:T:")) != -1) {
        switch (c) {
        case 'd':
            outputDieharderText = true;
            numCalls = readUint32(c, optarg);
            break;
        case 'D':
            outputDieharderBinary = true;
            break;
        case 'G':
            printTestVectors();
            return 0;
        case 'p':
            password = optarg;
            passwordlen = strlen(password);
            break;
        case 'T':
            outputTime = true;
            repeat = readUint32(c, optarg);
            break;
        default:
            usage("Invalid argument");
        }
    }

    if(optind + 2 != argc) {
        usage("Invalid number of arguments");
    }
    uint32_t t_cost = readUint32('0', argv[optind]);
    uint32_t m_cost = readUint32('\0', argv[optind+1]);
        
    if(outputDieharderText) {
        printf("type: d\n"
            "count: %u\n"
            "numbit: 32\n", numCalls*8);
    }

    if(outputDieharderText || outputDieharderBinary) {
        uint32_t i;
        for(i = 0; (i < numCalls) || outputDieharderBinary; i++) {
            uint8_t pwdbuf[passwordlen + 4];
            memcpy(pwdbuf, password, passwordlen);
            encodeLittleEndian(pwdbuf + passwordlen, &i, 4);
            if((r = PHS(out, outlen, pwdbuf, passwordlen + 4, salt, saltlen, t_cost, m_cost))) {
                printf("Password hashing for %s failed with code %d!\n", argv[0], r);
                return 1;
            }
            if(outputDieharderText) {
                uint32_t j;
                for(j = 0; j < 8; j++) {
                    uint32_t v;
                    decodeLittleEndian(&v, out + j*4, 4);
                    printf("%u\n", v);
                }
            } else if(outputDieharderBinary) {
                fwrite(out, outlen, 1, stdout);
            } else {
                printHex("", out, outlen);
            }
        }
    } else if (outputTime) {
        double ms;
        if((r = time_PHS(repeat, out, outlen, (uint8_t *)password, passwordlen, salt, saltlen, t_cost, m_cost, &ms))) {
            printf("Password hashing for %s failed with code %d!\n", argv[0], r);
            return 1;
        }
        printf("Runtime: %2.0f ms\n", ms);
        printHex("", out, outlen);
    } else {
        if((r = PHS(out, outlen, (uint8_t *)password, passwordlen, salt, saltlen, t_cost, m_cost))) {
            printf("Password hashing for %s failed with code %d!\n", argv[0], r);
            return 1;
        }
        printHex("", out, outlen);
    }

    return 0;
}
