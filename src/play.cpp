/* CSC 442: Intro to AI
 * Spring 2019
 * Project 1: Game Playing
 * Authors: Soubhik Ghosh (netId: sghosh13), Rich Magnotti
 */

#include <unistd.h>
#include <bits/stdc++.h>

#include "mancala.h"
#include "search.h"

#undef BRANCHING_FACTOR
#undef PIE_RULE

#define PRESS_ENTER std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
// #undef PRESS_ENTER

constexpr int CUTOFF_DEPTH_MINIMAX = 8;
constexpr int CUTOFF_DEPTH_ALPHABETA = 8;

using namespace mancala;

// Player 1 outcome
enum Outcome
{
	WIN = 1,
	LOSS = 2,
	DRAW = 3
};

// List of valid strategies
enum class Strategy
{
        RANDOM,
	HUMAN,
	MINIMAX,
	ALPHABETA
};

const std::map<std::string, Strategy> strategy_types = {
	{ "random", Strategy::RANDOM},
	{ "human", Strategy::HUMAN},
	{ "minimax", Strategy::MINIMAX},
	{ "alphabeta", Strategy::ALPHABETA}
};

// High resolution timer class for measuring times
class Timer
{
private:
	using hr_clock_t = std::chrono::high_resolution_clock;
	using millis_t = std::chrono::duration<double, std::milli>;

	std::chrono::time_point<hr_clock_t> m_beg;

public:
	Timer() : m_beg(hr_clock_t::now()) {}

	double elapsed() const {
		return std::chrono::duration_cast<millis_t>(hr_clock_t::now() - m_beg).count();
	}
};

// Current player's turn to make a move
bool evaluate(Strategy player, Turn player_turn, BoardState &current_state) 
{
	int move;
	bool is_free_turn = true;

	static std::random_device random_device;
  	static std::mt19937 engine{random_device()};

	std::cerr << "\nP" << player_turn + 1 << " turn...\n\nPress enter to continue...";
        //PRESS_ENTER

	// Free turn loop
	do {
		// Get list of valid player actions on the current board
		std::vector<int> actions = current_state.actions(player_turn);
	
		std::cerr << current_state;

		// Decide the player's strategy: (human, random, minimax, alphabeta)
		switch(player) {
			case Strategy::HUMAN:
				// Illegal move loop
				while (true) {
					std::cerr << "\nEnter a move: ";
				
					// Check if move is an integer
					if (!(std::cin >> move)) {
						std::cin.clear();
    						PRESS_ENTER
					} else {
						PRESS_ENTER
						move--;
						
						// Check if move is a valid action
						if (std::find(actions.begin(), actions.end(), move) != actions.end()) {
							std::cerr << "Human choose: " << move + 1 << std::endl;
							break;
						}
					}
					std::cerr << "Illegal move" << std::endl;
				}
				break;
			case Strategy::RANDOM:
				{
					std::uniform_int_distribution<int> dist(0, actions.size() - 1);
					move = actions[dist(engine)];
					std::cerr << "Random choose: " << move + 1 << std::endl;
				}
				break;
			case Strategy::MINIMAX:
				{
					std::cerr << "Minimax running...\n";
					Timer minimax_time;
					move = Search<BoardState, Turn>::minimax_decision(current_state, player_turn, CUTOFF_DEPTH_MINIMAX);
					std::cerr << "Minimax choose: " << move + 1 << std::endl;
					std::cerr << "Elapsed time: " << minimax_time.elapsed() << " ms" << std::endl;
					#ifdef BRANCHING_FACTOR
						std::cerr << "Minimax explored nodes: " << Search<BoardState, Turn>::minimax_nodes << std::endl;
					#endif
				}
				break;
			case Strategy::ALPHABETA:
				{
					std::cerr << "Alphabeta running...\n";
					Timer alphabeta_time;
					move = Search<BoardState, Turn>::alphabeta_decision(current_state, player_turn, CUTOFF_DEPTH_ALPHABETA);
					std::cerr << "Alphabeta choose: " << move + 1 << std::endl;
					std::cerr << "Elapsed time: " << alphabeta_time.elapsed() << " ms" << std::endl;
					#ifdef BRANCHING_FACTOR
                                                std::cerr << "Alphabeta explored nodes: " << Search<BoardState, Turn>::alphabeta_nodes << std::endl;
                                        #endif
				}
				break;
			default:
				break;
		}

		// Board is updated as the player makes a move
		is_free_turn = current_state.result(move, player_turn);

		// Display the updated board
		std::cerr << current_state;

		// Check if game is terminated
		if (current_state.terminal_test()) {
			return true;
		}

		// Prompt for a free turn
		if (is_free_turn) {
			std::cerr << "\nP" << player_turn + 1 << " gets another turn...\n\nPress enter to continue...";
			//PRESS_ENTER
		} else {
			// TODO: print the capture state here		
		}
	} while(is_free_turn);

	return false;
}

