#include <queue>
#include <mutex>
#include <iostream>

struct VirtualControllerCmdReqType{
    int header_type = -1;
    int length = -1;
    int cmd_type = -1;
    int msid = -1;
    std::vector<uint8_t> recvd_data;
};


class VirtualControllerCmdQueue{
public:
   void enqueue(VirtualControllerCmdReqType cmd) {
        std::lock_guard < std::mutex > lock(mtx);
        virtual_controller_cmds.push(cmd);
    }

   VirtualControllerCmdReqType dequeue(){
       std::lock_guard<std::mutex> lock(mtx);
       if(virtual_controller_cmds.empty()){
           VirtualControllerCmdReqType ret;
           return ret;
       }
       auto ret = virtual_controller_cmds.front();
       virtual_controller_cmds.pop();
       return ret;
   }

private:
   std::queue<VirtualControllerCmdReqType> virtual_controller_cmds;
   std::mutex mtx;
};
