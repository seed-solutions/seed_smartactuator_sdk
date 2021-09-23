#ifndef _COSMO_RECEIVER_H_
#define _COSMO_RECEIVER_H_

#include "seed_smartactuator_sdk/cosmo_cmd_queue.h"
#include <iostream>

struct MsRecvRaw {
    uint8_t header[2];
    uint8_t address;
    uint8_t cmd;
    uint8_t msid;
    uint8_t data[63];
};

struct CosmoReceiver{

    CosmoReceiver(CosmoCmdQueue* tgt_queue):tgt_queue(tgt_queue){

    }

    bool operator()(const std::string &recvd_str){
    	 int header;
    	 int addr;
    	 int type;
        uint8_t recvd_raw[recvd_str.size()] = {0};
        if(recvd_str.size() != 64)
        {
            int cosmo_header_pos = recvd_str.find("feef");
            //not cosmocommand            
            if(cosmo_header_pos == std::string::npos)
            {
                return false;
            }
            else
            {
                //64byte
                std::cout << "irr cosmo command length !!! " << std::endl;
                recvd_str.substr(cosmo_header_pos, 64);
            }
            if(recvd_str.size() != 68) {
    		
    		std::cout << "not aero_command :" << recvd_str << std::endl;
    	    }
            
        }
        for(size_t i=0; i < recvd_str.size() ; ++i){
            recvd_raw[i] = static_cast<uint8_t>(recvd_str[i]);
        }
        MsRecvRaw* recvd = reinterpret_cast<MsRecvRaw*>(recvd_raw);

//ヘッダーデバッグ用
    	/*if(recvd_str.size() == 64) {
    		std::stringstream ss;
    		for (int i = 0; i < 2; i++)
    		{
    			ss << "0x" << std::hex << static_cast<unsigned>(recvd->header[i]) << ", ";
     		}
    		std::cout << "64byte command : " << ss.str() << std::endl;
    	}*/

        //headerの受信
        if(recvd->header[0] == 0xef){
        	header = 0;

        }else if(recvd->header[0] == 0xfe){ //EF FE形式で受け取ったら
        	header = 1;

        }else { //FE EF形式で受け取ったら

    		std::cout << "not aero_command :" << recvd_str << std::endl;
    		
        	return false; //Cosmoでない場合

        }

    	
        //送り先の設定
        addr = recvd->address;

        //コマンドのタイプ設定
        if(recvd->cmd == 0xA1) type = 1;
        else if(recvd->cmd == 0xA2) type = 2;

        //コマンド内容の受信
        if (tgt_queue) {
            int idx = 0;
            std::ostringstream convert;
            while (recvd->data[idx] != 0x00) {
                convert << (char) recvd->data[idx++];
            }

            std::string cmd_str = convert.str();
            std::cout << "##################DEBUG: " << cmd_str << std::endl;

            tgt_queue->enqueue(header,addr,type,recvd->msid,cmd_str);
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
