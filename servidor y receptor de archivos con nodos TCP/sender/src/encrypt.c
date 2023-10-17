#include <stdio.h>
#include <string.h>

#include "crypto.h"
#include "util.h"

int main(int argc, char* argv[]) {
    if (sodium_init() != 0) {
        fprintf(stderr, "%s: could not initialize cryptography\n", argv[0]);
        return 1;
    }

    if (argc != 4) {
        fprintf(stderr, "usage: %s key_file in_file out_file\n", argv[0]);
        return 1;
    }

    const char* key_file = argv[1];
    const char* in_file = argv[2];
    const char* out_file = argv[3];

    unsigned char key[crypto_secretbox_KEYBYTES];
    open_key(key_file, key);

    FILE* f_in;
    if ((f_in = fopen(in_file, "rb")) == NULL) {
        fprintf(stderr, "%s: could not open input file\n", argv[0]);
        return 1;
    }

    size_t input_len = file_size(f_in);
    unsigned char* input_data = malloc(input_len);

    read_file(f_in, input_data, input_len);
    fclose(f_in);

    struct encrypted_t enc;
    encrypt(key, &enc, input_data, input_len);

    FILE* f_out;
    if ((f_out = fopen(out_file, "wb")) == NULL) {
        fprintf(stderr, "%s: could not open output file\n", argv[0]);
        return 1;
    }

    write_file(f_out, enc.nonce, sizeof enc.nonce);
    write_file(f_out, enc.msg, enc.len);

    fclose(f_out);

    return 0;
}
