#pragma once

#include "json/json.h"
#include <iostream>
#include <vector>

using namespace std;

struct board_point {
    unsigned int x, y, z;
};

class game_state {
    public:
        game_state(Json::Value);
        void set_game_state(Json::Value);

        vector<board_point> legal_moves;

        vector< vector< vector<int> > > board;

        int game_id;
        int your_points;
        int their_points;
        int opponent_id;
        int player_number;
        int your_tokens;
        int their_tokens;
        unsigned long long time_remaining_ns;
        int moves_remaining;

        board_point nextMove;
        int score;

        // calcualte evaluation score
        int calcualteScore();

        // get evaluation score
        int getScore() const; 

        // a table store the legal value
        void updateLegalTable(vector< vector< vector<int> > > &legalTable,
                              int x, int y, int z);

        // update legal move array
        void updateLegalMovesAndEmptyCount();

        int countEmpty(const int x, const int y, const int z);
        // find the best point in this move that can get maximum extra point, return the extra point
        int findBestLevel();

        // swap player number between me and oopponent, so we can simulate his best shot
        void swapUser();

        void logInfo();
        void logFinalScore();

};
