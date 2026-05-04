

#ifndef GQUESTSERVER_OPCODES_H
#define GQUESTSERVER_OPCODES_H
//#include <cstdint>



enum Opcodes : uint16_t {

  //Authentication Opcode - Must be first packet sent.
  CMSG_AUTH = 0x0001, // Authcode the client sends to request the password Hash
  SMSG_AUTH = 0x0002, // AuthCode the server sends which will contain the Hash the client will salt. 

  CMSG_AUTH_CHALLENGE = 0x0003, // Auth code the client will send containing password + hash
  SMSG_AUTH_CHALLENGE = 0x0004, // Authcode the server will send telling the client authentication was successful. 
  SMSG_AUTH_CHALLENGE_FAIL = 0x0005, // Authcode the server will send if the client authentication FAILED.

  //Ping Opcodes
  CMSG_PING = 0x0006, // OpCode the client sends regarding its ping information
  SMSG_PONG = 0x0007, // Opcide the server sends containing server time 

  //Server Sync
  SMSG_SETSERVERTIME = 0x0008, // Server Opcode sent containing the server timestamp which will be stored in the Game Instance in UE 

  //Basic Combat Opcodes
  CMSG_PLAYER_AUTOATTACK_ON = 0x0009, // Client Opcode that is sent by the client when the autoattack command is turned on
  CMSG_PLAYER_AUTOATTACK_OFF = 0x0010, // Client Opcode that is sent by the client when the autoattack command is turned off

  //AutoAttack Damage Opcode
  SMSG_PLAYER_AUTOATTACK_DAMAGE = 0x0011, // Server Opcode sent to clients for reporting auto attack damage
  SMSG_CREATURE_AUTOATTACK_DAMAGE = 0x0012, // Server Opcode sent to relative clients 

  //Player and Creature DoAbility 
  CMSG_PLAYER_DOABILLITY = 0x0013, //Client Opcode that is sent by the client when the client requests to do an ability
  SMSG_CREATURE_DOABILITY = 0x014, //Server Opcode that sends the creature Do Ability information 

  //Movement Opcodes
  CMSG_UPDATE_PLAYER_LOCATION_ROTATION = 0x0013, // Client Opcode send when the client is moving updates position 
  SMSG_UPDATE_PLAYER_LOCATION_ROTATION = 0x0014, // Server Opcode sent to clients when the clients position is suspect or falls out of bounds, teleport etc. 
  
  //Other Player Movement Opcodes 
  SMSG_UPDATE_PLAYER_CREATURE_LOCATION_ROTATION = 0x0015,
  
  //Other Player Spawn an Despawn their Actor/Pawns on relative clients. 
  SMSG_PLAYER_SPAWN = 0x0016, // Server Message telling relative clients to spawn in a player actor, things like appearance etc. 
  SMSG_PLAYER_DESPAWN = 0x0017,//Server message telling relative clients to despawn an assigned player actor. 

  //Spawn - Despawn Relative Game Objects for the server. Interactables etc. 
  SMSG_OBJECT_SPAWN = 0x0018, //Opcode telling the client to spawn in a Game Object 
  SMSG_OBJECT_DEPAWN = 0x0019, //Opcode telling the client to de-spawn a Game Object
  SMSG_OBJECT_UPDATE = 0x0020 //Opcode telling the client to update the state of a Game Object. 
  
};


#endif //GQUESTSERVER_OPCODES_H
