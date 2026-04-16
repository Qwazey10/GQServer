#include <cmath>
#include <cstring>
#include <iostream>
#include <iosfwd>
#include <fstream>
#include <sstream>
#include <vector>
#include "Recast.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshBuilder.h"

// Assume you have a simple function to load OBJ files into flat arrays
bool LoadOBJ(const char* filepath, std::vector<float>& outVerts, std::vector<int>& outTris) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "v") {
            // Read vertex (x, y, z)
            float x, y, z;
            iss >> x >> y >> z;
            outVerts.push_back(x);
            outVerts.push_back(y);
            outVerts.push_back(z);

            // NOTE ON AZEROTHCORE/WOW COORDINATES:
            // WoW uses Z-up. If your OBJ is Y-up, you might need to swap Y and Z here:
            // outVerts.push_back(x); outVerts.push_back(z); outVerts.push_back(-y);
        }
        else if (type == "f") {
            // Read face (triangle). OBJ formats faces like "v/vt/vn v/vt/vn v/vt/vn"
            // We only need the first part (the vertex index).
            std::string vertexData;
            int vertexCount = 0;
            int firstIndex = -1, previousIndex = -1;

            while (iss >> vertexData) {
                // Extract just the vertex index (before the first slash)
                std::string vIndexStr = vertexData.substr(0, vertexData.find('/'));
                int vIndex = std::stoi(vIndexStr);

                // OBJ indices are 1-based, C++ arrays are 0-based
                vIndex -= 1;

                // Simple triangulation for polygons with > 3 vertices (like quads)
                if (vertexCount == 0) {
                    firstIndex = vIndex;
                } else if (vertexCount >= 2) {
                    outTris.push_back(firstIndex);
                    outTris.push_back(previousIndex);
                    outTris.push_back(vIndex);
                }
                previousIndex = vIndex;
                vertexCount++;
            }
        }
    }

    return true;
}


