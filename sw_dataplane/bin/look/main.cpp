#include <iostream>
#include "sw-src/dataplane/pipeline.h"

PHV * init_phv(uint8_t packet[], uint32_t recv_len, int if_index, Pipeline * pipe) {

    PHV *phv = new PHV();
    memcpy(phv->packet, packet, recv_len * sizeof(uint8_t));
    memset(phv->packet + FRONT_HEADER_LEN, 0, sizeof(uint8_t) * META_LEN);
    phv->cur_trans_fd_num = 1;
    phv->cur_state = 0;
    phv->cur_trans_key = 0xffff;

    phv->ig_if = if_index;

    int base_offset = FRONT_HEADER_LEN * 8;

    for(auto meta : pipe->get_metadata_header()) {
        phv->parsed_headers[meta->hdr_id]->hdr_id = meta->hdr_id;
        phv->parsed_headers[meta->hdr_id]->hdr_len = meta->hdr_len * 8;
        phv->parsed_headers[meta->hdr_id]->hdr_offset = meta->hdr_offset * 8 + FRONT_HEADER_LEN * 8;
        LOG(INFO) << "MataData: ";
        LOG(INFO) << "  header_id: " << phv->parsed_headers[meta->hdr_id]->hdr_id;
        LOG(INFO) << "  header_len: " << phv->parsed_headers[meta->hdr_id]->hdr_len;
        LOG(INFO) << "  header_offset: " << phv->parsed_headers[meta->hdr_id]->hdr_offset;
    }

    FieldInfo ig_if_fd{31, 0, 9};
    Data data = {9, get_uints(if_index, 9)};
    modify_phv_with_data(ig_if_fd, data, phv);
    return phv;
}




int main(int argc, char** argv) {
    google::InitGoogleLogging((const char *)argv[0]);
    LOG(INFO) << "Hello World" << std::endl;
    return 0;
}
