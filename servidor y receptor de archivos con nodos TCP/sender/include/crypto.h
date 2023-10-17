#ifndef CRYPTO_H
#define CRYPTO_H

#include <sodium.h>
#include <sodium/crypto_pwhash.h>
#include <sodium/crypto_secretbox.h>

void generate_key(unsigned char* out);
int open_key(const char* path, unsigned char* key);

struct encrypted_t {
    unsigned char nonce[crypto_secretbox_NONCEBYTES];
    unsigned char* msg;
    size_t len;
};

void encrypt(const unsigned char* key, struct encrypted_t* out,
             const unsigned char* msg, const size_t msg_len);
int decrypt(const unsigned char* key, unsigned char* out,
            const struct encrypted_t* in);

#endif
