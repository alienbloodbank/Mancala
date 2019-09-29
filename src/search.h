#ifndef SEARCH_H
#define SEARCH_H

// List of search functions and helpers
// Borrowed directly from the textbook

template <class T, class P>
struct Search {
	static inline int64_t minimax_nodes = 0;
       	static inline int64_t alphabeta_nodes = 0;
	
	// MINIMAX

	private:	
	static int8_t max_value(T& state, P player_turn, int depth)
	{
		if (state.terminal_test()) return state.utility(player_turn);
		if (state.cutoff_test(depth)) return state.eval(player_turn);

		std::vector<int> actions = state.actions(player_turn);

		int8_t v = -128, v1;
		for (auto a : actions) {
			T next_state(state);
			
			#ifdef BRANCHING_FACTOR
        			minimax_nodes++;
			#endif

			bool is_turn = next_state.result(a, player_turn);
			v1 = is_turn ? max_value(next_state, player_turn, depth) : min_value(next_state, player_turn, depth - 1);
			v = std::max(v, v1);
		}
		return v;
	}

	static int8_t min_value(T& state, P player_turn, int depth)
	{
		if (state.terminal_test()) return state.utility(player_turn);
                if (state.cutoff_test(depth)) return state.eval(player_turn);

		P opponent = static_cast<P>(1 - player_turn);
		std::vector<int> actions = state.actions(opponent);

		int8_t v = 126, v1;
		for (auto a : actions) {
			T next_state(state);
		
			#ifdef BRANCHING_FACTOR
                                minimax_nodes++;
                        #endif

			bool is_turn = next_state.result(a, opponent);
			v1 = is_turn ? min_value(next_state, player_turn, depth) : max_value(next_state, player_turn, depth - 1);
			v = std::min(v, v1);
		}
		return v;
	}

	public:
	static int minimax_decision(T& state, P player_turn, int depth)
	{
		std::vector<int> actions = state.actions(player_turn);
		
		minimax_nodes = 1;

		int8_t v = -128, v1;
		int m = -1;
		for (auto a : actions) {
			// Deepcopy state
			T next_state(state);

			#ifdef BRANCHING_FACTOR
                                minimax_nodes++;
                        #endif

			bool is_turn = next_state.result(a, player_turn);
			v1 = is_turn ? max_value(next_state, player_turn, depth) : min_value(next_state, player_turn, depth - 1);
			if (v1 > v) {
				v = v1;
				m = a;
			}
		}
		return m;
	}

	// ALPHABETA PRUNING
	private:
	static int8_t max_value(T& state, P player_turn, int depth, int8_t alpha, int8_t beta)
	{
                if (state.terminal_test()) return state.utility(player_turn);
		if (state.cutoff_test(depth)) return state.eval(player_turn);

		std::vector<int> actions = state.actions(player_turn);

		int8_t v = -128, v1;
		for (auto a : actions) {
			T next_state(state);
			
			#ifdef BRANCHING_FACTOR
                                alphabeta_nodes++;
                        #endif

			bool is_turn = next_state.result(a, player_turn);
			v1 = is_turn ? max_value(next_state, player_turn, depth, alpha, beta) : min_value(next_state, player_turn, depth - 1, alpha, beta);
			v = std::max(v, v1);
			if(v >= beta) return v;
			alpha = std::max(alpha, v);
		}
		return v;
	}

	static int8_t min_value(T& state, P player_turn, int depth, int8_t alpha, int8_t beta)
	{
                if (state.terminal_test()) return state.utility(player_turn);
		if (state.cutoff_test(depth)) return state.eval(player_turn);

		P opponent = static_cast<P>(1 - player_turn);		

		std::vector<int> actions = state.actions(opponent);

		int8_t v = 126, v1;
		for (auto a : actions) {
			T next_state(state);

			#ifdef BRANCHING_FACTOR
                                alphabeta_nodes++;
                        #endif

			bool is_turn = next_state.result(a, opponent);
			v1 = is_turn ? min_value(next_state, player_turn, depth, alpha, beta) : max_value(next_state, player_turn, depth - 1, alpha, beta);
			v = std::min(v, v1);
			if(v <= alpha) return v;
			beta = std::min(beta, v);
		}
		return v;
	}
	public:
	static int alphabeta_decision(T& state, P player_turn, int depth)
	{
                std::vector<int> actions = state.actions(player_turn);
		
		alphabeta_nodes = 1;

		int8_t alpha = -128, beta = 126;
		int8_t v = -128, v1;
		int m = -1;
		for (auto const a : actions) {
			T next_state(state);

			#ifdef BRANCHING_FACTOR
                                alphabeta_nodes++;
                        #endif

			bool is_turn = next_state.result(a, player_turn);
			v1 = is_turn ? max_value(next_state, player_turn, depth, alpha, beta) : min_value(next_state, player_turn, depth - 1, alpha, beta);
			if (v1 > v) {
				v = v1;
				m = a;
			}
			alpha = std::max(alpha, v);
		}
		return m;
	}
};

#endif
