#ifndef _COSMO_CMD_QUEUE_H_
#define _COSMO_CMD_QUEUE_H_

#include <queue>
#include <mutex>
#include <iostream>

struct CosmoCmdReqType{
    int header_type;
    int addr;
    int cmd_type;
    int msid;
    std::string cmd_str;
};

struct CosmoCmdRespType{
    int header_type;
    int addr;
    int cmd_type;
    int msid;
    std::string cmd_str;
};

class CosmoCmdQueue{
public:
   void enqueue(CosmoCmdReqType cmd) {
        std::lock_guard < std::mutex > lock(mtx);
        cosmo_cmds.push(cmd);
        std::cout <<"enqueue -> cosmo size : " << cosmo_cmds.size() << std::endl;

        
    }

   CosmoCmdReqType dequeue(){
        std::lock_guard<std::mutex> lock(mtx);
        if(cosmo_cmds.empty()){
            std::cout << "dequeue -> cosmo cmd is empty" << std::endl;
            CosmoCmdReqType ret = {-1,-1,-1,-1,""};
            return ret;
        }

        std::cout <<"dequeue -> cosmo size : " << cosmo_cmds.size() << std::endl;
        auto ret = cosmo_cmds.front();
        cosmo_cmds.pop();
        return ret;
    }

private:
    std::queue<CosmoCmdReqType> cosmo_cmds;
    std::mutex mtx;
};

#endif
