#include "seed_smartactuator_sdk/virtual_controller_cmd_queue.h"
#include <iostream>

struct VirtualControllerRecvRaw
{
    uint8_t header[2];
    uint8_t address;
    uint8_t cmd;
    uint8_t msid;
    uint8_t data[6];
};

struct VirtualControllerReceiver
{

    VirtualControllerReceiver(VirtualControllerCmdQueue *tgt_queue) : tgt_queue(tgt_queue)
    {
    }

    bool operator()(const std::string &recvd_str)
    {
        int header = 0;
        int addr;
        int type;
        int length = 7;
        int header_size = 4;
        uint8_t recvd_raw[recvd_str.size()] = {0};


        for (size_t i = 0; i < recvd_str.size(); ++i)
        {
            recvd_raw[i] = static_cast<uint8_t>(recvd_str[i]);
        }
        VirtualControllerRecvRaw* recvd = reinterpret_cast<VirtualControllerRecvRaw*>(recvd_raw);
        //headerの受信
        if (recvd->header[0] == 0xFB) {//FB BF形式で受け取ったら
            header = 1;
        } else if (recvd->header[0] == 0xBF) { //BF FB形式で受け取ったら
            header = 2;
        } else {
            return false;
        }

        //送り先の設定
        addr = recvd->address;
        //コマンドのタイプ設定
        if (recvd->cmd == 0x00){
            type = 1;
        }
        else{
            type = -1;
            return false;
        }

        //コマンド内容の受信
        if (tgt_queue && recvd_str.size() == 11)
        {
            VirtualControllerCmdReqType req;
            req.header_type = header;
            req.length = length;
            req.cmd_type = type;
            req.msid = recvd->msid;
            req.recvd_data.resize(11);
            req.recvd_data[0] = recvd_raw[0];
            req.recvd_data[1] = recvd_raw[1];
            req.recvd_data[2] = length;
            req.recvd_data[3] = recvd_raw[3];
            for(size_t i=0;i<length;++i) req.recvd_data[i+header_size] = recvd_raw[i+header_size];
            tgt_queue->enqueue(req);
        }
        return true;
    }

private:
    VirtualControllerCmdQueue *tgt_queue;
};
