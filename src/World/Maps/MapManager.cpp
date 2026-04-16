#include "MapManager.h"
#include <fstream>
#include "DetourNavMesh.h"
#include "DetourNavMeshQuery.h"
#include <vector>
#include <iostream>

dtNavMesh* LoadNavMesh() {
    const char* path = "src/World/Maps/MapNavData/dungeon.nav";

    std::ifstream inFile(path, std::ios::in | std::ios::binary | std::ios::ate);
    if (!inFile.is_open()) return nullptr;

    std::streamsize size = inFile.tellg();
    inFile.seekg(0, std::ios::beg);

    // Allocate memory and read the binary nav data
    unsigned char* navData = (unsigned char*)dtAlloc(size, DT_ALLOC_PERM);
    inFile.read((char*)navData, size);
    inFile.close();

    // Initialize the Detour NavMesh
    dtNavMesh* mesh = dtAllocNavMesh();
    dtStatus status = mesh->init(navData, size, DT_TILE_FREE_DATA);

    if (dtStatusFailed(status)) {
        dtFree(navData);
        dtFreeNavMesh(mesh);
        return nullptr;
    }

    return mesh;
}




// Assuming 'mesh' is the dtNavMesh* you loaded previously
void FindPathExample(dtNavMesh* mesh) {
    // 1. Initialize the Query Object
    dtNavMeshQuery* navQuery = dtAllocNavMeshQuery();
    // 2048 is the max number of nodes the A* search can process.
    // AzerothCore uses larger numbers (up to 65535) for long overland paths.
    navQuery->init(mesh, 2048);

    // 2. Define Start and End Coordinates (x, y, z)
    float startPos[3] = { 10.0f, 0.0f, 15.0f }; // Example starting coordinate
    float endPos[3]   = { 50.0f, 0.0f, 60.0f }; // Example destination coordinate

    // 3. Define the Search Extents and Filter
    // "Extents" define the bounding box size used to snap coordinates to the nearest polygon.
    // X, Y, Z. Usually, you want a larger Y extent (height) to handle falling/stairs.
    float polyPickExt[3] = { 2.0f, 4.0f, 2.0f };

    dtQueryFilter filter;
    filter.setIncludeFlags(0xffff); // Allow all walkable surface types
    filter.setExcludeFlags(0);

    // 4. Find the NavMesh Polygons closest to our raw coordinates
    dtPolyRef startRef = 0;
    dtPolyRef endRef = 0;
    float nearestStart[3];
    float nearestEnd[3];

    navQuery->findNearestPoly(startPos, polyPickExt, &filter, &startRef, nearestStart);
    navQuery->findNearestPoly(endPos, polyPickExt, &filter, &endRef, nearestEnd);

    if (!startRef || !endRef) {
        std::cerr << "Could not snap start or end points to the NavMesh." << std::endl;
        dtFreeNavMeshQuery(navQuery);
        return;
    }

    // 5. Calculate the rough path (a list of Polygon References)
    const int MAX_POLYS = 256;
    dtPolyRef path[MAX_POLYS];
    int pathCount = 0;

    navQuery->findPath(startRef, endRef, nearestStart, nearestEnd, &filter, path, &pathCount, MAX_POLYS);

    if (pathCount == 0) {
        std::cerr << "No path could be found between those points." << std::endl;
        dtFreeNavMeshQuery(navQuery);
        return;
    }

    // 6. "String Pulling" (Smoothing the path into exact X,Y,Z waypoints)
    // The previous step just gave us the polygons. Now we calculate the exact straight lines.
    const int MAX_WAYPOINTS = 256;
    float straightPath[MAX_WAYPOINTS * 3]; // Array of [x,y,z, x,y,z...]
    unsigned char straightPathFlags[MAX_WAYPOINTS];
    dtPolyRef straightPathPolys[MAX_WAYPOINTS];
    int straightPathCount = 0;

    navQuery->findStraightPath(nearestStart, nearestEnd, path, pathCount,
                               straightPath, straightPathFlags,
                               straightPathPolys, &straightPathCount, MAX_WAYPOINTS);

    // 7. Output the resulting path
    std::cout << "Found a path with " << straightPathCount << " waypoints:" << std::endl;
    for (int i = 0; i < straightPathCount; ++i) {
        float px = straightPath[i * 3];
        float py = straightPath[i * 3 + 1];
        float pz = straightPath[i * 3 + 2];

        std::cout << "Waypoint " << i << ": (" << px << ", " << py << ", " << pz << ")" << std::endl;
    }

    // Cleanup
    dtFreeNavMeshQuery(navQuery);
}
