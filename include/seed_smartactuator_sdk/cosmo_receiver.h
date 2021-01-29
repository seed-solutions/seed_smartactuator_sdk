#ifndef _COSMO_RECEIVER_H_
#define _COSMO_RECEIVER_H_

#include "seed_smartactuator_sdk/cosmo_cmd_queue.h"
#include <iostream>

struct MsRecvRaw {
    uint8_t header[2];
    uint8_t len;
    uint8_t cmd;
    uint8_t msid;
    uint8_t data[63];
};

struct CosmoReceiver{

    CosmoReceiver(CosmoCmdQueue* tgt_queue):tgt_queue(tgt_queue){

    }

    bool operator()(std::string &recvd_str){

        uint8_t recvd_raw[recvd_str.size()] = {0};
        for(size_t i=0; i < recvd_str.size() ; ++i){
            recvd_raw[i] = static_cast<uint8_t>(recvd_str[i]);
        }
        MsRecvRaw* recvd = reinterpret_cast<MsRecvRaw*>(recvd_raw);
        if(recvd->cmd != 0xa0 && recvd->cmd != 0xa1){
            return false; //Cosmoでない場合
        }

        if (tgt_queue) {
            int idx = 0;
            std::ostringstream convert;
            while (idx < recvd->len - 5 && recvd->data[idx] != 0x00) {
                convert << (char) recvd->data[idx++];
            }

            recvd_str.erase (0,recvd->len+4);

            std::string cmd_str = convert.str();
            tgt_queue->enqueue(recvd->msid,cmd_str);
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
