//
// Created by michael on 4/6/26.
//

#ifndef GQUESTSERVER_OPCODES_H
#define GQUESTSERVER_OPCODES_H
#include <cstdint>

#endif //GQUESTSERVER_OPCODES_H

enum Opcodes : uint16_t {

  //Authentication Opcode - Must be first packet sent.
  CMSG_AUTH = 0x0001,
  SMSG_AUTH = 0x0002,

  //Ping Opcode
  CMSG_PING = 0x0003,
  SMSG_PONG = 0x0004,

  CMSG_UPDATE_PLAYER_LOCATION_ROTATION = 0x0005,
  SMSG_UPDATE_PLAYER_LOCATION_ROTATION = 0x0006,

  SMSG_LOCATION_UPDATE = 0x0007,

};