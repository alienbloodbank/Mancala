#ifndef MANCALA_H
#define MANCALA_H

// Mancala Namespace with board states and methods

enum Turn {
	PLAYER1 = 0,
	PLAYER2 = 1
};

namespace mancala {
	// Hardcoded values of number of pits, stones
	constexpr int PITS = 6;
	constexpr int STONES_PER_PIT = 4;
	
	// Precomputed values to speedup computations
	constexpr int TOTAL_STONES = STONES_PER_PIT * (PITS << 1);
	constexpr int HALF_STONES = TOTAL_STONES >> 1;
	constexpr int BOARD_SIZE = (PITS + 1) << 1;
	constexpr int TOTAL_PITS = PITS << 1;

	// Actual mancala board's state class
	class BoardState {
	private:
		// Mancala board's data structure
		int8_t bins[BOARD_SIZE];
	public:
		BoardState() 
		{
			static_assert(sizeof(int8_t) == 1, "Must be 1 byte long");
			// Initialize board with each bin having 4 stones
			std::fill_n (bins, BOARD_SIZE, STONES_PER_PIT);
			// Initially both stores are empty
			bins[PITS] = bins[BOARD_SIZE - 1] = 0;
		}
		
		// copy constructor to deepcopy boards
		BoardState(const BoardState &state) 
		{
			memcpy(this->bins, state.bins, BOARD_SIZE * sizeof(int8_t));
		}

		bool result(int move, Turn player_turn);
		int8_t utility(Turn player_turn) const;

		// Heuristics
		
		// Difference between players store and oppponent store
		int8_t h1(Turn player_turn)
        	{
                	return bins[PITS + (PITS + 1) * player_turn] - bins[PITS + (PITS + 1) * (1 - player_turn)];

        	}
/*
		// Number of non-empty pits in player's row
                int8_t h2(Turn player_turn)
                {
			int8_t count = 0;
			int precomputed_limit = (PITS + 1) * player_turn;
			for(size_t i = 0;i < PITS;i++) {{
				count = count + !!bins[i + precomputed_limit];
			}
                        return count;
                }

		// Number of stones on player's board
                int8_t h3(Turn player_turn)
                {
               		int8_t count = 0;
			int precomputed_limit = (PITS + 1) * player_turn;
                	for(size_t i = 0;i < PITS;i++) {
                        	count += (bins[i + precomputed_limit]);
                	}
                	return count;
		}

		int8_t h4(Turn player_turn)
		{
			return h1(player_turn) + h2(player_turn) + 0.3 * (h3(player_turn));
		}
*/
		// Cutoff tests for heuristic minimax/alphabeta
		bool cutoff_test(int depth)
		{
			return (depth == 0) || (bins[PITS] > HALF_STONES) || (bins[BOARD_SIZE - 1] > HALF_STONES);
		}

		bool terminal_test();

		std::vector<int> actions(Turn player_turn);

		// Friend function to display the board
		friend std::ostream& operator<< (std::ostream &out, const BoardState &board);
	};

	// The transition model, which defines the result of the current player's move
	// This function changes the current board's contents after the move and returns a boolean
	// to tell if the player gets a free turn
	bool BoardState::result(int move, Turn player_turn) 
	{
		int precomputed_limit = (PITS + 1) * player_turn;

		// Picked up all stones from the chosen pit
		int i = move + precomputed_limit;
		int8_t stones = bins[i];
		bins[i] = 0;

		// Distributing the stones in a counter-clockwise manner
		int opponent_store = TOTAL_PITS + 1 - precomputed_limit;
		while(stones) {
			i = (i + 1) % BOARD_SIZE;
			// Skip if the bin is the opponent's store
			if (i == opponent_store) continue;
			bins[i]++;
			--stones;			
		}

		// Check if last stone landed in player's store
		// In this case the player gets another turn
		if (i == PITS + precomputed_limit) {
			return true;
		}

		// Check if last stone landed in player's empty bin and opposite player's bin has at least one stone
		// In this case all the stones in the opponent's bins go to the player's store
		if (i < (PITS + precomputed_limit) && 
			i >= precomputed_limit &&
			bins[i] == 1 && bins[TOTAL_PITS - i] > 0) {
			bins[PITS + precomputed_limit] += (bins[TOTAL_PITS - i] + 1);
			bins[TOTAL_PITS - i] = bins[i] = 0;
		}

		return false;
	}

	// The payoff function which is applied for a teminal state (win/loose/draw)
	// This function calculates the difference between the stores and the difference between
	// each player's side of the board
	inline int8_t BoardState::utility(Turn player_turn) const
	{
		int8_t diff = 0;		
		for(size_t i = 0;i <= PITS;i++) {
			diff += (bins[i + (PITS + 1) * player_turn] - bins[i + (PITS + 1) * (1 - player_turn)]);
		}
		return diff;
	}

	// A terminal test, which is true when the game is over and false otherwise
	bool BoardState::terminal_test()
	{
		// Check if either player1's bins or player2's bins are empty
		for(int8_t *ptr = bins; ptr < bins + PITS;++ptr) {
			if (*ptr) {
				for(ptr = bins + PITS + 1; ptr < bins + TOTAL_PITS + 1;++ptr) {
                        		if (*ptr) 
    						return false;
                		}
				return true;
			}
		}
		return true;
	}

	// Returns the set of legal moves in a state.
	// This function returns a list of bin numbers (0-index) having at least one stone
	std::vector<int> BoardState::actions(Turn player_turn)
	{
	        int precomputed_limit = (PITS + 1) * player_turn;
		std::vector<int> moves;
		for(size_t i = 0;i < PITS;i++) {
			if (bins[i + precomputed_limit]) {
				moves.push_back(i);
			}
		}
		return moves;
	}

	std::ostream& operator<< (std::ostream &err, const BoardState &board)
	{
		err << "\n\nCurrent board contents:\n\n";

		err.fill(' ');
		err << "     ";
		for(size_t i = PITS;i > 0;i--){
			err << " " << std::setw(2) << i << "  ";
			if (i == 4) err << " ";
		}

		err.fill('0');
		err << "\n    --------------------------------    \n";
		err << " " << std::setw(2) << int(board.bins[PITS + (PITS + 1)]) << " |";
		for(size_t i = PITS;i > 0;i--) {
			err << " " << std::setw(2) << int(board.bins[i - 1 + (PITS + 1)]) << " |";
			if (i == 4) err << "|";
		}
		err << " P2 \n";
		err << "----------------------------------------\n";

		err << " P1 |";
		for(size_t i = 0;i < PITS;i++) {
			err << " " << std::setw(2) << int(board.bins[i]) << " |";
			if (i == 2) err << "|";
		}
		err << " " << std::setw(2) << int(board.bins[PITS]);
		err << "\n    --------------------------------    \n";

		err.fill(' ');
		err << "     ";
		for(size_t i = 1;i < PITS + 1;i++) {
			err << " " << std::setw(2) << i << "  ";
			if (i == 3) err << " ";
		}

		err << "\n\n" << std::flush;

		return err;
	}
}

#endif