void GenerateNavMesh() {
    const char* inputPath = "src/World/Maps/MapObj/dungeon.obj";
    const char* outputPath = "src/World/Maps/MapNavData/dungeon.nav";

    std::vector<float> vertices;
    std::vector<int> triangles;

    if (!LoadOBJ(inputPath, vertices, triangles)) {
        std::cerr << "Failed to load: " << inputPath << std::endl;
        return;
    }

    // 1. Initialize Recast Configuration
    rcConfig cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.cs = 0.3f;          // Cell size (resolution)
    cfg.ch = 0.2f;          // Cell height
    cfg.walkableSlopeAngle = 45.0f;
    cfg.walkableHeight = (int)ceil(2.0f / cfg.ch); // Agent height ~2 meters
    cfg.walkableClimb = (int)floorf(0.9f / cfg.ch); // Max climb height
    cfg.walkableRadius = (int)ceilf(0.6f / cfg.cs); // Agent radius ~0.6m
    cfg.maxEdgeLen = (int)(12.0f / cfg.cs);
    cfg.maxSimplificationError = 1.3f;
    cfg.minRegionArea = (int)rcSqr(8);
    cfg.mergeRegionArea = (int)rcSqr(20);
    cfg.maxVertsPerPoly = 6;
    cfg.detailSampleDist = 6.0f < 0.9f ? 0 : cfg.cs * 6.0f;
    cfg.detailSampleMaxError = cfg.ch * 1.0f;

    // Calculate bounding box of the OBJ
    rcCalcBounds(vertices.data(), vertices.size() / 3, cfg.bmin, cfg.bmax);
    rcCalcGridSize(cfg.bmin, cfg.bmax, cfg.cs, &cfg.width, &cfg.height);

    rcContext ctx;

    // 2. Voxelize the Input Geometry
    rcHeightfield* solid = rcAllocHeightfield();
    rcCreateHeightfield(&ctx, *solid, cfg.width, cfg.height, cfg.bmin, cfg.bmax, cfg.cs, cfg.ch);

    std::vector<unsigned char> triFlags(triangles.size() / 3);
    rcMarkWalkableTriangles(&ctx, cfg.walkableSlopeAngle, vertices.data(), vertices.size() / 3, triangles.data(), triangles.size() / 3, triFlags.data());
    rcRasterizeTriangles(&ctx, vertices.data(), vertices.size() / 3, triangles.data(), triFlags.data(), triangles.size() / 3, *solid, cfg.walkableClimb);

    // 3. Filter and Compact
    rcFilterLowHangingWalkableObstacles(&ctx, cfg.walkableClimb, *solid);
    rcFilterLedgeSpans(&ctx, cfg.walkableHeight, cfg.walkableClimb, *solid);
    rcFilterWalkableLowHeightSpans(&ctx, cfg.walkableHeight, *solid);

    rcCompactHeightfield* chf = rcAllocCompactHeightfield();
    rcBuildCompactHeightfield(&ctx, cfg.walkableHeight, cfg.walkableClimb, *solid, *chf);
    rcFreeHeightField(solid);

    // 4. Build Contours and PolyMesh
    rcErodeWalkableArea(&ctx, cfg.walkableRadius, *chf);
    rcBuildDistanceField(&ctx, *chf);
    rcBuildRegions(&ctx, *chf, 0, cfg.minRegionArea, cfg.mergeRegionArea);

    rcContourSet* cset = rcAllocContourSet();
    rcBuildContours(&ctx, *chf, cfg.maxSimplificationError, cfg.maxEdgeLen, *cset);

    rcPolyMesh* pmesh = rcAllocPolyMesh();
    rcBuildPolyMesh(&ctx, *cset, cfg.maxVertsPerPoly, *pmesh);

    rcPolyMeshDetail* dmesh = rcAllocPolyMeshDetail();
    rcBuildPolyMeshDetail(&ctx, *pmesh, *chf, cfg.detailSampleDist, cfg.detailSampleMaxError, *dmesh);

    rcFreeCompactHeightfield(chf);
    rcFreeContourSet(cset);

    // 5. Convert to Detour Data
    dtNavMeshCreateParams params;
    memset(&params, 0, sizeof(params));
    params.verts = pmesh->verts;
    params.vertCount = pmesh->nverts;
    params.polys = pmesh->polys;
    params.polyAreas = pmesh->areas;
    params.polyFlags = pmesh->flags;
    params.polyCount = pmesh->npolys;
    params.nvp = pmesh->nvp;
    params.detailMeshes = dmesh->meshes;
    params.detailVerts = dmesh->verts;
    params.detailVertsCount = dmesh->nverts;
    params.detailTris = dmesh->tris;
    params.detailTriCount = dmesh->ntris;
    params.walkableHeight = cfg.walkableHeight * cfg.ch;
    params.walkableRadius = cfg.walkableRadius * cfg.cs;
    params.walkableClimb = cfg.walkableClimb * cfg.ch;
    rcVcopy(params.bmin, pmesh->bmin);
    rcVcopy(params.bmax, pmesh->bmax);
    params.cs = cfg.cs;
    params.ch = cfg.ch;
    params.buildBvTree = true;

    unsigned char* navData = 0;
    int navDataSize = 0;

    if (!dtCreateNavMeshData(&params, &navData, &navDataSize)) {
        std::cerr << "Could not build Detour navmesh." << std::endl;
        return;
    }

    // 6. Save the NavMesh to disk
    std::ofstream outFile(outputPath, std::ios::out | std::ios::binary);
    if (outFile.is_open()) {
        // Optional: write a custom header here so your server knows it's a valid file
        outFile.write(reinterpret_cast<const char*>(navData), navDataSize);
        outFile.close();
        std::cout << "Successfully saved NavMesh to " << outputPath << std::endl;
    }

    // Cleanup
    dtFree(navData);
    rcFreePolyMesh(pmesh);
    rcFreePolyMeshDetail(dmesh);
}

