#include <queue>
#include <mutex>
#include <iostream>

//#define cmdstock

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

#ifdef cmdstock
        virtual_controller_cmds.push(cmd);
#else
        virtual_controller_cmd = cmd;
#endif
    }

   VirtualControllerCmdReqType dequeue(){
       std::lock_guard<std::mutex> lock(mtx);
#ifdef cmdstock
       if(virtual_controller_cmds.empty()){
           VirtualControllerCmdReqType ret;
           return ret;
       }
       auto ret = virtual_controller_cmds.front();
       virtual_controller_cmds.pop();
#else
       if(virtual_controller_cmd.header_type == 0){
           VirtualControllerCmdReqType ret;
           return ret;
       }
       auto ret = virtual_controller_cmd;
#endif
       return ret;
   }

private:
   VirtualControllerCmdReqType virtual_controller_cmd;
   std::queue<VirtualControllerCmdReqType> virtual_controller_cmds;
   std::mutex mtx;
};
