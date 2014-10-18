#ifndef MY_PLAYER_H
#define MY_PLAYER_H

#include<game_state.h>
// AI class

class MyPlayer {
    int myPlayerNum;

    public:
    MyPlayer(int playerNum) {
        myPlayerNum = playerNum;
    }

    // place the next legal move available for this player
    void placeNextLegalMoveOnState(game_state &game_state);

};

#endif
