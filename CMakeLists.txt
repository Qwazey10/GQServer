cmake_minimum_required(VERSION 4.0)
project(GQuestServer)

set(CMAKE_CXX_STANDARD 20)

add_executable(GQuestServer main.cpp
        src/NetworkSocketMgr/NetworkSocketMgr.h
        src/NetworkSocketMgr/NetworkSocketMgr.cpp
        src/World/Player.cpp
        src/World/Player.h
        src/World/WorldSession.h
        src/World/WorldSession.cpp
        src/World/WorldSessionMgr.h
        src/World/World.cpp
        src/World/World.h
        src/World/Opcodes/Opcodes.h
        src/World/Packets/WorldPacket.cpp
        src/World/Packets/WorldPacket.h
        src/World/WorldSessionMgr.cpp
        src/World/Opcodes/OpcodeHandler.cpp
        src/World/Opcodes/OpcodeHandler.h
        src/World/Creature.cpp
        src/World/Creature.h
        src/World/Map.cpp
        src/World/Map.h
        src/World/AI/CreatureAI.cpp
        src/World/AI/CreatureAI.h
        src/World/Maps/MapManager.cpp
        src/World/Maps/MapManager.h
        src/World/Maps/MapBuilder.cpp
        src/World/Maps/MapBuilder.h)



target_include_directories(${PROJECT_NAME}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/asio/include
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/Recast/Recast/Include


)