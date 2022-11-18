#include "rp4runtime.h"
#include "ipsa_output.h"

#include <sstream>
#include <dirent.h>
#include <cstring>
#include <chrono>
#include <thread>
using namespace std;

Rp4Runtime runtime;

void insertEntry(int port, std::string entry_path);

bool isDigital(string str) {
    for (int i = 0;i < str.size();i++) {
        if (str.at(i) == '-' && str.size() > 1)  // 有可能出现负数
            continue;
        if (str.at(i) > '9' || str.at(i) < '0')
            return false;
    }
    return true;
}

void FindFiles(string root) {

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(root.c_str())) != NULL) {

        while ((ent = readdir(dir)) != NULL) {
            if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, ".."))
                continue;
            string port, type;
            string *s = &port;
            for (char c: ent->d_name) {
                if (c == '\000')
                    break;
                if (c == '.')
                    s = &type;
                else
                    s->push_back(c);
            }
            if (type == "rp4" && isDigital(port)) {
                runtime.emitNewTask(root + ent->d_name, atoi(port.c_str()));
                remove((root + ent->d_name).c_str());
                closedir(dir);
                return;
            } else if (type == "update" && isDigital(port)) {
                runtime.emitUpdate(root + ent->d_name, atoi(port.c_str()));
                remove((root + ent->d_name).c_str());
                closedir(dir);
                return;
            } else if (type == "json" && isDigital(port)) {
                insertEntry(atoi(port.c_str()), root + ent->d_name);
                remove((root + ent->d_name).c_str());
                closedir(dir);
                return;
            }


        }
        closedir(dir);
        return;
    }
}
void InitializeController();
int main() {
    InitializeController();
    string root = "path";
    while (true) {
        FindFiles(root);
        std::this_thread::sleep_for(std::chrono::microseconds(1000));
    }
}