//
// Created by michael on 4/6/26.
//

#ifndef GQUESTSERVER_WORLDSESSIONMGR_H
#define GQUESTSERVER_WORLDSESSIONMGR_H
#include <vector>

#include "WorldSession.h"


class WorldSessionMgr {
    std::vector<std::shared_ptr<WorldSession>> sessions_;
    std::mutex sessions_mutex_;

    WorldSessionMgr() = default;
    WorldSessionMgr(const WorldSessionMgr&) = delete;


    public:
    std::vector<WorldSession> sessions_;
};


#endif //GQUESTSERVER_WORLDSESSIONMGR_H