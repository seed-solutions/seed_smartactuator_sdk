#ifndef AERO_COMMAND_H_
#define AERO_COMMAND_H_

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/range/iterator_range.hpp>
#include <vector>
#include <unordered_map>
#include <thread>
#include "seed_smartactuator_sdk/cosmo_receiver.h"

using namespace boost::asio;

namespace aero
{
  namespace controller
  {

  class AeroBuff{
  public:
      void set(const std::string &recvd){
          std::lock_guard<std::mutex> lk(mtx);
          this->recvd = recvd;
      }

      std::string get(){
          std::lock_guard<std::mutex> lk(mtx);
          std::string ret = recvd;
          recvd.clear();
          return ret;
      }

  private:
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
      void readBuffer(std::vector<uint8_t>& _receive_data, uint8_t _size);
      void flushPort();

      bool comm_err_;
      CosmoCmdQueue cosmo_cmd_queue;
      AeroBuff receive_buff;
      const int at_least_size = 8;

    private:
      std::thread io_thread;
      io_service io_;
      serial_port serial_;
      deadline_timer timer_;
      CosmoReceiver cosmo_receiver_;
      bool is_canceled_;
      boost::asio::streambuf stream_buffer_;

    };

    class AeroCommand{
    public:
      AeroCommand();
      ~AeroCommand();

      bool is_open_,comm_err_;

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

      std::tuple<int,int,int,int,std::string> getCosmoCmd(){
          return serial_com_.cosmo_cmd_queue.dequeue();
      }

      void sendCosmoCmdResp(int header_type, int addr, int cmd_type, int msid,std::string _cmd){
          check_sum_ = 0;
          length_ = 64;

          send_data_.resize(length_);
          fill(send_data_.begin(),send_data_.end(),0);
          if(header_type == 0){ //EF EFコマンドに対しての返信
        	  send_data_[0] = 0xEF;
        	  send_data_[1] = 0xFE;
          }else if(header_type == 1){ //FE EFコマンドに対しての返信
        	  send_data_[0] = 0xFE;
        	  send_data_[1] = 0xEF;
          }
          switch (addr) {
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
        	  send_data_[2] = 0x04;//TODO 本当は0x16だけどなんか現状の仕様だと0x16ではMSが受け取らないっぽい？
        	  break;
          case 32:
        	  send_data_[2] = 0x20;
        	  break;
          case 64:
        	  send_data_[2] = 0x40;
        	  break;
          default:
        	  send_data_[2] = addr;
        	  break;
          	}
          if(cmd_type == 1) send_data_[3] = 0xA1;
          if(cmd_type == 2) send_data_[3] = 0xA2;
          send_data_[4] = msid;
          strcpy((char*)&send_data_[5],_cmd.c_str());
          std::stringstream ss;
          for (int i = 0; i < send_data_.size(); i++)
          {
        	  ss << "0x" << std::hex << static_cast<unsigned>(send_data_[i]) << ", ";
          }
          std::cout << "send cosmo data: " << ss.str() << std::endl;

          //CheckSum
          for(count_ = 2;count_ < length_-1;count_++) check_sum_ += send_data_[count_];
          send_data_[length_-1] = ~check_sum_;

          serial_com_.flushPort();
          serial_com_.writeAsync(send_data_);

          
      }

    private:
      //Value
      unsigned int check_sum_,count_,length_,cosmo_length_;
      std::vector<uint8_t> send_data_;
      MsRecvRaw cosmo_data_;

    protected:
      SerialCommunication serial_com_;
    };

  } //end namesapce controller
} //end namespaace aero

#endif
