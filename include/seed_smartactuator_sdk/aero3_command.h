#ifndef AERO_COMMAND_H_
#define AERO_COMMAND_H_

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/range/iterator_range.hpp>
#include <vector>
#include <unordered_map>
#include <thread>
#include "seed_smartactuator_sdk/cosmo_receiver.h"
#include "seed_smartactuator_sdk/robot_status_receiver.h"

#define IREX_2022

using namespace boost::asio;

namespace aero
{
  namespace controller
  {

  class AeroBuff{
  public:
      void set(const std::string &recvd){
          std::lock_guard<std::mutex> lk(mtx);
          recvd_queue.push(recvd);
      }

      std::string get(){
          std::lock_guard<std::mutex> lk(mtx);
          std::string ret;
          if(!recvd_queue.empty()){
              ret = recvd_queue.front();
              recvd_queue.pop();
          }
          return ret;
      }

  private:
      std::queue<std::string> recvd_queue;
      std::string recvd;
      std::mutex mtx;
  };


    class SerialCommunication
    {
    public:
      SerialCommunication();
      ~SerialCommunication();

      bool openPort(std::string _port, unsigned int _baud_rate);
      void closePort();
      void writeAsync(std::vector<uint8_t>& _send_data);
      void onReceive(const boost::system::error_code& _error, size_t _bytes_transferred);
      void onTimer(const boost::system::error_code& _error);
      void readBufferAsync();
      void readBuffer(std::vector<uint8_t>& _receive_data,const std::vector<uint8_t> &header, uint8_t _size);
      void flushPort();

      bool comm_err_;
      bool is_move_ = false;
      std::vector<uint8_t> cosmo_cmd_;//バーチャルコントローラ用移動CosmCMD
      std::vector<uint8_t> move_cmd_;//バーチャルコントローラ用移動CosmCMD
      CosmoCmdQueue cosmo_cmd_queue;
      RobotStatusCmdQueue robot_status_cmd_queue;
      AeroBuff receive_buff;
      const int at_least_size = 8;

    private:
      std::thread io_thread;
      io_service io_;
      serial_port serial_;
      deadline_timer timer_;
      CosmoReceiver cosmo_receiver_;
      RobotStatusReceiver robot_status_receiver_;
      bool is_canceled_;
      boost::asio::streambuf stream_buffer_;
      std::string port;


    };

    class AeroCommand{
    public:
      AeroCommand();
      ~AeroCommand();

      bool is_open_,comm_err_;
      bool is_move_ = false;
      std::vector<uint8_t> cosmo_cmd_;//バーチャルコントローラ用移動CosmCMD
      std::vector<uint8_t> move_cmd_;//バーチャルコントローラ用移動CosmCMD

      bool openPort(std::string _port, unsigned int _baud_rate);
      void closePort();
      void flushPort();

      void setCurrent(uint8_t _number,uint8_t _max, uint8_t _down);
      void onServo(uint8_t _number,uint16_t _data);
      std::vector<int16_t> getPosition(uint8_t _number);
      std::vector<uint16_t> getCurrent(uint8_t _number);
      std::vector<uint16_t> getTemperatureVoltage(uint8_t _number);
      std::string getVersion(uint8_t _number);
      std::vector<uint16_t> getStatus(uint8_t _number);
      void throughCAN(uint8_t _send_no,uint8_t _command,
        uint8_t _data1, uint8_t _data2, uint8_t _data3, uint8_t _data4, uint8_t _data5);
      std::vector<int16_t> actuateByPosition(uint16_t _time, int16_t *_data);
      std::vector<int16_t> actuateBySpeed(int16_t *_data);
      void runScript(uint8_t _number,uint16_t _data);
      void setControllerCmd();

      void resetting();
      void write_1byte(uint16_t _address, uint8_t *_write_data, int write_size);
      void write_2byte(uint16_t _address, uint16_t *_write_data);
      std::vector<uint8_t> read_1byte(uint16_t _address, int size);
      std::vector<uint8_t> read_2byte(uint16_t _address);

