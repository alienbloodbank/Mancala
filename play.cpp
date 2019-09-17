/* CSC 442: Intro to AI
 * Spring 2019
 * Project 1: Game Playing
 * Author: Soubhik Ghosh (netId: sghosh13)
 */

#include <bits/stdc++.h>
#include <unistd.h>

constexpr int PITS = 6;
constexpr int STONES = 3;

typedef enum class PlayerStrategy
{
        RANDOM,
	HUMAN,
	MINIMAX,
	ALPHABETA
} Player;

const std::map<std::string, Player> player_types = {
	{ "random", Player::RANDOM},
	{ "human", Player::HUMAN},
	{ "minimax", Player::MINIMAX},
	{ "alphabeta", Player::ALPHABETA}
};

std::pair<Player, Player> get_cmd_line_args(int argc, char **argv) {
	if (argc != 3) {
                std::cerr << "Duh this version of Mancala is a 2-player game. \nExiting..." << std::endl;
                exit(EXIT_FAILURE);
        }

	auto process_player = [](const char *argv, int arg_num) -> Player {
		std::string arg(argv);
		if ( player_types.find(arg) == player_types.end() ) {
                	std::cerr << "Invalid player" << arg_num + 1 << " strategy. \nExiting..." << std::endl;
                	exit(EXIT_FAILURE);
        	} else {
                	return player_types.at(arg);
        	}
	};

        return std::make_pair(process_player(argv[1], 0), process_player(argv[2], 1));
}

int main(int argc, char *argv[]) {
	auto [player1, player2] = get_cmd_line_args(argc, argv);

	std::cout << static_cast<int>(player1) << std::endl;
	std::cout << static_cast<int>(player2) << std::endl;
        return EXIT_SUCCESS;
}

