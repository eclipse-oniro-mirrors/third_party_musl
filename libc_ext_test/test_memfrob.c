#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *memfrob(void *s, size_t n);

static void test_basic() {
    char str[] = "hello";
    const char original[] = "hello";
    size_t len = strlen(str);

    // Apply memfrob and verify XOR
    memfrob(str, len);
    for (size_t i = 0; i < len; i++) {
        assert(str[i] == (original[i] ^ 0x2a));
    }

    // Revert and check original data
    memfrob(str, len);
    assert(strcmp(str, original) == 0);
}

static void test_empty_buffer() {
    char str[] = "test";
    void *result = memfrob(str, 0);
    assert(result == str);
    assert(strcmp(str, "test") == 0);
}

static void test_single_byte() {
    unsigned char c = 0xAB;
    memfrob(&c, 1);
    assert(c == (0xAB ^ 0x2a));
    memfrob(&c, 1);
    assert(c == 0xAB);
}

static void test_even_odd_lengths() {
    char even[] = "abcd";
    char odd[] = "abcde";
    size_t even_len = 4;
    size_t odd_len = 5;

    memfrob(even, even_len);
    memfrob(even, even_len);
    assert(memcmp(even, "abcd", even_len) == 0);

    memfrob(odd, odd_len);
    memfrob(odd, odd_len);
    assert(memcmp(odd, "abcde", odd_len) == 0);
}

static void test_alignment() {
    char buffer[64];
    char *unaligned = buffer + 3; // Force misalignment
    size_t len = 57; // Arbitrary non-aligned length
    memset(unaligned, 0x55, len);

    memfrob(unaligned, len);
    for (size_t i = 0; i < len; i++) {
        assert(unaligned[i] == (0x55 ^ 0x2a));
    }

    memfrob(unaligned, len);
    for (size_t i = 0; i < len; i++) {
        assert(unaligned[i] == 0x55);
    }
}

static void test_all_bytes() {
    unsigned char data[256];
    for (int i = 0; i < 256; i++) {
        data[i] = i;
    }

    memfrob(data, 256);
    for (int i = 0; i < 256; i++) {
        assert(data[i] == (i ^ 0x2a));
    }

    memfrob(data, 256);
    for (int i = 0; i < 256; i++) {
        assert(data[i] == i);
    }
}

static void test_large_buffer() {
    const size_t size = 1 << 20; // 1MB
    unsigned char *data = malloc(size);
    assert(data != NULL);
    memset(data, 0xAA, size);

    memfrob(data, size);
    for (size_t i = 0; i < size; i++) {
        assert(data[i] == (0xAA ^ 0x2a));
    }

    memfrob(data, size);
    for (size_t i = 0; i < size; i++) {
        assert(data[i] == 0xAA);
    }
    free(data);
}

int main() {
    test_basic();
    test_empty_buffer();
    test_single_byte();
    test_even_odd_lengths();
    test_alignment();
    test_all_bytes();
    test_large_buffer();
    printf("All tests passed!\n");
    return 0;
}