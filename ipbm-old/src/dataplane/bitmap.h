#pragma once

#include "defs.h"
#include <cstring>
#include <cinttypes>

class Bitmap {
public:
    static constexpr int WIDTH = 8;
    static constexpr unsigned HIGHEST_BIT = 0x80;

    static void init(Buffer bm, int size) {
        memset(bm, 0, size);
    }

    static void set(Buffer bm, int pos) {
        bm[get_bucket(pos)] |= get_bit(pos);
    }

    static void reset(Buffer bm, int pos) {
        bm[get_bucket(pos)] &= (uint8_t) ~get_bit(pos);
    }

    static bool test(InputBuffer bm, int pos) {
        return (bm[get_bucket(pos)] & get_bit(pos)) != 0;
    }

    static int count_ones(InputBuffer bm, int size) {
        int cnt = 0;
        for (int i = 0; i < size; i++) {
            if (test(bm, i)) {
                cnt++;
            }
        }
        return cnt;
    }

private:
    static int get_bucket(int pos) {
        return pos / WIDTH;
    }

    static uint8_t get_bit(int pos) {
        return HIGHEST_BIT >> (uint8_t) (pos % WIDTH);
    }
};
