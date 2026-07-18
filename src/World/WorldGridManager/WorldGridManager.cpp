#include "WorldGridManager.h"
#include <cmath>
#include "World/WorldSessionMgr.h"

void WorldGridManager::Initialize() {


    m_gridCoordinates.clear();

    m_gridCountX = static_cast<int32_t>(
        std::ceil((maxCoordinate_x - minCoordinate_x) / gridWidth));

    m_gridCountY = static_cast<int32_t>(
        std::ceil((maxCoordinate_y - minCoordinate_y) / gridHeight));

    int32_t gridID = 0;

    for (int32_t y = 0; y < m_gridCountY; ++y)
    {
        for (int32_t x = 0; x < m_gridCountX; ++x)
        {
            GridCoordinate grid;

            grid.x_coordinate = x;
            grid.y_coordinate = y;
            grid.grid_id = gridID++;

            m_gridCoordinates.push_back(grid);
        }
    }
}

void WorldGridManager::CalculatePlayerGridCoordinates()
{

    //Because this will be run from the WorldThread, We GET sessions, not copy.
    //No need to copy and write. Should we offload this task to a thread later,
    //Change this to WorldSessionMgr::Instance().CopySessions and &session.
    //You will also need to update this copy with the existing Sessions array
    //in WorldSessionMngr.

    auto sessions = WorldSessionMgr::Instance().GetSessions();

    for (auto session : sessions)
    {
        if (auto player = session->GetPlayer())
        {
            const Position& pos = player->GetPosition();

            int32_t gridX = static_cast<int32_t>(
                (pos.x - minCoordinate_x) / gridWidth);

            int32_t gridY = static_cast<int32_t>(
                (pos.y - minCoordinate_y) / gridHeight);

            gridX = std::clamp(gridX, 0, m_gridCountX - 1);
            gridY = std::clamp(gridY, 0, m_gridCountY - 1);

            int32_t gridID = gridY * m_gridCountX + gridX;

            //Do comparison of existing zone ID to new gridID
            if (player->zoneID_ == gridID) {
                //Player ZoneID is the same, Do not update
            }
            else {
                //Player ZoneID is NOT the same, Update ZoneID on the player
                //Potentially call any related change functions here later.
                player->zoneID_ = gridID;
            }
        }
    }
}

void WorldGridManager::Update(float Diff) {

    CalculatePlayerGridCoordinates();
}


