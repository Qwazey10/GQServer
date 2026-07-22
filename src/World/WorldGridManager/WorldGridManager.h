#ifndef GQUESTSERVER_WORLDGRIDMANAGER_H
#define GQUESTSERVER_WORLDGRIDMANAGER_H
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

class Player;

struct GridCoordinate
{
    int32_t grid_id;

    int32_t x_coordinate;
    int32_t y_coordinate;

    float minX;
    float maxX;

    float minY;
    float maxY;
};

class WorldGridManager {
public:

    //Set up the grid coordinate system, create the spatialized grid system
    //And assign them zone ids, each unit is 1cm, which will correlate to
    //Unreal Engine Units.

    // Delete copy
    WorldGridManager(const WorldGridManager&) = delete;
    WorldGridManager& operator=(const WorldGridManager&) = delete;

    // Instance Declaration
    static WorldGridManager& Instance() {
        static WorldGridManager instance;
        return instance;
    }
    WorldGridManager() = default;

    
    void Initialize();

    void CalculateVisiblePlayers();

    float minCoordinate_x = -100000.0f;
    float minCoordinate_y = -100000.0f;

    float maxCoordinate_x = 100000.0f;
    float maxCoordinate_y = 100000.0f;

    float gridWidth = 10000.0f;
    float gridHeight = 10000.0f;

    std::vector<GridCoordinate> m_gridCoordinates;

    int32_t m_gridCountX = 0;
    int32_t m_gridCountY = 0;


    std::unordered_map<int32_t, std::vector<std::shared_ptr<Player>>> m_playersByGrid;
    const std::unordered_map<int32_t, std::vector<std::shared_ptr<Player>>>& GetPlayersByGrid() const
    {
        return m_playersByGrid;
    }


    //Update Function that will be called from the world thread that will update
    //The Grid coordinates
    void Update(float Diff);

    void CalculatePlayerGridCoordinates();

    std::vector<int32_t> GetVisibleGrids(int32_t gridX, int32_t gridY, int32_t radius) const;


};

#endif //GQUESTSERVER_WORLDGRIDMANAGER_H
