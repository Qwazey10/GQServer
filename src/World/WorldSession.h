//
// Created by michael on 4/6/26.
//

#ifndef GQUESTSERVER_WORLDSESSION_H
#define GQUESTSERVER_WORLDSESSION_H
#include "asio/ip/tcp.hpp"


class WorldSession {
    asio::ip::tcp::socket playerSocket_;
    int playerId_;

};


#endif //GQUESTSERVER_WORLDSESSION_H