#include "seed_smartactuator_sdk/robot_status_cmd_queue.h"
#include <iostream>

struct MsRobotStatusRecvRaw
{
    uint8_t header[2];
    uint8_t address;
    uint8_t cmd;
    uint8_t msid;
    uint8_t data[63];
};

struct RobotStatusReceiver
{

    RobotStatusReceiver(RobotStatusCmdQueue *tgt_queue) : tgt_queue(tgt_queue)
    {
    }

    bool operator()(const std::string &recvd_str)
    {
        int header = 0;
        int addr;
        int type;
        uint8_t recvd_raw[recvd_str.size()] = {0};


        for (size_t i = 0; i < recvd_str.size(); ++i)
        {
            recvd_raw[i] = static_cast<uint8_t>(recvd_str[i]);
        }
        MsRobotStatusRecvRaw* recvd = reinterpret_cast<MsRobotStatusRecvRaw*>(recvd_raw);

        //headerの受信
        if (recvd->header[0] == 0xdf) {//DF FD形式で受け取ったら
            header = 1;
        } else if (recvd->header[0] == 0xfd) { //FD DF形式で受け取ったら
            header = 2;
        } else {
            return false;
        }

        //送り先の設定
        addr = recvd->address;

        //コマンドのタイプ設定
        if (recvd->cmd == 0x61){
            type = 61;
        }
        else{
            type = -1;
            return false;
        }

        //コマンド内容の受信
        if (tgt_queue && recvd_str.size() == 68)
        {
            RobotStatusCmdReqType req;
            req.header_type = header;
            req.addr = addr;
            req.cmd_type = type;
            req.msid = recvd->msid;
            for(int idx = 0; idx < recvd_str.size(); idx++){
                req.recvd_data[idx] = recvd_raw[idx];
            }
            tgt_queue->enqueue(req);
        }
        return true;
    }

private:
    RobotStatusCmdQueue *tgt_queue;
};
