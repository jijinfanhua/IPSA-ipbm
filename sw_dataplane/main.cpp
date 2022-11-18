/** @summary: This is a data plane simulator, which simulates our data plane template.
 *  @author:  Walker Dong 2022/3/1 Yong Feng 2022/1/22
 *
 *  This is not only our hardware simulator to show our current research,
 *  but also a great lab to test, develop a network system using our runtime programmable switch.
 *
 */


#include <iostream>
#include <gflags/gflags.h>
#include <glog/logging.h>
#include <unistd.h>
#include <thread>

#include "sw-src/dataplane/pipeline.h"
#include "sw-src/dataplane/physical_layer.h"
#include "sw-src/dataplane/Global/global.h"

#include "sw-src/api/DataPlaneServer.h"
#define DEBUG

#include <string.h>
#include <chrono>



/**
 * Config means whether have been configured or not.
 * It doesn't work for any time until the initial configuration downloads.
 */
bool config = false;

/**
 * For presentation, we use grpc port to represent this switch's id.
 */
int grpc_port;



/**
 * Software data plane simulator consists of two parts of function module, grpc server & pipeline.
 * Grpc server gets the commands downloaded from the controller, and change the status of pipeline.
 * Pipeline is the key component about the data plane simulator.
 */


/**
 * Use the protobuf to define our grpc protocol,
 * and launch the data plane rpc server to wait the controller's commands.
 * @param pipe
 * @param grpc_port
 */
void ServerLaunch(Pipeline* pipe, int grpc_port) {
    api::DataPlaneServer server(pipe, grpc_port);
    api::RunServer(server);
}


/**
 * Pipeline function.
 * This simulates sending packets in physical layer.
 * In this simulator, we use id starts from 1 to identify the ingress & egress.
 * However, the real ingress & egress id starts from 0 at class PhysicalLayer.
 * @param phy  physical layer encapsulation.
 * @param phv  packet and other information.
 * @param i    egress port.
 */
void Output(PhysicalLayer& phy, PHV* phv, int i) {
    int len = phv->recv_len;
#ifdef DEBUG
    LOG(INFO) << "===== OUT PACKET =======";
    LOG(INFO) << "Emit via " << Global::glb.if_names[i-1];
    LOG(INFO) << "Egress interface index: " << i;
    LOG(INFO) << "Tx length: " << len;
    byte buffer[BUFSIZ];
    for (int j = 0; j < len; j++) buffer[j] = rvs[(byte) phv->packet[j]];
    BitArray(buffer, len * 8).Print(16);
#endif
    i--;
    phy.send_packet(i, buffer, len);
#ifdef DEBUG
    LOG(INFO) << "send ok!";
    LOG(INFO) << "========================";
#endif
}

// this function is just for presentations' convenience, to show our 3 use cases.
void PrintRoute(int e) {
    printf("\033[3%dm", grpc_port % 10);
    printf("%d -> ", grpc_port % 10);
    printf("\033[0m");
    if (
            ((e == 1 || e == 2)           && (grpc_port == 50011 || grpc_port == 50017)) || // uc1
            ((e == 1 || e == 2 || e == 3) && (grpc_port == 50021                      )) ||
            ((e == 1)                     && (grpc_port == 50022 || grpc_port == 50023)) || // uc2
            ((e == 1)                     && (grpc_port == 50033)                      )    // uc3

            ) {
        printf("\n");
    } else {
        fflush(stdout);
    }
}

/**
 * Launch data plane, Main Logic.
 * @param pipe
 */
void DataPlaneLaunch(Pipeline* pipe) {
    PhysicalLayer phy;
    FieldInfo outPort = FieldInfo(31, 9, 9);
    FieldInfo groupMark = FieldInfo(30, 7, 32);
    FieldInfo isGroup = FieldInfo(30, 0, 7);
    FieldInfo dropField = FieldInfo(31, 91, 1);
    while (true) {

        byte packet[BUFSIZ];
        memset(packet, 0, BUFSIZ);
        int ingress_interface;
        uint32_t recv_len = phy.recv_packet(packet, -1, ingress_interface);
        ingress_interface++;
        if (!config) continue;
#ifdef DEBUG
        LOG(INFO) << " ";
        LOG(INFO) << "===== IN PACKET ======";
        LOG(INFO) << "Ingress interface index: " << ingress_interface;
        LOG(INFO) << "Rx length: " << recv_len;
        LOG(INFO) << "======================";
#endif

        auto phv_instance = PHV();
        auto phv = &phv_instance;
        BitArray(packet, recv_len * 8).Print(16);
        phv->Initialize(packet, recv_len, pipe->GetMetadataHeader(), ingress_interface);
        pipe->Execute(phv);


        if (dropField.ToBitArray(phv).Integer() == 1 || outPort.ToBitArray(phv).Integer() == 0) {
            LOG(INFO) << "Drop Packet";
        } else if (isGroup.ToBitArray(phv).Integer()) {
            LOG(INFO) << "Multicast. ";
            BitArray group = groupMark.ToBitArray(phv);
            group.Print();
            for (int i = 0; i < Global::glb.num_if; i++) {
                if (i == ingress_interface) continue;
                else if (group.GetBit(i)) {
                    Output(phy, phv, i);
                }
            }
        } else {
            uint32_t egress_interface = outPort.ToBitArray(phv).Integer();
            LOG(INFO) << "egress interface port: " << egress_interface;

            try {
                // broadcast.
                if (egress_interface == 0x1ff) {
                    for (int i = 0; i < Global::glb.num_if; i++) {
                        Output(phy, phv, i);
                    }
                } else {
                    // normal.
                    Output(phy, phv, egress_interface);
                    PrintRoute(egress_interface);
                }
            }
            catch (const std::runtime_error &e) {
                LOG(ERROR) << e.what();
            }
        }
    }
}


int main(int argc, char *argv[]) {

    google::InitGoogleLogging("/dev/null");
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    std::string path   = argv[1];
    grpc_port = atoi(argv[2]);
    Global::NetConfigInitialize(path);

    auto pipe = new Pipeline(PROC_NUM);

    std::thread server = std::thread(ServerLaunch, pipe, grpc_port);
    std::thread data_plane = std::thread(DataPlaneLaunch, pipe);

    data_plane.join();
    server.join();

    return 0;
}
