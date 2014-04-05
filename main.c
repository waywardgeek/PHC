#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <getopt.h>

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
        "    -p password      -- provide a string as the password\n",
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

// Print a value out in hex - from Catena.
static void printHex(const char *message, uint8_t *x, uint32_t len) {
    puts(message);
    for(uint32_t i = 0; i < len; i++) {
        if(i != 0 && i % 8 == 0) {
            puts("");
        }
        printf("%02x ", x[i]);
    }
    printf("     %d (octets)\n\n", len);
}

// Decode a little-endian length len vector of (unsigned char) into a length
// len/4 vector of (uint32_t).  Assumes len is a multiple of 4.
static inline void decodeLittleEndian(uint32_t *dst, const uint8_t *src, uint32_t len) {
    const uint8_t *p = src;
    for(uint32_t i = 0; i < len / 4; i++) {
        dst[i] = ((uint32_t)(p[0]) + ((uint32_t)(p[1]) << 8) + ((uint32_t)(p[2]) << 16) +
            ((uint32_t)(p[3]) << 24));
        p += 4;
    }
}

// Encode a length len/4 vector of (uint32_t) into a length len vector of
// (unsigned char) in little-endian form.  Assumes len is a multiple of 4.
static inline void encodeLittleEndian(uint8_t *dst, const uint32_t *src, uint32_t len) {
    uint8_t *p = dst;
    for (uint32_t i = 0; i < len / 4; i++) {
        *p++ = *src;
        *p++ = *src >> 8;
        *p++ = *src >> 16;
        *p++ = *src++ >> 24;
    }
}

int main(int argc, char **argv) {
    exeName = argv[0];

    uint8_t out[32];
    char *password = (char *)"password";
    uint32_t passwordlen = 12;
    uint8_t *salt = (uint8_t *)"salt";
    uint32_t numCalls = 1;
    uint32_t outlen = 32;
    uint32_t saltlen = 4;
    bool outputDieharderText = false;
    bool outputDieharderBinary = false;

    char c;
    while((c = getopt(argc, argv, "d:Dp:")) != -1) {
        switch (c) {
        case 'd':
            outputDieharderText = true;
            numCalls = readUint32(c, optarg);
            break;
        case 'D':
            outputDieharderBinary = true;
            break;
        case 'p':
            password = optarg;
            passwordlen = strlen(password);
            break;
        default:
            usage("Invalid argument");
        }
    }
    if(optind + 2 != argc) {
        usage("Invalid number of arguments");
    }
    uint32_t m_cost = readUint32('\0', argv[optind]);
    uint32_t t_cost = readUint32('0', argv[optind+1]);
        
    if(outputDieharderText) {
        printf("type: d\n"
            "count: %u\n"
            "numbit: 32\n", numCalls*8);
    }

    if(outputDieharderText || outputDieharderBinary) {
        for(uint32_t i = 0; (i < numCalls) || outputDieharderBinary; i++) {
            uint8_t pwdbuf[passwordlen + 4];
            memcpy(pwdbuf, password, passwordlen);
            encodeLittleEndian(pwdbuf + passwordlen, &i, 4);
            if(PHS(out, outlen, pwdbuf, passwordlen + 4, salt, saltlen, t_cost, m_cost)) {
                printf("Password hashing for %s failed!\n", argv[0]);
                return 1;
            }
            if(outputDieharderText) {
                for(uint32_t j = 0; j < 8; j++) {
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
    } else {
        if(PHS(out, outlen, (uint8_t *)password, passwordlen, salt, saltlen, t_cost, m_cost)) {
            printf("Password hashing for %s failed!\n", argv[0]);
            return 1;
        }
        printHex("", out, outlen);
    }

    return 0;
}
