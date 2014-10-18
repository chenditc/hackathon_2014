#include "game_state.h"
#include <fstream>
#include <sstream>
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
    return score;
}

/*
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
*/

int game_state::countEmpty(const int x, const int y, const int z) {
    int cnt = 0;

    for(int h = 0; h <= z; h++) {
        for(int i = x; i - x <= z-h; i++) {
            for(int j = y; i - x + j - y <=  z-h; j++) {
                if(board[i][j][h] == 0)
                   cnt++; 
            }
        }
    } 
    return cnt;
}
// find the best point that can earn most extra points
int game_state::findBestLevel() {
    board_point bestPoint;
    int tempBest = 1;
    for(int i = 0; i <legal_moves.size(); i++) {
        if(legal_moves.at(i).z < your_tokens) {
            int x = legal_moves.at(i).x;
            int y = legal_moves.at(i).y;
            int z = legal_moves.at(i).z;
            int tempCnt =  countEmpty(x,y,z); 
            if (tempCnt > tempBest) {
                bestPoint = legal_moves.at(i);
                tempBest = tempCnt; 
            }
        }
    }
    return tempBest;
}

void game_state::swapUser() {
    player_number = player_number == 1? 2 : 1;
    swap(your_tokens, their_tokens);
}

int game_state::calcualteScore() {
    // TODO: put prediction function here
    double myScore = 0;
    double hisScore = 0;

    int size = board.size();

    // get basic score
    for(int x = 0; x < board.size(); x++) {
        for(int y = 0; y < board[x].size(); y++) {
            for(int z = 0; z < board[x][y].size(); z++) {
                if(board[x][y][z] == player_number)
                    myScore++;
                else if (board[x][y][z] != 0)
                    hisScore++;
            }
        }
    }    

    // get potential score
    double myBestShot = findBestLevel();
    their_tokens++;
    swapUser();
    updateLegalMovesAndEmptyCount();
    double hisBestShot = findBestLevel();
    swapUser();

    if(nextMove.x == 256)
        cout << "WAIT move:" << endl;
    cout << "basic: " << myScore << " " << hisScore;

    myScore += myBestShot * 0.7;

    // penalize his possible score by 
    hisScore += hisBestShot * 0.9;

    cout << "\t token: " << your_tokens << " " << their_tokens;
    cout << "\t bestLevel: " << myBestShot << " " << hisBestShot;
    cout << "\t final: " << myScore << " " << hisScore <<endl;

     
    score = myScore - hisScore;
    return myScore - hisScore; 
}

const int LEGAL = -1;
const int ILLEGAL = -2;

// so called legal, but it means the point that could belong to me.
void game_state::updateLegalTable(vector< vector< vector<int> > > &legalTable,
                     int x, int y, int z) {
    if(legalTable[x][y][z] < 0)
        return;

    // bottom layer short cut, base case/
    if(z == 0 && 
       (legalTable[x][y][z] == 0 || legalTable[x][y][z] == player_number)) {
        legalTable[x][y][z] = LEGAL;
        return;
    }
    else if (z == 0) {
        legalTable[x][y][z] = ILLEGAL;
        return;
    }

    // update all three lower level
    if(legalTable[x][y][z-1] >= 0) 
        updateLegalTable(legalTable, x, y, z-1);
    if(legalTable[x+1][y][z-1] >= 0) 
        updateLegalTable(legalTable, x+1, y, z-1);
    if(legalTable[x][y+1][z-1] >= 0) 
        updateLegalTable(legalTable, x, y+1, z-1);
   
    // update this level, base case 
    if(legalTable[x][y][z-1] == LEGAL &&
       legalTable[x+1][y][z-1] == LEGAL &&
       legalTable[x][y+1][z-1] == LEGAL &&
       (legalTable[x][y][z] == 0 || legalTable[x][y][z] == player_number))
        legalTable[x][y][z] = LEGAL;
    else
        legalTable[x][y][z] = ILLEGAL;

    // update empty count
}



void game_state::updateLegalMovesAndEmptyCount() {
    legal_moves.clear();
    vector< vector< vector<int> > > legalTable = board;
    for(unsigned int x = 0; x < board.size(); x++) {
        for(unsigned int y = 0; y < board[x].size(); y++) {
            for(unsigned int z = 0; z <board[x][y].size(); z++) {
                updateLegalTable(legalTable, x, y, z); 
                if(legalTable[x][y][z] == LEGAL 
                        && board[x][y][z] == 0
                        && z < your_tokens) 
                    legal_moves.push_back({x, y, z});
            }
        }
    }
}

void game_state::logInfo() {
    cout << "log into file" << endl;
    stringstream fileNameSStr;
    fileNameSStr << "log." << game_id << "." << moves_remaining;
    ofstream log;
    log.open(fileNameSStr.str().c_str());
    log << game_id << " ";
    log << opponent_id << " " 
        << your_points << " " 
        << their_points << " "
        << your_tokens << " " 
        << their_tokens << " ";
    for(int x = 0; x <board.size(); x++) {
        for(int y = 0; y < board[x].size(); y++) {
            for(int z = 0; z < board[x][y].size(); z++){
                if (board[x][y][z] == 0) {
                    log << 0 << " ";
                    continue;
                }
                log << (board[x][y][z] == player_number ? 1 : 2);
                log << " ";
            }
        }
    }
    log <<endl;
    log.close();

}

void game_state::logFinalScore() {
    stringstream fileNameSStr;
    fileNameSStr << "score." << game_id ;
    ofstream log;
    log.open(fileNameSStr.str().c_str());
    log << your_points - their_points;
    log << endl;
    log.close();
}
