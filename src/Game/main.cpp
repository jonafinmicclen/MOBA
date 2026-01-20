#include "GameClient/GameClient.hpp"

#include <iostream>

int main(int argc, char **argv) {
    std::cout<<"Starting Game."<<std::endl;
    GameClient gameC = GameClient();
    gameC.Run();
    return 0;
}
