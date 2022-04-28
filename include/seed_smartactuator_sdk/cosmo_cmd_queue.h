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
    }

   CosmoCmdReqType dequeue(){
        std::lock_guard<std::mutex> lock(mtx);
        if(cosmo_cmds.empty()){
            CosmoCmdReqType ret = {-1,-1,-1,-1,""};
            return ret;
        }

        auto ret = cosmo_cmds.front();
        cosmo_cmds.pop();
        return ret;
    }

private:
    std::queue<CosmoCmdReqType> cosmo_cmds;
    std::mutex mtx;
};

#endif
