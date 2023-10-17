#include "crypto.h"

#include <sodium.h>
#include <sodium/crypto_secretbox.h>

void generate_key(unsigned char* out) {
    crypto_secretbox_keygen(out);
}

int open_key(const char* path, unsigned char* key) {
    FILE* fp;
    if ((fp = fopen(path, "rb")) == NULL) {
        return 1;
    }

    fread(key, crypto_box_SECRETKEYBYTES, 1, fp);
    fclose(fp);

    return 0;
}

void encrypt(const unsigned char* key, struct encrypted_t* out,
             const unsigned char* msg, const size_t msg_len) {
    randombytes_buf(out->nonce, sizeof out->nonce);

    out->msg = malloc(crypto_secretbox_MACBYTES + msg_len);
    out->len = crypto_secretbox_MACBYTES + msg_len;

    crypto_secretbox_easy(out->msg, (const unsigned char*)msg, msg_len,
                          out->nonce, key);
}

int decrypt(const unsigned char* key, unsigned char* out,
            const struct encrypted_t* in) {
    return crypto_secretbox_open_easy((unsigned char*)out, in->msg, in->len,
                                      in->nonce, key);
}
