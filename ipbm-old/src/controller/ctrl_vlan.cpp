#include "api/api_client.h"

using namespace api;

int main() {
    RC rc;
    ModClient rp4_client(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));
    {
        FlowEntry src_flow;
        int iface = 0;
        uint32_t src_port = 0xffffffff;
        src_flow.key = std::string((char *) &iface, sizeof(int));
        src_flow.action_id = -1;
        src_flow.action_data = {std::string((char *) &src_port, sizeof(uint32_t))};

        FlowEntry dst_flow;
        uint32_t dst_port = (0x01 | 0x02);
        dst_flow.key = std::string((char *) &iface, sizeof(int));
        dst_flow.action_id = -1;
        dst_flow.action_data = {std::string((char *) &dst_port, sizeof(uint32_t))};

        rc = rp4_client.mod_flow_entry(TAB_ID_VLAN_GROUP, src_flow, dst_flow);
        if (rc) {
            std::cerr << "FAILED with return code " << rc << std::endl;
        }
    }
    {
        FlowEntry src_flow;
        int iface = 1;
        uint32_t src_port = 0xffffffff;
        src_flow.key = std::string((char *) &iface, sizeof(int));
        src_flow.action_id = -1;
        src_flow.action_data = {std::string((char *) &src_port, sizeof(uint32_t))};

        FlowEntry dst_flow;
        uint32_t dst_port = (0x01 | 0x02);
        dst_flow.key = std::string((char *) &iface, sizeof(int));
        dst_flow.action_id = -1;
        dst_flow.action_data = {std::string((char *) &dst_port, sizeof(uint32_t))};

        rp4_client.mod_flow_entry(TAB_ID_VLAN_GROUP, src_flow, dst_flow);
        if (rc) {
            std::cerr << "FAILED with return code " << rc << std::endl;
        }
    }
    {
        FlowEntry src_flow;
        int iface = 2;
        uint32_t src_port = 0xffffffff;
        src_flow.key = std::string((char *) &iface, sizeof(int));
        src_flow.action_id = -1;
        src_flow.action_data = {std::string((char *) &src_port, sizeof(uint32_t))};

        FlowEntry dst_flow;
        uint32_t dst_port = (0x04 | 0x08);
        dst_flow.key = std::string((char *) &iface, sizeof(int));
        dst_flow.action_id = -1;
        dst_flow.action_data = {std::string((char *) &dst_port, sizeof(uint32_t))};

        rp4_client.mod_flow_entry(TAB_ID_VLAN_GROUP, src_flow, dst_flow);
        if (rc) {
            std::cerr << "FAILED with return code " << rc << std::endl;
        }
    }
    {
        FlowEntry src_flow;
        int iface = 3;
        uint32_t src_port = 0xffffffff;
        src_flow.key = std::string((char *) &iface, sizeof(int));
        src_flow.action_id = -1;
        src_flow.action_data = {std::string((char *) &src_port, sizeof(uint32_t))};

        FlowEntry dst_flow;
        uint32_t dst_port = (0x04 | 0x08);
        dst_flow.key = std::string((char *) &iface, sizeof(int));
        dst_flow.action_id = -1;
        dst_flow.action_data = {std::string((char *) &dst_port, sizeof(uint32_t))};

        rp4_client.mod_flow_entry(TAB_ID_VLAN_GROUP, src_flow, dst_flow);
        if (rc) {
            std::cerr << "FAILED with return code " << rc << std::endl;
        }
    }
    return 0;
}
