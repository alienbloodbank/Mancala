#ifndef MANCALA_H
#define MANCALA_H

#include <bits/stdc++.h>

enum Turn {
	PLAYER1 = 0,
	PLAYER2 = 1
};

namespace mancala {
	constexpr int PITS = 6;
	constexpr int STONES_PER_PIT = 4;
	constexpr int TOTAL_STONES = STONES_PER_PIT * (PITS << 1);
	constexpr int HALF_STONES = TOTAL_STONES >> 1;

	constexpr int BOARD_SIZE = (PITS + 1) << 1;
	constexpr int TOTAL_PITS = PITS << 1;

	class BoardState {
	private:
		int8_t bins[BOARD_SIZE];
	public:
		BoardState() 
		{
			// Initialize board with each bin having 4 stones
			std::fill_n (bins, BOARD_SIZE, STONES_PER_PIT);
			// Initially both stores are empty
			bins[PITS] = bins[BOARD_SIZE - 1] = 0;
		}
		
		// copy constructor
		BoardState(const BoardState &state) 
		{
			memcpy(this->bins, state.bins, BOARD_SIZE * sizeof(int8_t));
		}

		bool result(int move, Turn player_turn);
		int8_t utility(Turn player_turn) const;

		int8_t h1(Turn player_turn)
		{
			return bins[PITS + (PITS + 1) * player_turn] - bins[PITS + (PITS + 1) * (1 - player_turn)];
		}

		int8_t h2(Turn player_turn)
		{
			int8_t val = bins[PITS + (PITS + 1) * player_turn] - bins[PITS + (PITS + 1) * (1 - player_turn)];
			int8_t rem = 0;
			for(size_t i = 0;i < PITS;i++) {
				rem += (bins[i] + bins[i + (PITS + 1)]);
			}
			if(val > 0) {
				val += (rem >> 1);
			} else if (val < 0) {
				val -= (rem >> 1);
			}
			return val;
		}

		bool cutoff_test1(int depth)
		{
			return depth == 0;
		}

		bool cutoff_test2(int depth)
		{
			return (bins[PITS] > HALF_STONES) || (bins[BOARD_SIZE - 1] > HALF_STONES);
		}

		bool terminal_test();
		
		std::vector<int> actions(Turn player_turn);
		void display(Turn player_turn) const;
	};

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

		// Check if last stone landed in player's empty bin and opposite player's bin has stones
		// In this case all the stones in the opponent's bins go to the player's store
		if (i < (PITS + precomputed_limit) && 
			i >= precomputed_limit &&
			bins[i] == 1 && bins[TOTAL_PITS - i] > 0) {
			bins[PITS + precomputed_limit] += (bins[TOTAL_PITS - i] + 1);
			bins[TOTAL_PITS - i] = bins[i] = 0;
		}

		return false;
	}

	inline int8_t BoardState::utility(Turn player_turn) const
	{
		// (my_mancala – your_mancala) + (stones_on_my_side – stones_on_your_side)
		int8_t diff = 0;		
		for(size_t i = 0;i <= PITS;i++) {
			diff += (bins[i + (PITS + 1) * player_turn] - bins[i + (PITS + 1) * (1 - player_turn)]);
		}
		return diff;
	}

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

	inline std::vector<int> BoardState::actions(Turn player_turn)
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

	void BoardState::display(Turn player_turn) const 
	{
		player_turn = Turn::PLAYER1;
		std::cerr << "\n\nCurrent board contents:\n\n";
		
		std::cerr.fill(' ');
		std::cerr << "     ";
		for(size_t i = PITS;i > 0;i--){
			std::cerr << " " << std::setw(2) << i << "  ";
			if (i == 4) std::cerr << " ";
		}

		std::cerr.fill('0');
		std::cerr << "\n    --------------------------------    \n";
		std::cerr << " " << std::setw(2) << int(bins[PITS + (PITS + 1) * (1 - player_turn)]) << " |";
		for(size_t i = PITS;i > 0;i--) {
			std::cerr << " " << std::setw(2) << int(bins[i - 1 + (PITS + 1) * (1 - player_turn)]) << " |";
			if (i == 4) std::cerr << "|";
		}
		std::cerr << ((player_turn == Turn::PLAYER1) ? " P2 \n": " P1 \n");
		std::cerr << "----------------------------------------\n";

		std::cerr << ((player_turn == Turn::PLAYER1) ? " P1 |": " P2 |");
		for(size_t i = 0;i < PITS;i++) {
			std::cerr << " " << std::setw(2) << int(bins[i + (PITS + 1) * player_turn]) << " |";
			if (i == 2) std::cerr << "|";
		}
		std::cerr << " " << std::setw(2) << int(bins[PITS + (PITS + 1) * player_turn]);
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

#endif
