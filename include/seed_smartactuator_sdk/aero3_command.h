#ifndef AERO_COMMAND_H_
#define AERO_COMMAND_H_

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <vector>
#include <unordered_map>

#include "seed_smartactuator_sdk/cosmo_receiver.h"

using namespace boost::asio;

namespace aero
{
  namespace controller
  {
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
      void readBufferAsync(uint8_t _size, uint16_t _timeout);
      void readBuffer(std::vector<uint8_t>& _receive_data, uint8_t _size);
      void flushPort();

      std::string receive_buffer_;
      bool comm_err_;
      CosmoCmdQueue cosmo_cmd_queue;

    private:
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

      std::string getCosmoCmd(){
          return serial_com_.cosmo_cmd_queue.dequeue();
      }

    private:
      //Value
      unsigned int check_sum_,count_,length_;
      std::vector<uint8_t> send_data_;

    protected:
      SerialCommunication serial_com_;
    };

  } //end namesapce controller
} //end namespaace aero

#endif
