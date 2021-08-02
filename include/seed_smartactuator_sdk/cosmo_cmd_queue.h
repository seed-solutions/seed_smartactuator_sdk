#ifndef _COSMO_CMD_QUEUE_H_
#define _COSMO_CMD_QUEUE_H_

#include <queue>
#include <mutex>

class CosmoCmdQueue{
public:
    void enqueue(int header_type, int addr, int cmd_type, int msid, std::string cmd) {
        std::lock_guard < std::mutex > lock(mtx);
        cosmo_cmds.push(std::forward_as_tuple(header_type, addr, cmd_type, msid, cmd));
    }

    std::tuple<int,int,int,int,std::string> dequeue(){
        std::lock_guard<std::mutex> lock(mtx);
        if(cosmo_cmds.empty()){
            return std::forward_as_tuple(-1,-1,-1,-1,"");
        }

        auto ret = cosmo_cmds.front();
        cosmo_cmds.pop();
        return ret;
    }

private:
    std::queue<std::tuple<int,int,int,int,std::string>> cosmo_cmds;
    std::mutex mtx;
};

#endif
