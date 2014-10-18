#include "simulator.h"


board_point Simulator::getBestMove() {
    // TODO: sort the container using comparator

}

void Simulator::simulateAllMyMove() {
    // TODO: calcualte the legal move for my side and call simulate on that player
    // TODO: put all generated game_state into container

}


void Simulator::simulateAllHisMove() {
     // TODO: calcualte the legal move for my side and call simulate on that player
    // TODO: put all generated game_state into container


}
        // get all legal move for specific side
vector<board_point> Simulator::getAllLegalMoves(const game_state &gameState, const int side) {
    // TODO: 1. get moves only on first floor
}

// create a new game state and put a new move on specific point 
static game_state Simulator::placeMove(const game_state &gameState,
                                       const int x,
                                       const int y,
                                       const int z,
                                       const int side) {
    


} 


