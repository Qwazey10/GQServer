#pragma once

#include <string>
#include <unordered_map>
#include <DetourNavMesh.h>
#include <DetourNavMeshQuery.h>
#include <vector>

struct NavMeshData
{
    dtNavMesh* navMesh = nullptr;
    dtNavMeshQuery* query = nullptr;
};

class MapManager
{
public:
    bool LoadMap(int mapId, const std::string& file);
    void UnloadMap(int mapId);
    dtNavMesh* LoadNavMesh();
    void FindPathExample(dtNavMesh* mesh);

private:
    std::unordered_map<int, NavMeshData> m_maps;


};