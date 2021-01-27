#ifndef _COSMO_CMD_QUEUE_H_
#define _COSMO_CMD_QUEUE_H_

#include <queue>
#include <mutex>

class CosmoCmdQueue{
public:
    void enqueue(int msid, std::string cmd) {
        std::lock_guard < std::mutex > lock(mtx);
        cosmo_cmds.push(std::make_pair(msid, cmd));
    }

    std::pair<int,std::string> dequeue(){
        std::lock_guard<std::mutex> lock(mtx);
        if(cosmo_cmds.empty()){
            return std::make_pair(-1,"");
        }

        auto ret = cosmo_cmds.front();
        cosmo_cmds.pop();
        return ret;
    }

private:
    std::queue<std::pair<int,std::string>> cosmo_cmds;
    std::mutex mtx;
};

#endif
