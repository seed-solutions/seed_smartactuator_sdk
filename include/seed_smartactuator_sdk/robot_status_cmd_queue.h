#include <queue>
#include <mutex>
#include <iostream>

struct RobotStatusCmdReqType{
    int header_type = -1;
    int addr = -1;
    int cmd_type = -1;
    int msid = -1;
    uint8_t recvd_data[68] = {0};
};

struct RobotStatusCmdRespType{
    //今のところは何も使用しない
    int header_type = -1;
    int addr = -1;
    int cmd_type = -1;
    int msid = -1;
    uint8_t recvd_data[68] = {0};
};

class RobotStatusCmdQueue{
public:
   void enqueue(RobotStatusCmdReqType cmd) {
        std::lock_guard < std::mutex > lock(mtx);
        robot_status_cmd = cmd;
    }

   RobotStatusCmdReqType dequeue(){
       std::lock_guard<std::mutex> lock(mtx);
       if(robot_status_cmd.header_type == 0){
           RobotStatusCmdReqType ret;
           return ret;
       }
       auto ret = robot_status_cmd;
       return ret;
   }

private:
    RobotStatusCmdReqType robot_status_cmd;
    std::mutex mtx;
};
