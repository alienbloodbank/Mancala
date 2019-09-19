/* CSC 442: Intro to AI
 * Spring 2019
 * Project 1: Game Playing
 * Author: Soubhik Ghosh (netId: sghosh13)
 */

#include <bits/stdc++.h>
#include <unistd.h>

// Put inside mancala namsespace
constexpr int PITS = 6;
constexpr int STONES = 4;

constexpr int TOTAL_PITS = (PITS + 1) << 1;

enum Turn {
	MAX = 0,
	MIN = 1
};

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

 /* High resolution timer class for measuring times */
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

namespace mancala {
	struct BoardState {
		int bins[TOTAL_PITS];
		BoardState() {
			// Initialize board with each bin having 4 stones
			std::fill_n (bins, TOTAL_PITS, STONES);
			// Initially both stores are empty
			bins[PITS] = bins[TOTAL_PITS - 1] = 0;
		}
	};

	bool result(BoardState &state, int move, Turn player_turn) {
		// Picked up all stones from the chosen pit
		int choosen_pit = move + (PITS + 1) * player_turn;
		int stones = state.bins[choosen_pit];
		state.bins[choosen_pit] = 0;

		// Distributing the stones in a counter-clockwise manner
		int i = choosen_pit;
		while(stones) {
			i = (i + 1) % TOTAL_PITS;
			// Skip if the bin is the opponent's store
			if (i == PITS + (PITS + 1) * (1 - player_turn)) continue;
			state.bins[i]++;
			--stones;			
		}

		// Check if last stone landed in player's store
		// In this case the player gets another turn
		
		if (i == PITS + (PITS + 1) * player_turn) {
			return true;
		}

		// Check if last stone landed in player's empty bin and opposite player's bin has stones
		// In this case all the stones in the opponent's bins go to the player's store
		if (i < (PITS + (PITS + 1) * player_turn) && 
			i >= (PITS + 1) * player_turn &&
			state.bins[i] == 1 && state.bins[PITS * 2 - i] > 0) {
			std::cerr << "Player" << player_turn + 1 << " captures them all!\n";
			state.bins[PITS + (PITS + 1) * player_turn] += state.bins[PITS * 2 - i];
			state.bins[PITS * 2 - i] = 0;
		}

		return false;
	}

	int utility(BoardState &state, Turn player_turn) {
		if (state.bins[PITS] > state.bins[PITS * 2 + 1]) {
			return player_turn == Turn::MAX? 1: -1;
		}
		else if(state.bins[PITS] < state.bins[PITS * 2 + 1]) {
			return player_turn == Turn::MIN? -1: 1;
		}
		else {
			return 0;
		}
	}

	inline bool terminal_test(BoardState &state) {
		// Check if either player1's bins or player2's bins are empty
		
		return std::all_of(state.bins, state.bins + PITS, [](int x){ return x == 0; }) ||
                std::all_of(state.bins + PITS + 1, state.bins + (PITS << 1) + 1, [](int x){ return x == 0; });
	}

	std::vector<int> get_turn_based_actions(BoardState &state, Turn player_turn) {
		std::vector<int> actions;
		for(int i = 0;i < PITS;i++) {
			if (state.bins[i + (PITS + 1) * player_turn])
				actions.push_back(i);	
		}
		return actions;
	}

	void display_board(BoardState &state, Turn player_turn) {
		std::cerr << "\n\nCurrent board contents:\n\n";
		
		std::cerr.fill(' ');
		std::cerr << "     ";
		for(size_t i = PITS;i > 0;i--){
			std::cerr << " " << std::setw(2) << i << "  ";
			if (i == 4) std::cerr << " ";
		}

		std::cerr.fill('0');
		std::cerr << "\n    --------------------------------    \n";
		std::cerr << " " << std::setw(2) << state.bins[PITS + (PITS + 1) * (1 - player_turn)] << " |";
		for(size_t i = PITS;i > 0;i--) {
			std::cerr << " " << std::setw(2) << state.bins[i - 1 + (PITS + 1) * (1 - player_turn)] << " |";
			if (i == 4) std::cerr << "|";
		}
		std::cerr << ((player_turn == Turn::MAX) ? " P2 \n": " P1 \n");
		std::cerr << "----------------------------------------\n";

		std::cerr << ((player_turn == Turn::MAX) ? " P1 |": " P2 |");
		for(size_t i = 0;i < PITS;i++) {
			std::cerr << " " << std::setw(2) << state.bins[i + (PITS + 1) * player_turn] << " |";
			if (i == 2) std::cerr << "|";
		}
		std::cerr << " " << std::setw(2) << state.bins[PITS + (PITS + 1) * player_turn];
		std::cerr << "\n    --------------------------------    \n";

		std::cerr.fill(' ');
		std::cerr << "     ";
		for(size_t i = 1;i < PITS + 1;i++) {
			std::cerr << " " << std::setw(2) << i << "  ";
			if (i == 3) std::cerr << " ";
		}

		std::cerr << "\n\n" << std::flush;

	}

}

