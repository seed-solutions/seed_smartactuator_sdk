#ifndef _COSMO_RECEIVER_H_
#define _COSMO_RECEIVER_H_

#include "seed_smartactuator_sdk/cosmo_cmd_queue.h"
#include <iostream>

struct MsRecvRaw
{
    uint8_t header[2];
    uint8_t address;
    uint8_t cmd;
    uint8_t msid;
    uint8_t data[63];
};

struct CosmoReceiver
{

    CosmoReceiver(CosmoCmdQueue *tgt_queue) : tgt_queue(tgt_queue)
    {
    }

    bool operator()(const std::string &recvd_str)
    {
        int header;
        int addr;
        int type;
        uint8_t recvd_raw[recvd_str.size()] = {0};

        for (size_t i = 0; i < recvd_str.size(); ++i)
        {
            recvd_raw[i] = static_cast<uint8_t>(recvd_str[i]);
        }
        MsRecvRaw *recvd = nullptr;
        if (recvd_str.size() != 64)
        {
            std::stringstream ss;
            for (int i = 0; i < recvd_str.size(); i++)
            {
                ss << std::hex << static_cast<unsigned>(recvd_raw[i]);
            }


            int cosmo_header_pos = ss.str().find("fe");
            if (cosmo_header_pos == std::string::npos)
            {
                //aeroの場合
                //std::cout << "cosmoヘッダが見つかりません" << std::endl;
                return false;
            }
            else
            {
                //header位置によって分割位置を分ける
                std::cout << "データ長異常 " <<  recvd_str.size() << std::endl;
                uint8_t recvd_raw_re[64] = {0};
                std::stringstream ss;
                ss << std::hex << static_cast<unsigned>(recvd_raw[0]);
                std::cout << ss.str() << std::endl;
                if(ss.str() == "fe")
                {
                    std::cout << "cosmo header" << std::endl;
                }
                for (size_t i = 0; i < 64; ++i)
                {
                    recvd_raw_re[i] = recvd_raw[i];
                }
                recvd = reinterpret_cast<MsRecvRaw *>(recvd_raw_re);


            }
            /* fe,ef,4,a1,1,73,65,6e,61,5f,72,65,73,74,61,72,74,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4e, */
        }
        else
        {
            //cosmoの場合
            std::cout << "cosmoを受信しました" << std::endl;

            recvd = reinterpret_cast<MsRecvRaw *>(recvd_raw);
        }
        

        //headerの受信
        if (recvd->header[0] == 0xef)
        {
            header = 0;
        }
        else if (recvd->header[0] == 0xfe)
        { //EF FE形式で受け取ったら
            header = 1;
        }
        //ヘッダーデバッグ用
        /*if(recvd_str.size() == 64) {
    		std::stringstream ss;
    		for (int i = 0; i < 2; i++)
    		{
    			ss << "0x" << std::hex << static_cast<unsigned>(recvd->header[i]) << ", ";
     		}
    		std::cout << ss.str() << std::endl;
    	}*/

    	
        //送り先の設定
        addr = recvd->address;

        //コマンドのタイプ設定
        if (recvd->cmd == 0xA1)
            type = 1;
        else if (recvd->cmd == 0xA2)
            type = 2;

        //コマンド内容の受信
        if (tgt_queue)
        {
            int idx = 0;
            std::ostringstream convert;
            while (recvd->data[idx] != 0x00)
            {
                convert << (char)recvd->data[idx++];
            }

            std::string cmd_str = convert.str();
            std::cout << "##################DEBUG: " << cmd_str << std::endl;

            tgt_queue->enqueue(header, addr, type, recvd->msid, cmd_str);
        }
        return true;
    }

    uint8_t getMSID()
    {
        return MSID;
    }

private:
    CosmoCmdQueue *tgt_queue;
    uint8_t MSID;
};

#endif
