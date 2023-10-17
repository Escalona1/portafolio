#include <sodium.h>
#include <sodium/randombytes.h>

#include "crypto.h"
#include "util.h"

int main(int argc, char* argv[]) {
    if (sodium_init() != 0) {
        fprintf(stderr, "%s: could not initialize cryptography\n", argv[0]);
        return 1;
    }

    if (argc != 2) {
        fprintf(stderr, "usage: %s output\n", argv[0]);
        return 1;
    }

    unsigned char key[crypto_secretbox_KEYBYTES];
    generate_key(key);

    FILE* fp;
    if ((fp = fopen(argv[1], "wb")) == NULL) {
        fprintf(stderr, "%s: could not open output file", argv[0]);
        return 1;
    }

    write_file(fp, key, crypto_secretbox_KEYBYTES);
    fclose(fp);

    return 0;
}
