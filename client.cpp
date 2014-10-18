#ifndef _MSC_VER
#include <unistd.h>
#define SLEEP(x) sleep(x)
#else
#include <windows.h>
#define SLEEP(x) Sleep(x)
#endif

#include "client.h"
#include "game_io/game_state.h"
#include "simulator.h"
#include "json_socket/json_socket.h"

void client::error(error_msg* err) {
    cout << "error: " << err->message << endl;
    throw GameError();
}

move_response* client::move(move_request* req) {
    cout << "time left: " << req->state->time_remaining_ns/1000000 << endl;
    Simulator simulator(*(req->state));
    board_point result = simulator.getBestMove();
    if (result.x == 256) {
        return new wait_response();
    }
    else {
        return new take_space_response(result);
    }
}

void client::server_greeting(greeting* greet) {
    cout << "Connected to server." << endl;
}

void client::game_over(game_result* r) {
    // left blank for you
}

void client::hand_done(move_result* r) {
    // left blank for you
    // TODO: update game state

}

void client::move_received(move_result* r) {
    // left blank for you
}

int main(int argc, char * argv[]) {
#ifdef _MSC_VER
	// Initialize Winsock
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 0), &wsaData);
#endif

    string server = "cuda.contest";
    string port = "9999";

    switch (argc) {
        case 3:
            port = argv[2];
        case 2:
            server = argv[1];
        case 1:
            break;
        default:
            cout << "Usage: " << argv[0] << " [host [port]]" << endl;
            return 0;
    }

    int wait = 10;
    for (;;) {
        try {
            json_socket contest_server = json_socket(server, "9999");

            client myclient = client();

            game_mediator game = game_mediator(&myclient, &contest_server);

            game.start();
        }
        catch (UnableToConnect) {
            cout << "Unable to connect. Waiting 10 seconds..." << endl;
            wait = 10;
        }
        catch (NotParseableJson) {
            cout << "Unparsable JSON encountered. Reconnecting..."
                 << endl;
            wait = 1;
        }
        catch (ConnectionError) {
            cout << "Unexpectedly unable to read from socket. Reconnecting..."
                 << endl;
            wait = 1;
        }
        catch (PeerDisconnected) {
            cout << "Server disconnected. Reconnecting..." << endl;
            wait = 1;
        }
        catch (GameError) {
            break;
        }
        SLEEP(wait);
    }

    return 0;
}