/*
template <class GameT>
class GameTree {
private:
	struct Node {
		GameT state;
		int minmax_value;
		int alpha;
		int beta;
	};

	TURN current_turn;
	Node *current_node;
	Strategy max_turn_strategy;
	Strategy min_turn_strategy;

public:
	GameTree(Strategy strategy1, Strategy strategy2): max_turn_strategy(strategy1), min_turn_strategy(strategy2) {
		
	}

	~GameTree() {
		delete current_state;
	}

	ACTION actions(GameT *state) {
		current_state->
	}

	result() {

	}

	TURN player() const {
		return current_turn;
	}
};
*/

void evaluate(Strategy player, Turn player_turn, mancala::BoardState &current_state) {
	int move;
	bool is_turn = true;

	static std::random_device random_device;
  	static std::mt19937 engine{random_device()};

	std::cerr << "\nPlayer" << player_turn + 1 << " turn...\n\nPress enter to continue...";
        std::cin.ignore(32767, '\n');

	while (is_turn) {
		std::vector<int> legal_actions = mancala::get_turn_based_actions(current_state, player_turn);
	
		mancala::display_board(current_state, player_turn);

		switch(player) {
			case Strategy::HUMAN:
				while(true) {
					std::cerr << "\nEnter a move: ";
				
					if (!(std::cin >> move)){
						std::cin.clear();
						std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    					} else {
						std::cin.ignore(32767, '\n');
						move--;
						if(std::find(legal_actions.begin(), legal_actions.end(), move) != legal_actions.end()) {
							std::cerr << "Human choose: " << move + 1 << std::endl;
							break;
						}
					}
					std::cerr << "Illegal move" << std::endl;
				}
				break;
			case Strategy::RANDOM:
				{
					std::uniform_int_distribution<int> dist(0, legal_actions.size() - 1);
					move = legal_actions[dist(engine)];
					std::cerr << "Random choose: " << move + 1 << std::endl;
				}
				break;
			case Strategy::MINIMAX:
				std::cerr << "Minimax running...\n";
				// Use game tree
				break;
			case Strategy::ALPHABETA:
				std::cerr << "Alphabeta running...\n";
				// Use game tree
				break;
			default:
				break;
		}
		
		// current_state is updated as the player makes a move
		is_turn = mancala::result(current_state, move, player_turn);
		
//		mancala::display_board(current_state, player_turn);

		// Check if game is terminated
		if (terminal_test(current_state)) {
			int outcome = utility(current_state, player_turn);
			if(outcome == 1) {
				std::cout << "Player " << player_turn + 1 << " wins!" << std::endl;
			} else if(outcome == 0) {
				std::cout << "Draw." << std::endl;
			}
			exit(EXIT_SUCCESS);
		}

		if (is_turn) {
			std::cerr << "\nPlayer" << player_turn + 1 << " gets another turn...\n\nPress enter to continue...";
        		std::cin.ignore(32767, '\n');
		}
	}
}

std::pair<Strategy, Strategy> get_strategy_types(int argc, char **argv) {
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

int main(int argc, char *argv[]) {
	auto [strategy1, strategy2] = get_strategy_types(argc, argv);

	std::cout << "\n\n*************** WELCOME TO MANCALA ******************\n\n";
	std::cout << "Player1: \"" << argv[1] << "\" VS " << "Player2: \"" << argv[2] << "\"" << std::endl;	

//	std::cout << static_cast<int>(strategy1) << std::endl;
//        std::cout << static_cast<int>(strategy2) << std::endl;

	using mancala_board = mancala::BoardState; 
	mancala_board current_state;

	// Display initial state
//	mancala::display_board(current_state, Turn::MAX);

	// Game loop
	while (true) {
		// Player 1
		evaluate(strategy1, Turn::MAX, current_state);

		// Player 2
		evaluate(strategy2, Turn::MIN, current_state);
	
	}

        return EXIT_SUCCESS;
}