      CosmoCmdReqType getCosmoCmd(){
          return serial_com_.cosmo_cmd_queue.dequeue();
      }

    void sendCosmoCmdResp(CosmoCmdRespType resp) {
        check_sum_ = 0;
        length_ = 64;

        send_data_.resize(length_);
        fill(send_data_.begin(), send_data_.end(), 0);
        if (resp.header_type == 0) { //EF EFコマンドに対しての返信
            send_data_[0] = 0xEF;
            send_data_[1] = 0xFE;
        } else if (resp.header_type == 1) { //FE EFコマンドに対しての返信
            send_data_[0] = 0xFE;
            send_data_[1] = 0xEF;
        }
        switch (resp.addr) {
        case 1:
            send_data_[2] = 0x01;
            break;
        case 2:
            send_data_[2] = 0x02;
            break;
        case 4:
            send_data_[2] = 0x04;
            break;
        case 8:
            send_data_[2] = 0x08;
            break;
        case 16:
            send_data_[2] = 0x10;
            break;
        case 32:
            send_data_[2] = 0x20;
            break;
        case 64:
            send_data_[2] = 0x40;
            break;
        default:
            send_data_[2] = resp.addr;
            break;
        }
        if (resp.cmd_type == 1)
            send_data_[3] = 0xA1;
        if (resp.cmd_type == 2)
            send_data_[3] = 0xA2;
        send_data_[4] = resp.msid;
        strcpy((char*) &send_data_[5], resp.cmd_str.c_str());
        std::stringstream ss;
        for (int i = 0; i < send_data_.size(); i++) {
            ss << "0x" << std::hex << static_cast<unsigned>(send_data_[i]) << ", ";
        }
        std::cout << "send cosmo data: " << ss.str() << std::endl;

        //CheckSum
        for (count_ = 2; count_ < length_ - 1; count_++)
            check_sum_ += send_data_[count_];
        send_data_[length_ - 1] = ~check_sum_;

        serial_com_.flushPort();
        serial_com_.writeAsync(send_data_);
    }

    RobotStatusCmdReqType getRobotStatusCmd(){
        return serial_com_.robot_status_cmd_queue.dequeue();
    }

    void sendRobotStatusCmdResp(RobotStatusCmdRespType resp) {
#ifndef IREX_2022
        check_sum_ = 0;
        length_ = 68;

        send_robot_status_data_.resize(length_);
        fill(send_robot_status_data_.begin(), send_robot_status_data_.end(), 0);
        send_robot_status_data_[0] = 0xDF;
        send_robot_status_data_[1] = 0xFD;
        send_robot_status_data_[2] = 0x40; //64
        send_robot_status_data_[3] = 0x61;
        send_robot_status_data_[4] = 0x00;
        send_robot_status_data_[5] = resp.msid;
        strcpy((char*) &send_robot_status_data_[6], resp.cmd_str.c_str());
        std::stringstream ss;
        for (int i = 0; i < send_robot_status_data_.size(); i++) {
            ss << "0x" << std::hex << static_cast<unsigned>(send_robot_status_data_[i]) << ", ";
        }
        std::cout << "send robot status data: " << ss.str() << std::endl;

        //CheckSum
        for (count_ = 2; count_ < length_ - 1; count_++)
            check_sum_ += send_robot_status_data_[count_];
        send_robot_status_data_[length_ - 1] = ~check_sum_;

        serial_com_.flushPort();
        serial_com_.writeAsync(send_robot_status_data_);
#else
#endif
    }

    void moveCmdReset() {
        is_move_ = serial_com_.is_move_ = false;
        move_cmd_.clear();
        serial_com_.move_cmd_.clear();
    }

    private:
      //Value
      unsigned int check_sum_,count_,length_,cosmo_length_;
      std::vector<uint8_t> send_data_;
      std::vector<uint8_t> send_robot_status_data_;
      MsRecvRaw cosmo_data_;

    protected:
      SerialCommunication serial_com_;
    };

  } //end namesapce controller
} //end namespaace aero

#endif
