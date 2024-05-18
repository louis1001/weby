#ifndef CORE_H
#define CORE_H
#include <errno.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define usize size_t
#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t

#define nullptr NULL

#define bool int
#define true 1
#define false 0

#define ASSERT(cond, msg) do { if (!cond) { printf("%s:%d %s\n", __FILE__, __LINE__, msg); exit(1); } } while(0)
#define TODO() ASSERT(false, "TODO")

#define loop while(1)

#define debugf(msg, ...) printf("DEBUG: " msg "\n", __VA_ARGS__)
#define debug(msg) printf("DEBUG: " msg "\n")

#define FAILURE_STATUS -1;

#define STRINGIFY(x) #x

void show_error(const char*, errno_t);
#endif

// Implentation
#ifdef WITH_IMPL
void show_error(const char *desc, errno_t error) {
    char output[100];

    strerror_r(error, output, 100);

    fprintf(stderr, "%s:\n\t%d: %s\n", desc, error, output);
}
#endif
