#include "simulator.h"
#include <algorithm>
#include <assert.h>

struct StateComp {
    bool operator() (const game_state &lhs, const game_state &rhs) {
        return lhs.getScore() > rhs.getScore();
    }
} stateComp;

board_point Simulator::getBestMove() {
    // stateContainer.push_back(gameState);
    simulateAllMyMove(gameState);

    // calculate the score in all condition
    for(StateContainer::iterator it = stateContainer.begin();
            it != stateContainer.end(); it++) {
        it->calcualteScore();
    }
    // sort the container using comparator
    sort(stateContainer.begin(), stateContainer.end(), stateComp);  
    // return the move that can lead to best situation. 

    assert(stateContainer.size() > 0);
    assert(stateContainer.at(0).getScore() > stateContainer.back().getScore());
    return stateContainer.at(0).nextMove; 
}

void Simulator::simulateAllMyMove(const game_state &oldGameState) {
    // calcualte the legal move for my side and call simuate on that player
    for (int i = 0; i < oldGameState.legal_moves.size(); i++) { 
        game_state newGameState = placeMove(oldGameState, i, me.myPlayerNum) ;
        // put all generated game_state into container
        stateContainer.push_back(newGameState);
    }
}


void Simulator::simulateAllHisMove() {
     // TODO: calcualte the legal move for my side and call simulate on that player
    // TODO: put all generated game_state into container


}

// create a new game state and put a new move on specific point 
game_state Simulator::placeMove(const game_state &gameState, 
                                const int nextPoint,
                                const int side) {
    game_state result = gameState;
    board_point next = gameState.legal_moves.at(nextPoint);

    // place move
    result.board[next.x][next.y][next.z] = side; 
    result.nextMove = next;

    // update legal move
    result.legal_moves.pop_back();
    return result;
} 


