#ifndef SEARCH_H
#define SEARCH_H

#include <bits/stdc++.h>

// MINIMAX

template <class T, class P>
struct Search {
//int8_t min_value(const T& state, Turn player_turn, int ply);
	static int8_t max_value(T& state, P player_turn, int ply)
	{
		if (ply == 0 || state.terminal_test()) return state.utility(player_turn);

		std::vector<int> actions = state.actions(player_turn);

		int8_t v = -128, v1;
		for (auto const a : actions) {
			T child_state(state);
			bool is_turn = child_state.result(a, player_turn);
			v1 = is_turn ? max_value(child_state, player_turn, ply) : min_value(child_state, static_cast<P>(1 - player_turn), ply - 1);
			v = std::max(v, v1);
		}
		return v;
	}

	static int8_t min_value(T& state, P player_turn, int ply)
	{
		if (ply == 0 || state.terminal_test()) return state.utility(player_turn);

		std::vector<int> actions = state.actions(player_turn);

		int8_t v = 126, v1;
		for (auto const a : actions) {
			T child_state(state);
			bool is_turn = child_state.result(a, player_turn);
			v1 = is_turn ? min_value(child_state, player_turn, ply) : max_value(child_state, static_cast<P>(1 - player_turn), ply - 1);
			v = std::min(v, v1);
		}
		return v;
	}

	static int minimax_decision(T& state, P player_turn, int ply)
	{
		std::vector<int> actions = state.actions(player_turn);

		int8_t v = -128, v1;
		int m = -1;
		for (auto const a : actions) {
			// Deepcopy state
			T child_state(state);
			bool is_turn = child_state.result(a, player_turn);
			v1 = is_turn ? max_value(child_state, player_turn, ply) : min_value(child_state, static_cast<P>(1 - player_turn), ply - 1);
			if (v1 > v) {
				v = v1;
				m = a;
			}
		}
		return m;
	}

	// ALPHABETA PRUNING

	static int8_t max_value(T& state, P player_turn, int ply, int8_t alpha, int8_t beta)
	{
		if (ply == 0 || state.terminal_test()) return state.utility(player_turn);

		std::vector<int> actions = state.actions(player_turn);

		int8_t v = -128, v1;
		for (auto const a : actions) {
			T child_state(state);
			bool is_turn = child_state.result(a, player_turn);
			v1 = is_turn ? max_value(child_state, player_turn, ply, alpha, beta) : min_value(child_state, static_cast<P>(1 - player_turn), ply - 1, alpha, beta);
			v = std::max(v, v1);
			if(v >= beta) return v;
			alpha = std::max(alpha, v);
		}
		return v;
	}

	static int8_t min_value(T& state, P player_turn, int ply, int8_t alpha, int8_t beta)
	{
		if (ply == 0 || state.terminal_test()) return state.utility(player_turn);

		std::vector<int> actions = state.actions(player_turn);

		int8_t v = 126, v1;
		for (auto const a : actions) {
			T child_state(state);
			bool is_turn = child_state.result(a, player_turn);
			v1 = is_turn ? min_value(child_state, player_turn, ply, alpha, beta) : max_value(child_state, static_cast<P>(1 - player_turn), ply - 1, alpha, beta);
			v = std::min(v, v1);
			if(v <= alpha) return v;
			beta = std::min(beta, v);
		}
		return v;
	}

	static int alphabeta_search(T& state, P player_turn, int ply)
	{
		std::vector<int> actions = state.actions(player_turn);

		int8_t alpha = -128, beta = 126;
		int8_t v = -128, v1;
		int m = -1;
		for (auto const a : actions) {
			// Deepcopy state
			T child_state(state);
			bool is_turn = child_state.result(a, player_turn);
			v1 = is_turn ? max_value(child_state, player_turn, ply, alpha, beta) : min_value(child_state, static_cast<P>(1 - player_turn), ply - 1, alpha, beta);
			if (v1 > v) {
				v = v1;
				m = a;
			}
			if (v >= beta) break;
			alpha = std::max(alpha, v);
		}
		return m;
	}
};

#endif
