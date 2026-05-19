cmake_minimum_required(VERSION 3.22)
project(GQuestServer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(GQuestServer
    main.cpp

    # Your sources
    src/NetworkSocketMgr/NetworkSocketMgr.cpp
    src/World/Player.cpp
    src/World/WorldSession.cpp
    src/World/World.cpp
    src/World/Packets/WorldPacket.cpp
    src/World/WorldSessionMgr.cpp
    src/World/Opcodes/OpcodeHandler.cpp
    src/World/Creature.cpp
    src/World/Map.cpp
    src/World/AI/CreatureAI.cpp
    src/World/Maps/MapManager.cpp
    src/World/Maps/MapBuilder.cpp
    src/World/TimeManager/TimeManager.cpp

    src/World/SQL/DatabaseManager.cpp
    src/World/SQL/DB/DBWorker.cpp
    src/World/SQL/DB/DatabasePool.cpp


    # Recast/Detour sources (only .cpp files)
    src/Recast/DebugUtils/Source/DebugDraw.cpp
    src/Recast/DebugUtils/Source/DetourDebugDraw.cpp
    src/Recast/DebugUtils/Source/RecastDebugDraw.cpp
    src/Recast/DebugUtils/Source/RecastDump.cpp

    src/Recast/Detour/Source/DetourAlloc.cpp
    src/Recast/Detour/Source/DetourAssert.cpp
    src/Recast/Detour/Source/DetourCommon.cpp
    src/Recast/Detour/Source/DetourNavMesh.cpp
    src/Recast/Detour/Source/DetourNavMeshBuilder.cpp
    src/Recast/Detour/Source/DetourNavMeshQuery.cpp
    src/Recast/Detour/Source/DetourNode.cpp

    src/Recast/DetourCrowd/Source/DetourCrowd.cpp
    src/Recast/DetourCrowd/Source/DetourLocalBoundary.cpp
    src/Recast/DetourCrowd/Source/DetourObstacleAvoidance.cpp
    src/Recast/DetourCrowd/Source/DetourPathCorridor.cpp
    src/Recast/DetourCrowd/Source/DetourPathQueue.cpp
    src/Recast/DetourCrowd/Source/DetourProximityGrid.cpp

    src/Recast/DetourTileCache/Source/DetourTileCache.cpp
    src/Recast/DetourTileCache/Source/DetourTileCacheBuilder.cpp

    src/Recast/Recast/Source/Recast.cpp
    src/Recast/Recast/Source/RecastAlloc.cpp
    src/Recast/Recast/Source/RecastArea.cpp
    src/Recast/Recast/Source/RecastAssert.cpp
    src/Recast/Recast/Source/RecastContour.cpp
    src/Recast/Recast/Source/RecastFilter.cpp
    src/Recast/Recast/Source/RecastLayers.cpp
    src/Recast/Recast/Source/RecastMesh.cpp
    src/Recast/Recast/Source/RecastMeshDetail.cpp
    src/Recast/Recast/Source/RecastRasterization.cpp
    src/Recast/Recast/Source/RecastRegion.cpp
     "src/World/SQL/DB/DBResult.h" "src/World/SQL/DB/PreparedStatements.h" "src/World/SQL/DB/DBOperation.h" "src/World/SQL/DB/QueryResults.h" "src/World/SQL/DB/Field.h" "src/World/SQL/DB/Statements.h")

find_package(OpenSSL REQUIRED)
find_package(unofficial-libmariadb CONFIG REQUIRED)

# Windows-only socket libraries
if (WIN32)
    target_link_libraries(GQuestServer PRIVATE
            ws2_32
            mswsock
    )
endif()
target_link_libraries(GQuestServer PRIVATE
    OpenSSL::SSL
    OpenSSL::Crypto
)

target_link_libraries(GQuestServer PRIVATE unofficial::libmariadb)

target_include_directories(GQuestServer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/asio/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Recast/Recast/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Recast/Detour/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Recast/DetourCrowd/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Recast/DetourTileCache/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Recast/DebugUtils/Include
)