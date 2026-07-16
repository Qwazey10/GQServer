#ifndef GQUESTSERVER_WORLDGRIDMANAGER_H
#define GQUESTSERVER_WORLDGRIDMANAGER_H
#include <cstdint>
#include <vector>

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

    void Initialize();

    float minCoordinate_x = 0.0f;
    float minCoordinate_y = 0.0f;

    float maxCoordinate_x = 100000.0f;
    float maxCoordinate_y = 100000.0f;

    float gridWidth = 10000.0f;
    float gridHeight = 10000.0f;

    std::vector<GridCoordinate> m_gridCoordinates;

    int32_t m_gridCountX = 0;
    int32_t m_gridCountY = 0;
    //So this would be a 10x10 grid on a test map size, 1cm = 1 unreal unit
    // and 1km = 100,000 cm

    //Update Function that will be called from the world thread that will update
    //The Grid coordinates
    void Update(float Diff);

    void CalculatePlayerGridCoordinates();

};

#endif //GQUESTSERVER_WORLDGRIDMANAGER_H
