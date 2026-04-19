

#ifndef GQUESTSERVER_OPCODES_H
#define GQUESTSERVER_OPCODES_H
//#include <cstdint>



enum Opcodes : uint16_t {

  //Authentication Opcode - Must be first packet sent.
  CMSG_AUTH = 0x0001,
  SMSG_AUTH = 0x0002,

  //Ping Opcode
  CMSG_PING = 0x0003,
  SMSG_PONG = 0x0004,

  //Player movement - CMSG received from the client, SMSG is to SET Player Rotation ie a warp portal or local spell
  CMSG_UPDATE_PLAYER_LOCATION_ROTATION = 0x0005,
  SMSG_UPDATE_PLAYER_LOCATION_ROTATION = 0x0006,

  //Add or Remove Players from the player inView menu.
  SMSG_PLAYER_SPAWN = 0x0008,
  SMSG_PLAYER_DESPAWN = 0x0009,

  SMSG_LOCATION_UPDATE = 0x0007,

  CMSG_PLAYER_MOVEMENT = 0x000A,
  SMSG_PLAYER_MOVEMENT = 0x000B,

};


#endif //GQUESTSERVER_OPCODES_H
