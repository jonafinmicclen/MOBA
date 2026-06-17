

#include <iostream>

#include "Client/GameClient/GameClient.hpp"

int main(int argc, char **argv) {

    std::cout<<"Starting Game."<<std::endl;
    GameClient* gameC = new GameClient;
    gameC->run();

    return 0;
}
