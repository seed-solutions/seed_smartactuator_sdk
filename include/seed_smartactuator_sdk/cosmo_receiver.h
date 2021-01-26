#ifndef _COSMO_RECEIVER_H_
#define _COSMO_RECEIVER_H_

#include "seed_smartactuator_sdk/cosmo_cmd_queue.h"

struct MsRecvRaw {
    uint8_t header[2];
    uint8_t len;
    uint8_t cmd;
    uint8_t opt;
    uint8_t data[63];
};

struct CosmoReceiver{

    CosmoReceiver(CosmoCmdQueue* tgt_queue):tgt_queue(tgt_queue){

    }

    bool operator()(MsRecvRaw *recvd){
        if(recvd->cmd != 0xa0){
            return false; //Cosmoでない場合
        }

        if (tgt_queue) {

            int idx = 0;
            std::ostringstream convert;
            while (recvd->data[idx] != '\0' && idx < sizeof(recvd->data)) {
                convert << (char) recvd->data[idx++];
            }

            std::string cmd_str = convert.str();
            tgt_queue->enqueue(cmd_str);
        }
        return true;
    }

private:
    CosmoCmdQueue* tgt_queue;
};

#endif
