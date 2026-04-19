cmake_minimum_required(VERSION 4.0)
project(GQuestServer)

set(CMAKE_CXX_STANDARD 20)

add_executable(GQuestServer
        main.cpp

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


        # DebugUtils
        src/Recast/DebugUtils/Source/DebugDraw.cpp
        src/Recast/DebugUtils/Source/DetourDebugDraw.cpp
        src/Recast/DebugUtils/Source/RecastDebugDraw.cpp
        src/Recast/DebugUtils/Source/RecastDump.cpp

        # Detour
        src/Recast/Detour/Source/DetourAlloc.cpp
        src/Recast/Detour/Source/DetourAssert.cpp
        src/Recast/Detour/Source/DetourCommon.cpp
        src/Recast/Detour/Source/DetourNavMesh.cpp
        src/Recast/Detour/Source/DetourNavMeshBuilder.cpp
        src/Recast/Detour/Source/DetourNavMeshQuery.cpp
        src/Recast/Detour/Source/DetourNode.cpp

        #DetourCrowd
        src/Recast/DetourCrowd/Source/DetourCrowd.cpp
        src/Recast/DetourCrowd/Source/DetourLocalBoundary.cpp
        src/Recast/DetourCrowd/Source/DetourObstacleAvoidance.cpp
        src/Recast/DetourCrowd/Source/DetourPathCorridor.cpp
        src/Recast/DetourCrowd/Source/DetourPathQueue.cpp
        src/Recast/DetourCrowd/Source/DetourProximityGrid.cpp

        #DetourTileCache
        src/Recast/DetourTileCache/Source/DetourTileCache.cpp
        src/Recast/DetourTileCache/Source/DetourTileCacheBuilder.cpp

        # Recast
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

        src/Recast/DetourTileCache/Source/DetourTileCache.cpp
        src/Recast/DetourTileCache/Source/DetourTileCache.cpp
        src/Recast/DetourTileCache/Include/DetourTileCache.h
        src/World/TimeManager/TimeManager.cpp
        src/World/TimeManager/TimeManager.h


)


target_include_directories(${PROJECT_NAME}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/asio/include
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/Recast/Recast/Include
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/Recast/Detour/Include
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/Recast/DetourCrowd/Include
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/Recast/DetourTileCache/Include
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/Recast/DebugUtils/Include


)