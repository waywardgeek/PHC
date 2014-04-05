#ifndef CATFISH_H
#define CATFISH_H

int catfish(uint8_t *out, size_t outlen, const uint8_t *in, size_t inlen, const uint8_t *salt, size_t saltlen, unsigned int t_cost, unsigned int m_cost);

int PHS(void *out, size_t outlen, const void *in, size_t inlen, const void *salt, size_t saltlen,
    unsigned int t_cost, unsigned int m_cost);

#endif  // CATFISH_H
