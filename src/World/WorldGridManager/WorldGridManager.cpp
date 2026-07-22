#include "WorldGridManager.h"
#include <cmath>
#include <iostream>
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

    //Init Debug
    for (const auto& grid : m_gridCoordinates)
    {
        std::cout
            << "Grid "
            << grid.grid_id
            << " ("
            << grid.x_coordinate
            << ", "
            << grid.y_coordinate
            << ")\n";
    }

    std::cout << "=====================================\n";
    std::cout << "World Grid Initialized\n";
    std::cout << "Grid Count X : " << m_gridCountX << '\n';
    std::cout << "Grid Count Y : " << m_gridCountY << '\n';
    std::cout << "Total Grids  : " << m_gridCoordinates.size() << '\n';
    std::cout << "=====================================\n";
}

void WorldGridManager::CalculateVisiblePlayers()
{
    auto sessions = WorldSessionMgr::Instance().GetSessions();

    for (auto& session : sessions)
    {
        auto player = session->GetPlayer();
        if (!player) continue;

        // 1. Shift current state to previous state
        player->previousVisiblePlayers_ = std::move(player->currentVisiblePlayers_);
        player->currentVisiblePlayers_.clear();

        // 2. Build new current state based on visible grids
        for (int32_t gridID : player->visibleGrids_)
        {
            auto itr = m_playersByGrid.find(gridID);
            if (itr == m_playersByGrid.end()) continue;

            for (auto& other : itr->second)
            {
                // Don't add ourselves
                if (other == player) continue;

                player->currentVisiblePlayers_.insert(other->GetGUID());
            }
        }
    }
}

void WorldGridManager::CalculatePlayerGridCoordinates()
{
    auto sessions = WorldSessionMgr::Instance().GetSessions();
    m_playersByGrid.clear();

    for (auto& session : sessions)
    {
        auto player = session->GetPlayer();
        if (!player) continue;

        const Position& pos = player->GetPosition();

        int32_t gridX = static_cast<int32_t>((pos.x - minCoordinate_x) / gridWidth);
        int32_t gridY = static_cast<int32_t>((pos.y - minCoordinate_y) / gridHeight);

        gridX = std::clamp(gridX, 0, m_gridCountX - 1);
        gridY = std::clamp(gridY, 0, m_gridCountY - 1);

        // FIX: Store these on the player so CalculateVisiblePlayers can use them
        player->gridX_ = gridX;
        player->gridY_ = gridY;

        int32_t gridID = gridY * m_gridCountX + gridX;

        if (player->zoneID_ != gridID)
        {
            player->zoneID_ = gridID;
            // Radius 1 = 9 grids total (center + 8 surrounding)
            player->visibleGrids_ = GetVisibleGrids(gridX, gridY, 1);
        }

        m_playersByGrid[gridID].push_back(player);
    }
}

std::vector<int32_t> WorldGridManager::GetVisibleGrids(
    int32_t gridX,
    int32_t gridY,
    int32_t radius) const
{
    std::vector<int32_t> grids;

    std::cout << "\n=== Visible Grids ===\n";
    std::cout << "Center Grid: (" << gridX << ", " << gridY
              << ") Radius: " << radius << "\n";

    for (int32_t y = -radius; y <= radius; ++y)
    {
        for (int32_t x = -radius; x <= radius; ++x)
        {
            int32_t nx = gridX + x;
            int32_t ny = gridY + y;

            if (nx < 0 || nx >= m_gridCountX)
                continue;

            if (ny < 0 || ny >= m_gridCountY)
                continue;

            int32_t gridId = ny * m_gridCountX + nx;
            grids.push_back(gridId);

            std::cout << "Grid (" << nx << ", " << ny
                      << ") -> ID " << gridId << '\n';
        }
    }

    std::cout << "Returned " << grids.size() << " grids.\n";
    std::cout << "=====================\n\n";

    return grids;
}



void WorldGridManager::Update(float Diff) {

    //Calculate which players are in which grid
    CalculatePlayerGridCoordinates();

    //Calculate Visible Players
    CalculateVisiblePlayers();
}

/*std::vector<int32_t> WorldGridManager::GetVisibleGrids(
    int32_t gridX,
    int32_t gridY,
    int32_t radius) const
{
    std::vector<int32_t> grids;

    for (int32_t y = -radius; y <= radius; ++y)
    {
        for (int32_t x = -radius; x <= radius; ++x)
        {
            int32_t nx = gridX + x;
            int32_t ny = gridY + y;

            if (nx < 0 || nx >= m_gridCountX)
                continue;

            if (ny < 0 || ny >= m_gridCountY)
                continue;

            grids.push_back(ny * m_gridCountX + nx);
        }
    }


    return grids;
}*/


//debug for calculate player grid coordinates
/*std::cout << "\n==== Players By Grid ====\n";

for (const auto& [grid, players] : m_playersByGrid)
{
    std::cout << "Grid " << grid << ": ";

    for (auto& p : players)
    {
        std::cout << p->GetGUID() << " ";
    }

    std::cout << '\n';
}
std::cout << "=========================\n";*/