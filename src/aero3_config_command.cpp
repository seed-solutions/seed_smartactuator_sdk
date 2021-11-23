#include "seed_smartactuator_sdk/aero3_command.h"

#include <iostream> // for cout/cerr
using namespace aero;
using namespace controller;

void AeroCommand::write_1byte(uint16_t _address, uint8_t *_write_data)
{
  std::vector<uint8_t> send_data;
  std::vector<uint8_t> receive_data;

  length_ = 39;
  send_data.resize(length_);
  fill(send_data.begin(),send_data.end(),0);
  receive_data.resize(length_ + 2);
  fill(receive_data.begin(),receive_data.end(),0);

  send_data[0] = 0xFC;    //Headder
  send_data[1] = 0xCF;    //Headder
  send_data[2] = 35;      //Data Length
  send_data[3] = 0xFF;    //Command
  send_data[4] = _address >> 8;
  send_data[5] = _address;

  send_data[length_-1] = 0xFF;  //checksum

  for(int i = 0; i < 32; ++i){
      send_data[6+i] = _write_data[i];
  }

  serial_com_.writeAsync(send_data);

  serial_com_.readBuffer(receive_data,receive_data.size());

}


void AeroCommand::write_2byte(uint16_t _address, uint16_t *_write_data)
{
  std::vector<uint8_t> send_data;
  std::vector<uint8_t> receive_data;

  length_ = 71;
  send_data.resize(length_);
  fill(send_data.begin(),send_data.end(),0);
  receive_data.resize(length_ + 2);
  fill(receive_data.begin(),receive_data.end(),0);

  send_data[0] = 0xFC;    //Headder
  send_data[1] = 0xCF;    //Headder
  send_data[2] = 67;      //Data Length
  send_data[3] = 0xFF;    //Command
  send_data[4] = _address >> 8;
  send_data[5] = _address;

  send_data[length_-1] = 0xFF;  //checksum

  for(int i = 0; i < 32; ++i){
      send_data[6+i*2] = _write_data[i] >> 8;
      send_data[7+i*2] = _write_data[i];
  }

  //serial_com_.clear_serial_port("input");
  serial_com_.writeAsync(send_data);
  serial_com_.readBuffer(receive_data,receive_data.size());
}

std::vector<uint8_t> AeroCommand::read_1byte(uint16_t _address)
{

  std::vector<uint8_t> send_data;
  std::vector<uint8_t> receive_data;

  length_ = 8;
  send_data.resize(length_);
  fill(send_data.begin(),send_data.end(),0);
  receive_data.resize(38);
  fill(receive_data.begin(),receive_data.end(),0);

  send_data[0] = 0xFC;    //Headder
  send_data[1] = 0xCF;    //Headder
  send_data[2] = 4;      //Data Length
  send_data[3] = 0x00;    //Command
  send_data[4] = _address >> 8;
  send_data[5] = _address;
  send_data[6] = 0x20;   //read Data Length

  send_data[length_-1] = 0xFF;  //checksum

  //serial_com_.clear_serial_port("input");
  serial_com_.writeAsync(send_data);
  serial_com_.readBuffer(receive_data,receive_data.size());

  return receive_data;
}

std::vector<uint8_t> AeroCommand::read_2byte(uint16_t _address)
{
  std::vector<uint8_t> send_data;
  std::vector<uint8_t> receive_data;

  length_ = 8;
  send_data.resize(length_);
  fill(send_data.begin(),send_data.end(),0);
  receive_data.resize(71);
  fill(receive_data.begin(),receive_data.end(),0);

  send_data[0] = 0xFC;    //Headder
  send_data[1] = 0xCF;    //Headder
  send_data[2] = 4;      //Data Length
  send_data[3] = 0x00;    //Command
  send_data[4] = _address >> 8;
  send_data[5] = _address;
  send_data[6] = 0x40;   //read Data Length

  send_data[length_-1] = 0xFF;  //checksum

  //serial_com_.clear_serial_port("input");
  serial_com_.writeAsync(send_data);
  serial_com_.readBuffer(receive_data,receive_data.size());

  return receive_data;
}
