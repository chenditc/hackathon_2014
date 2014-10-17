#include "game_result.h"

game_result::game_result(Json::Value result) {
    this->set_game_result(result);
}

void game_result::set_game_result(Json::Value result) {
    this->state = new game_state(result);
    this->winner = result["state"]["winner"].asInt();
}

game_result::~game_result() {
    delete this->state;
}
