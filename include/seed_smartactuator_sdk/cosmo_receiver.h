#ifndef _COSMO_RECEIVER_H_
#define _COSMO_RECEIVER_H_

#include "seed_smartactuator_sdk/cosmo_cmd_queue.h"

struct MsRecvRaw {
    uint8_t header[2];
    uint8_t len;
    uint8_t cmd;
    uint8_t MSID;
    uint8_t data[57];
    uint8_t opt;
    uint8_t cs;
};

struct CosmoReceiver{

    CosmoReceiver(CosmoCmdQueue* tgt_queue):tgt_queue(tgt_queue){

    }

    bool operator()(MsRecvRaw *recvd){
        if(recvd->cmd != 0xa0){
            return false; //Cosmoでない場合
        }

        if (tgt_queue) {
            MSID = recvd->MSID;
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

    uint8_t getMSID(){
        return MSID;
    }

private:
    CosmoCmdQueue* tgt_queue;
    uint8_t MSID;
};

#endif
