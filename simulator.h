#include <vector>
#include "game_state.h"
#include "my_player.h"
using namespace std;


typedef vector<game_state> StateContainer;

class Simulator {
    // container that store all possible state.
    StateContainer stateContainer;
        
    MyPlayer me;
    MyPlayer him;


    game_state gameState;
    Simulator(const game_state &gameStateInput) 
        : gameState(gameStateInput),
          me(MyPlayer(gameStateInput.player_number)),
          him(MyPlayer(gameStateInput.player_number == 1? 2 : 1)) {
    }
    public:
        // sort everything we have, pick the best.
        board_point getBestMove();
        
        // simulate all legal move and generate new game_state into container
        void simulateAllMyMove();
        void simulateAllHisMove();

        // get all legal move for specific side
        vector<board_point> getAllLegalMoves(const game_state &gameState, const int side);

        // create a new game state and put a new move on specific point 
        static game_state placeMove(const game_state &gameState,
                                    const int x,
                                    const int y,
                                    const int z,
                                    const int side);        

};
