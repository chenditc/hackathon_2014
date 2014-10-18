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
    assert(stateContainer.at(0).getScore() >= stateContainer.back().getScore());

    stateContainer.at(0).logInfo();
    return stateContainer.at(0).nextMove; 
}

void Simulator::simulateAllMyMove(const game_state &oldGameState) {
    // calcualte the legal move for my side and call simuate on that player
    for (int i = 0; i < oldGameState.legal_moves.size(); i++) { 
        game_state newGameState = placeMove(oldGameState, i, me.myPlayerNum) ;
        // put all generated game_state into container
        stateContainer.push_back(newGameState);
    }

    // assume wait
    stateContainer.push_back(wait(oldGameState, me.myPlayerNum)); 
}


void Simulator::simulateAllHisMove() {
     // TODO: calcualte the legal move for my side and call simulate on that player
    // TODO: put all generated game_state into container


}

void updateBoard(vector< vector< vector<int> > > &board,
                 int x, int y, int z, int side) {
    
    if(board[x][y][z] != side && board[x][y][z] != 0)
        assert("trying to update something not legal");
    if (z > 0) {
        updateBoard(board, x+1, y, z-1, side);
        updateBoard(board, x, y+1, z-1, side);
        updateBoard(board, x, y , z-1, side);
    }
    board[x][y][z] = side;
}

// create a new game state and put a new move on specific point 
game_state Simulator::placeMove(const game_state &gameState, 
                                const int nextPoint,
                                const int side) {
    game_state result = gameState;
    board_point next = gameState.legal_moves.at(nextPoint);

    // place move
    result.board[next.x][next.y][next.z] = side; 
    // update all points under it.
    updateBoard(result.board, next.x, next.y, next.z, side);

    // take out tokens
    result.your_tokens -= next.z + 1; 
    result.nextMove = next;

    // update legal move
    result.updateLegalMovesAndEmptyCount();
    return result;
} 

game_state Simulator::wait(const game_state &gameState,
                           const int side) {
    game_state result = gameState;
    result.your_tokens++;
    result.updateLegalMovesAndEmptyCount();
    result.nextMove = {256, 256, 256};
    return result;    
}
