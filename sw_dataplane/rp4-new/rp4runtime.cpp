#include "rp4runtime.h"
#include "ipsa_output.h"
#include "DataPlaneController.h"
#include <sstream>

using namespace std;

Rp4Runtime runtime;

int main() {
    string line, temp;
    while (true) {
        cout << ">>> " << flush;
        getline(cin, line);
        vector<string> params;
        istringstream is(line);
        while (getline(is, temp, ' ')) {
            params.push_back(temp);
        }
        if (!params.size())
            continue;
        string cmd = params[0];
        if (cmd == "help") {
            cout << "help" << endl;
            cout << "compile <rp4 file> <port>" << endl;
            cout << "update <update file> <port>" << endl;
            cout << "insertEntry <entry file> <port>" << endl;
        } else if (cmd == "compile") {
            string rp4_filename = params[1];
            int port = atoi(params[2].c_str());
            runtime.emitNewTask(rp4_filename, port);
        } else if (cmd == "update") {
            string update_filename = params[1];
            int port = atoi(params[2].c_str());
            runtime.emitUpdate(update_filename, port);
        } else if (cmd == "insertEntry") {
            string entry_path = params[1];
            int port = atoi(params[2].c_str());
            insertEntry(port, entry_path);
        } else {
            cout << "error input!" << endl;
        }
    }
}