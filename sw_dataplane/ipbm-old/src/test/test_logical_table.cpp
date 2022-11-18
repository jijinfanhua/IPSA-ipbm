#include "dataplane/table.h"
#include <cassert>
#include <memory>
#include <map>

void test_logical_table(const std::shared_ptr<LogicTable> &tb) {
    std::map<int, int> entries = {
            {1, 5},
            {2, 4},
            {3, 3},
            {4, 2},
            {5, 1},
    };
    int pos;
    // query
    for (auto &entry: entries) {
        int key = entry.first;
        pos = tb->match_exact((Buffer) &key);
        assert(pos == tb->end_pos());
    }
    // insert
    for (auto &entry: entries) {
        int key = entry.first;
        int val = entry.second;
        tb->insert((Buffer) &key, (Buffer) &val);
    }
    // query
    for (auto &entry: entries) {
        int key = entry.first;
        int val = entry.second;
        pos = tb->match_exact((Buffer) &key);
        assert(pos < tb->end_pos());
        Buffer buf = tb->get_entry(pos);
        assert(*(int *) buf == key);
        assert(*(int *) (buf + sizeof(int)) == val);
    }
    // erase
    for (auto &entry: entries) {
        int key = entry.first;
        tb->erase((Buffer) &key);
    }
    // query
    for (auto &entry: entries) {
        int key = entry.first;
        pos = tb->match_exact((Buffer) &key);
        assert(pos == tb->end_pos());
    }
}

int main() {
    auto sram = std::make_shared<LogicSram>(0, sizeof(int), sizeof(int), 1019);
    test_logical_table(sram);
    auto tcam = std::make_shared<LogicTcam>(0, sizeof(int), sizeof(int), 1024);
    test_logical_table(sram);
    return 0;
}
