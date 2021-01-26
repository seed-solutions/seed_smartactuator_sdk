#ifndef _COSMO_CMD_QUEUE_H_
#define _COSMO_CMD_QUEUE_H_

#include <queue>
#include <mutex>

class CosmoCmdQueue{
public:
    void enqueue(std::string cmd){
        std::lock_guard<std::mutex> lock(mtx);
        cosmo_cmds.push(cmd);
    }

    std::string dequeue(){
        std::lock_guard<std::mutex> lock(mtx);
        if(cosmo_cmds.empty()){
            return "";
        }

        auto ret = cosmo_cmds.front();
        cosmo_cmds.pop();
        return ret;
    }

private:
    std::queue<std::string> cosmo_cmds;
    std::mutex mtx;
};

#endif
