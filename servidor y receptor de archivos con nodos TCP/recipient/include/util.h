#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>
#include <stdio.h>

size_t file_size(FILE *fp);
void read_file(FILE *fp, void *buf, size_t size);
void write_file(FILE *fp, void *buf, size_t size);

#endif  // UTIL_H
