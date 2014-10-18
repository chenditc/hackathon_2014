#include "game_state.h"
#include <cmath>

game_state::game_state(Json::Value state) {
    this->set_game_state(state);
}

void game_state::set_game_state(Json::Value message) {
    this->game_id = message["game_id"].asInt();

    Json::Value state = message["state"];
    
    if (!state["legal_moves"].isNull()) {
        this->legal_moves.clear();

        for (unsigned int i = 0; i < state["legal_moves"].size(); i++) {
            if (state["legal_moves"][i].size() == 3) {
                board_point move = {
                    state["legal_moves"][i][0].asUInt(),
                    state["legal_moves"][i][1].asUInt(),
                    state["legal_moves"][i][2].asUInt()
                };
                this->legal_moves.push_back(move);
            }
            else
                cout << "Item in legal_moves had incorrect size: " << state["legal_moves"][i].size() << endl;
        }
    }

    this->board.clear();
    unsigned int size = state["board"].size();

    if (size > 255) {
        cout << "Board parsing error: board size exceeds sane limit" << endl;
        return;
    }

    for (unsigned int x = 0; x < size; x++) {
        if (state["board"][x].size() != size - x) {
            cout << "Board parsing error: expected " << (size - x) << " y rows for x = " << x << ", found " << state["board"][x].size() << endl;
            break;
        }

        vector< vector<int> > x_slice;
        for (unsigned int y = 0; y < (size - x); y++) {
            if (state["board"][x][y].size() != size - x - y) {
                cout << "Board parsing error: expected " << (size - x - y) << " z rows for (x, y) = (" << x << ", " << y << "), found " << state["board"][x][y] << endl;
                break;
            }

            vector<int> y_slice;
            for (unsigned int z = 0; z < (size - x - y); z++) {
                if (!state["board"][x][y][z].isInt()) {
                    cout << "Board parsing error: item at (x, y, z) = (" << x << ", " << y << ", " << z << ") is not an integer" << endl;
                    break;
                }
                y_slice.push_back(state["board"][x][y][z].asInt());
            }
            x_slice.push_back(y_slice);
        }
        this->board.push_back(x_slice);
    }

    if (!state["time_remaining_ns"].isNull())
        this->time_remaining_ns = state["time_remaining_ns"].asUInt64();

    if (!state["moves_remaining"].isNull())
        this->moves_remaining = state["moves_remaining"].asInt();

    this->player_number = state["player"].asInt();

    int player1_score = state["score"]["player1"].asInt();
    int player2_score = state["score"]["player2"].asInt();
    this->your_points = this->player_number == 1 ? player1_score : player2_score;
    this->their_points = this->player_number == 2 ? player1_score : player2_score;

    this->your_tokens = state["tokens"].asInt();
    if (!state["opponent_tokens"].isNull())
        this->their_tokens = state["opponent_tokens"].asInt();
    //this->opponent_id = state["opponent_id"].asInt();

    // Mycode
    if (legal_moves.size() > 0)
        nextMove = legal_moves.at(0);
}

int game_state::getScore() const {
    return calcualteScore();
}

bool game_state::isNearEmpty(int x, int y, int z) const {
    if (x + y + z + 1 < board.size() && board[x+1][y][z] != 0)
        return false;
    if (x + y + z + 1 < board.size() && board[x][y+1][z] != 0)
        return false;
    if (x - 1 > -1 && board[x-1][y][z] != 0)
        return false;
    if (y - 1 > -1 && board[x][y-1][z] != 0)
        return false;

    return true;
} 

int game_state::calcualteScore() const {
    // TODO: put prediction function here
    double myScore = 0;
    int hisScore = 0;

    int size = board.size();

    // central point
    double best = size/3;
    for(int x = 0; x < board.size(); x++) {
        for(int y = 0; y < board[x].size(); y++) {
            // more central, better point
            if (board[x][y][0] == player_number) {
                myScore += x + y - best -best - abs(x-y);   
                if (!isNearEmpty(x, y, 0))
                    myScore -= size;
            }
            else {
                myScore -= size;
            }
            if (board[x][y][0] != player_number && board[x][y][0] != 0)
                hisScore += x + y - best;
        }
    }    
    return myScore; 
}
