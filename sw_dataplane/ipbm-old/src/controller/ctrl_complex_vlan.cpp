#include "ctrl_complex.h"

int main(int argc, char **argv) {
    google::InitGoogleLogging(argv[0]);
    CtrlFwd ctrl(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));
    ctrl.add_vlan();
    return 0;
}
