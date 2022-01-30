#include "dataplane/bytearray.h"
#include <cassert>

int main() {
    {
        ByteArray arr;
        assert(arr.size() == 0);
        assert(arr.data() == nullptr);
    }
    {
        ByteArray arr(10);
        assert(arr.size() == 10);
        auto data = arr.data();
        assert(data != nullptr);
        ByteArray other(std::move(arr));
        assert(other.size() == 10);
        assert(other.data() == data);
        assert(arr.empty());
        assert(arr.data() == nullptr);
    }
    {
        ByteArray arr(10);
        ByteArray other(100);
        arr = std::move(other);
        assert(arr.size() == 100);
        assert(other.empty());
    }
    return 0;
}
