#include "dataplane/bitmap.h"
#include <cassert>
#include <ctime>
#include <bitset>

constexpr int MAX_N = 4096;

void check_equal(uint8_t *bm, std::bitset<MAX_N> &mock) {
    for (int i = 0; i < MAX_N; i++) {
        assert(Bitmap::test(bm, i) == mock.test(i));
    }
}

int main() {
    srand((unsigned) time(nullptr));

    uint8_t bm[MAX_N / Bitmap::WIDTH];
    Bitmap::init(bm, MAX_N / Bitmap::WIDTH);
    std::bitset<MAX_N> mock;

    for (int round = 0; round < 10000; round++) {
        int choice = rand() % 2;
        int pos = rand() % MAX_N;
        if (choice == 0) {
            Bitmap::set(bm, pos);
            mock.set(pos);
        } else {
            Bitmap::reset(bm, pos);
            mock.reset(pos);
        }
        check_equal(bm, mock);
    }
    return 0;
}
