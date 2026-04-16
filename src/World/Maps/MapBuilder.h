#pragma once

#include <string>
#include <vector>
#include <Recast.h>
#include <DetourNavMeshBuilder.h>

class MapBuilder
{


public:


    MapBuilder() = default;
    MapBuilder(const MapBuilder&) = delete;
    MapBuilder& operator=(const MapBuilder&) = delete;

    void GenerateNavMesh();

    static MapBuilder& Instance() {
        static MapBuilder instance;
        return instance;
    }

    rcConfig cfg;






};