// Driver function containing the game loop
Outcome run_game(Strategy strategy1, Strategy strategy2)
{
	// Initialize the board
        BoardState current_state;

#ifdef PIE_RULE
        // Player 1
        evaluate(strategy1, Turn::PLAYER1, current_state);

        // Switiching sides
        std::swap(strategy1, strategy2);

        std::cout << "Sides are switched!" << std::endl;

        // Player 2
        evaluate(strategy2, Turn::PLAYER2, current_state);
#endif

	bool is_terminated = false;
	Turn current_player;
        // Game loop
        while (true) {
                // Player 1
		
		current_player = Turn::PLAYER1;
                is_terminated = evaluate(strategy1, current_player, current_state);

		if (is_terminated) {
			break;
		}

                // Player 2
		current_player = Turn::PLAYER2;
                is_terminated = evaluate(strategy2, current_player, current_state);

		if (is_terminated) {
			break;
		}
        }

	int outcome = current_state.utility(current_player);

	if (outcome > 0) {
		std::cout << "Player " << current_player + 1 << " wins!" << std::endl;
		return current_player == 0 ? Outcome::WIN : Outcome::LOSS;
	} else if (outcome < 0) { // Current player looses
		int opponent = (current_player == 0 ? 2 : 1);
		std::cout << "Player " << opponent << " wins!" << std::endl;
		return current_player == 0 ? Outcome::LOSS : Outcome::WIN;
	} else {
		std::cout << "Draw." << std::endl;
		return Outcome::DRAW;
	}
}

extern "C" {
    	// Python wrapper
	int run_game_wrapper(int a, int b){ 
	    	return run_game(static_cast<Strategy>(a), static_cast<Strategy>(b)); 
    	}
}

// Get and verify the command line arguments for valid strategies
std::pair<Strategy, Strategy> get_strategy_types(int argc, char **argv)
{
        if (argc != 3) {
                std::cerr << "Duh this version of Mancala is a 2-player game. \nExiting..." << std::endl;
                exit(EXIT_FAILURE);
        }

        std::string arg1(argv[1]), arg2(argv[2]);
        bool is_strategy1_valid = strategy_types.find(arg1) != strategy_types.end();
        bool is_strategy2_valid = strategy_types.find(arg2) != strategy_types.end();

        if (!is_strategy1_valid && !is_strategy2_valid) {
                std::cerr << "Invalid player strategies. \nTry Again...\n" << std::endl;
                exit(EXIT_FAILURE);
        } else if (!is_strategy1_valid) {
                std::cerr << "Invalid player1 strategy. \nTry Again..." << std::endl;
                exit(EXIT_FAILURE);
        } else if (!is_strategy2_valid) {
                std::cerr << "Invalid player2 strategy. \nTry Again..." << std::endl;
                exit(EXIT_FAILURE);
        }

        return std::make_pair(strategy_types.at(arg1), strategy_types.at(arg2));
}

int main(int argc, char *argv[]) 
{
	auto [strategy1, strategy2] = get_strategy_types(argc, argv);

	std::cout << "\n\n*************** WELCOME TO MANCALA ******************\n\n";
	std::cout << "P1: \"" << argv[1] << "\" VS " << "P2: \"" << argv[2] << "\"" << std::endl;	

	run_game(strategy1, strategy2);
		
        return EXIT_SUCCESS;
}

