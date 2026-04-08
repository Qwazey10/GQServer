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
        src/World/Opcodes/Opcodes.h)

target_include_directories(${PROJECT_NAME}
        PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/src/
        PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/src/asio/include
)