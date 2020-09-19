#include <iostream>
#include <conio.h>
#include <algorithm>
#include <unordered_map>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

typedef vector<vector<string>> Board;

int n = 8;

const string file_name = "C:\\Users\\lsqua\\source\\repos\\Chess\\BoardFile.txt";
const string start_board = "@";

const int max_depth = 5;
int num_move = 20;

void test_knights_move();
void test_rooks_move();

vector<pair<int, int>> queen_moves{ {-1, 1}, {-1, -1}, {1, -1}, {1, 1}, {-1, 0}, {0, 1}, {1, 0}, {0, -1} };

struct Situation {
	Board board;
	char color;
	int depth;
};

struct Hash_Situation {
	size_t operator()(const Situation& situation) const {
		string s = "";
		for (vector<string> row : situation.board) {
			for (string cell : row) {
				s += cell;
				s += " ";
			}
		}
		s += situation.color;
		s += " ";
		s += to_string(situation.depth);
		return hash<string> {}(s);
	}
};

size_t hash_board(const Board& board) {
	string s = "";
	for (vector<string> row : board) {
		for (string cell : row) {
			s += cell;
			s += " ";
		}
	}



	return hash<string> {}(s);
}



bool operator ==(const Situation& situation1, const Situation& situation2) {
	if (situation1.color != situation2.color) {
		return false;

	}

	if (situation1.depth != situation2.depth) {
		return false;
	}

	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col++) {
			if (situation1.board[row][col] != situation2.board[row][col]) {
				return false;
			}
		}
	}
	return true;
}

unordered_map<Situation, int, Hash_Situation> situation_best_score;
vector<Board> get_all_moves(const Board& board, char color, int num_move);

int get_points(const Board& board);

vector<vector<int>> place_value{
	{1, 1, 1, 1, 1, 1, 1, 1},
	{1, 2, 2, 2, 2, 2, 2, 1},
	{1, 2, 3, 3, 3, 3, 2, 1},
	{1, 2, 3, 5, 5, 3, 2, 1},
	{1, 2, 3, 5, 5, 3, 2, 1},
	{1, 2, 3, 3, 3, 3, 2, 1},
	{1, 2, 2, 2, 2, 2, 2, 1},
	{1, 1, 1, 1, 1, 1, 1, 1}
};





unordered_map<string, int> points{
	{"wR", 5}, {"bR", -5},
	{"wN", 3}, {"bN", -3},
	{"wB", 3}, {"bB", -3},
	{"wQ", 9}, {"bQ", -9},
	{"wK", 1000}, {"bK", -1000},
	{"wP", 1}, {"bP", -1},
	{"", 0}
};


Board get_move(const Board& board, pair<int, int> pos, pair<int, int> new_pos);

bool check_empty(const Board& board, pair<int, int> pos);
vector<Board> black_pawn(const Board& board, pair<int, int> pawn_pos);

void print_board(const Board& board) {
	int index = 1;
	cout << "Score = " << get_points(board) << "\n\n";
	cout << "Indentifier = " << hash_board(board) << "\n\n";
	for (vector<string> row : board) {
		cout << (9 - index) << " ";
		cout << " +--+--+--+--+--+--+--+--+\n   ";
		index++;
		for (string cell : row) {

			cout << "|" << ((cell.empty()) ? "  " : cell);
		}
		cout << "|\n";
	}
	cout << "   +--+--+--+--+--+--+--+--+\n   ";
	cout << "  ";
	for (char ch = 'a'; ch <= 'h'; ch++) {
		cout << ch << "  ";
	}
	cout << "\n------------------\n";
}

pair<int, int> operator +(const pair<int, int>& loc1, const pair<int, int>& loc2) {
	return pair<int, int>{loc1.first + loc2.first, loc1.second + loc2.second};
}

bool check_in(pair<int, int> pos) {
	return (pos.first < 8 && pos.first >= 0 && pos.second < 8 && pos.second >= 0);
}

bool check_valid(char color, const Board& board, pair<int, int> loc) {

	return (board[loc.first][loc.second].empty() || board[loc.first][loc.second][0] != color);
}

bool check_valid_take(pair<int, int> pos, const Board& board, pair<int, int> new_pos) {
	return (check_in(new_pos)
		&& !board[new_pos.first][new_pos.second].empty()
		&& board[pos.first][pos.second][0] != board[new_pos.first][new_pos.second][0]);
}

void add_board(const Board& board) {
	ofstream board_file;
	board_file.open(file_name, std::ios_base::app);
	board_file << start_board << hash_board(board) << "\n";
	string s = "";
	for (vector<string> ve : board) {
		for (string st : ve) {
			s += (st.empty()) ? ".." : st;
		}
		s += "\n";
	}
	s += "\n";
	board_file << s;
	board_file.close();
}
Board create_empty_board() {
	vector<string> tmp(8, "");
	return Board(8, tmp);
}

Board load_board(string name) {
	Board board;
	ifstream board_file;
	board_file.open(file_name);
	string s = "";
	while (board_file >> s && s != name) {}


	for (int row = 0; row < 8; row++) {
		board_file >> s;
		cout << s << "\n";
		vector<string> one_row;
		for (int col = 0; col < 16; col += 2) {
			string st = "";
			st += s[col];
			st += s[col + 1];
			if (st == "..") {
				st = "";
			}
			one_row.push_back(st);
		}
		board.push_back(one_row);
	}
	board_file.close();
	return board;
}




vector<Board> king_knights_move(const Board& board, pair<int, int> pos, vector<pair<int, int>> moves) {


	vector<Board> next_boards;

	char color = board[pos.first][pos.second][0];

	for (pair<int, int> move : moves) {
		pair<int, int> loc = pos + move;
		if (!check_in(loc)) {
			continue;
		}
		if (!check_valid(color, board, loc)) {
			continue;
		}
		Board new_board = board;
		new_board[loc.first][loc.second] = board[pos.first][pos.second];
		new_board[pos.first][pos.second] = "";
		next_boards.push_back(new_board);
	}
	return next_boards;
}


vector<Board> white_pawn_move(const Board& board, pair<int, int> pawn_pos) {
	vector<Board> moves;
	if (check_empty(board, pawn_pos + pair<int, int>{-1, 0})) {
		moves.push_back(get_move(board, pawn_pos, pawn_pos + pair<int, int>{-1, 0}));
		if (pawn_pos.first == 6 && check_empty(board, pawn_pos + pair<int, int>{-2, 0})) {
			moves.push_back(get_move(board, pawn_pos, pawn_pos + pair<int, int>{-2, 0}));
		}
	}
	if (check_valid_take(pawn_pos, board, pawn_pos + pair<int, int>{-1, -1})) {
		moves.push_back(get_move(board, pawn_pos, pawn_pos + pair<int, int>{-1, -1}));
	}
	if (check_valid_take(pawn_pos, board, pawn_pos + pair<int, int>{-1, 1})) {
		moves.push_back(get_move(board, pawn_pos, pawn_pos + pair<int, int>{-1, 1}));
	}

	return moves;
}

vector<Board> knights_move(const Board& board, pair<int, int> knight_pos) {

	vector<pair<int, int>> moves_for_knight{ {-1, 2}, {1, 2}, {2, 1}, {2, -1}, {1, -2}, {-1, -2}, {-2, -1}, {-2, 1} };
	return king_knights_move(board, knight_pos, moves_for_knight);
}

bool check_empty(const Board& board, pair<int, int> pos) {

	return (check_in(pos) && board[pos.first][pos.second].empty());
}

Board get_move(const Board& board, pair<int, int> pos, pair<int, int> new_pos) {
	Board new_board = board;
	new_board[new_pos.first][new_pos.second] = board[pos.first][pos.second];
	new_board[pos.first][pos.second] = "";
	return new_board;
}

vector<Board> alls_move(const Board& board, vector<pair<int, int>> moves, pair<int, int> pos) {
	char color = board[pos.first][pos.second][0];
	vector<Board> boards;

	for (pair<int, int> move : moves) {
		pair<int, int> loc = pos;
		while (check_empty(board, loc + move)) {
			loc = move + loc;
			boards.push_back(get_move(board, pos, loc));
		}
		if (check_valid_take(pos, board, loc + move)) {
			boards.push_back(get_move(board, pos, loc + move));
		}
	}
	return boards;
}


vector<Board> rooks_move(const Board& board, pair<int, int> rook_pos) {
	vector<pair<int, int>> moves{ {-1, 0}, {0, 1}, {1, 0}, {0, -1} };
	return alls_move(board, moves, rook_pos);
}

vector<Board> bishop_move(const Board& board, pair<int, int> bishop_pos) {
	vector<pair<int, int>> moves{ {-1, 1}, {-1, -1}, {1, -1}, {1, 1} };
	return alls_move(board, moves, bishop_pos);
}



vector<Board> queen_move(const Board& board, pair<int, int> queen_pos) {

	return alls_move(board, queen_moves, queen_pos);
}

vector<Board> king_move(const Board& board, pair<int, int> king_pos)
{
	return king_knights_move(board, king_pos, queen_moves);
}
vector<Board> promote(const Board& board) {

	vector<string> promotions{ "R", "N", "B", "Q" };
	vector<Board> moves;
	for (int i = 0; i < 8; i++) {
		if (board[7][i] == "bP") {
			for (string st : promotions) {
				Board new_board = board;
				new_board[7][i] = "b" + st;
				moves.push_back(new_board);
			}
		}
		if (board[0][i] == "wP") {
			for (string st : promotions) {
				Board new_board = board;
				new_board[0][i] = "w" + st;
				moves.push_back(new_board);
			}
		}
	}
	return  moves;
}
vector<Board> move_pawn(const Board& board, pair<int, int> pawn_pos) {
	vector<Board> moves;


	if (board[pawn_pos.first][pawn_pos.second][0] == 'w') {
		moves = white_pawn_move(board, pawn_pos);
	}
	else {
		moves = black_pawn(board, pawn_pos);
	}

	vector<Board> final_moves;

	for (Board b : moves) {
		vector<Board> promoted_movees = promote(b);

		if (promoted_movees.empty()) {
			final_moves.push_back(b);
		}
		else {
			final_moves.insert(final_moves.end(), promoted_movees.begin(), promoted_movees.end());
		}
	}

	return final_moves;
}


vector<Board> black_pawn(const Board& board, pair<int, int> pawn_pos) {
	vector<Board> moves;

	if (check_empty(board, pawn_pos + pair<int, int> {1, 0})) {
		moves.push_back(get_move(board, pawn_pos, pawn_pos + pair<int, int> {1, 0}));
		if (pawn_pos.first == 1 && check_empty(board, pawn_pos + pair<int, int> {2, 0})) {
			moves.push_back(get_move(board, pawn_pos, pawn_pos + pair<int, int> {2, 0}));
		}
	}
	if (check_valid_take(pawn_pos, board, pawn_pos + pair<int, int>{1, -1})) {
		moves.push_back(get_move(board, pawn_pos, pawn_pos + pair<int, int>{1, -1}));
	}
	if (check_valid_take(pawn_pos, board, pawn_pos + pair<int, int>{1, 1})) {
		moves.push_back(get_move(board, pawn_pos, pawn_pos + pair<int, int>{1, 1}));
	}
	return moves;
}

vector<Board> get_moves_piece(pair<int, int> pos, const Board& board, char color) {
	string piece = board[pos.first][pos.second];

	if (piece.empty() || piece[0] != color) {
		return {};
	}
	if (piece[1] == 'P') {
		return move_pawn(board, pos);
	}

	if (piece[1] == 'N') {
		return knights_move(board, pos);
	}
	if (piece[1] == 'B') {
		return bishop_move(board, pos);
	}
	if (piece[1] == 'R') {
		return rooks_move(board, pos);

	}
	if (piece[1] == 'K') {
		return king_move(board, pos);
	}
	if (piece[1] == 'Q') {
		return queen_move(board, pos);
	}

}

bool operator <(const Board& board1, const Board& board2) {
	return (get_points(board1) < get_points(board2));
}

vector<Board> get_all_moves(const Board& board, char color) {
	return get_all_moves(board, color, num_move);
}

vector<Board> get_all_moves(const Board& board, char color, int max_num_move) {
	vector<Board> moves;
	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col++) {
			vector<Board> piece_moves = get_moves_piece(pair<int, int>{row, col}, board, color);
			moves.insert(moves.end(), piece_moves.begin(), piece_moves.end());
		}
	}



	sort(moves.begin(), moves.end(),
		[color](const Board& a, const Board& b) {
			if (color == 'w') {
				return get_points(a) > get_points(b);
			}
			return get_points(a) < get_points(b); });


	if (max_num_move == -1 || max_num_move >= moves.size()) {
		return moves;
	}
	vector<Board> best_moves;
	for (int i = 0; i < moves.size() && i < max_num_move; i++) {
		best_moves.push_back(moves[i]);
	}
	return best_moves;
}

unordered_map<char, int> ratio{
	{'K', -1},
	{'Q', 0},
	{'R', 0},
	{'B', 1},
	{'N', 1},
	{'P', 2}
};

int get_points(const Board& board) {
	int score = 0;

	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col++) {
			score += points[board[row][col]];
		}
	}

	int location_score = 0;;
	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col++) {
			if (board[row][col].empty()) {
				continue;
			}
			int t = ratio[board[row][col][1]];
			location_score += t * ((board[row][col][0] == 'w') ? place_value[row][col] : place_value[row][col] - place_value[row][col] * 2);
		}
	}

	score *= 500;
	score += location_score;
	return score;
}

Board humans_move(const Board& board) {

	string pos1;
	string pos2;

	vector<Board> moves = get_all_moves(board, 'b', -1);
	cout << "all possible moves for black = " << moves.size() << "\n";
	/*
	for (const Board move : moves) {
		print_board(move);
	}*/
	Board new_board;
	while (true) {

		cout << "\n\n--------------- please move ------------------\n";
		print_board(board);

		cout << "\nEnter location:";
		cin >> pos1;
		cin >> pos2;
		cout << "\nAre you sure:";
		char y_n;
		cin >> y_n;
		if (y_n != 'y') {
			continue;
		}


		pair<int, int> board_pos_1{ 7 - (pos1[1] - '1'),  pos1[0] - 'a' };
		pair<int, int> board_pos_2{ 7 - (pos2[1] - '1'), pos2[0] - 'a' };

		if (!check_in(board_pos_1) || !check_in(board_pos_2)) {
			cout << "Invalid move \n";
			continue;
		}

		new_board = get_move(board, board_pos_1, board_pos_2);
		print_board(new_board);
		if (find(moves.begin(), moves.end(), new_board) != moves.end()) {
			print_board(new_board);
			return new_board;
		}
		cout << "Invalid move\n";

	}
	return new_board;
}

Board random_move(const Board& board, char color) {
	vector<Board> moves = get_all_moves(board, color);
	int index = rand() % moves.size();
	return moves[index];
}
string get_notation(const Board& board1, const Board& board2) {
	string pos1 = "";
	string pos2 = "";
	for (int row = 0; row < board2.size(); row++) {
		for (int col = 0; col < board2[0].size(); col++) {
			if (board1[row][col] != board2[row][col]) {
				if (board1[row][col].empty()) {
					pos2 = col + 'a';
					pos2 += 7 - row + '1';
				}
				else {
					pos1 = col + 'a';
					pos1 += 7 - row + '1';
				}
			}
		}
	}
	return pos1 + pos2;
}

Board smart_move(const Board& board) {
	vector<Board> moves = get_all_moves(board, 'w');
	Board max_board = board;
	int score = -100000;
	for (Board& new_board : moves) {
		int tmp = get_points(new_board);
		if (tmp > score) {
			score = tmp;
			max_board = new_board;
		}
	}
	return max_board;
}

bool check_king(char color, const Board& board) {
	
	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col++) {
			if (!board[row][col].empty() && board[row][col][0] == color && board[row][col][1] == 'K') {
				return true;
			}
		}
	}
	return false;
}

int move_smart_score(const Board& board, int level, char color, int current_best_score_white, int current_best_score_black) {

	Situation situation{ board, color, level };

	if (situation_best_score.find(situation) != situation_best_score.end()) {
		return situation_best_score[situation];
	}


	if (level >= max_depth) {
		return get_points(board);
	}
	int max_move = num_move;
	
	vector<Board> moves = get_all_moves(board, color, max_move);
	int best_score = (color == 'w') ? numeric_limits<int>::min() : numeric_limits<int>::max();

	char new_color = (color == 'w') ? 'b' : 'w';
	for (Board move : moves) {
		if (!check_king(new_color, move)) {
			return (color == 'w') ? numeric_limits<int>::max() : numeric_limits<int>::min();
		}
		int score;
		if (color == 'w') {
			score = move_smart_score(move, level + 1, new_color, max(current_best_score_white, best_score), current_best_score_black);
		}
		else {
			score = move_smart_score(move, level + 1, new_color, current_best_score_white, min(current_best_score_black, best_score));
		}


		if (score <= current_best_score_white && color == 'b') {
			return numeric_limits<int>::min();
		}

		if (score >= current_best_score_black && color == 'w') {
			return numeric_limits<int>::max();
		}


		if (score > best_score && color == 'w') {
			best_score = score;
		}
		if (score < best_score && color == 'b') {
			best_score = score;
		}
	}

	situation_best_score[situation] = best_score;
	return best_score;
}

Board computer_play(const Board& board) {
	int best_score = numeric_limits<int>::min();
	Board best_board = board;

	vector<Board> moves = get_all_moves(board, 'w', num_move);
	/*
	for (Board move : moves) {
		print_board(move);
	}*/

	for (Board move : moves) {
		int score = move_smart_score(move, 1, 'b', best_score, numeric_limits<int>::max());
		if (best_score < score) {
			best_score = score;
			best_board = move;
		}


	}
	cout << "Best move:" << best_score << "\n";
	return best_board;
}



void play_game() {
	Board current_board{
		{"bR", "bN", "bB", "bQ", "bK", "bB", "bN", "bR"},
		{"bP", "bP", "bP", "bP", "bP", "bP", "bP", "bP"},
		{"", "", "", "", "", "", "", "", },
		{"", "", "", "", "", "", "", "", },
		{"", "", "", "", "", "", "", "", },
		{"", "", "", "", "", "", "", "", },
		{"wP", "wP", "wP", "wP", "wP", "wP", "wP", "wP"},
		{"wR", "wN", "wB", "wQ", "wK", "wB", "wN", "wR"}
	};

	Board tested_board{
		{"bR", "bN", "bB", "bQ", "bK", "bB", "", "bR"},
		{"bP", "bP", "bP", "bP", "bP", "bP", "bP", "bP"},
		{"",   "",   "",   "",   "",   "",   "",   "", },
		{"",   "",   "",   "",   "",   "",   "", "", },
		{"",   "",   "",   "",   "wP",   "",   "bN",   "", },
		{"",   "",   "",   "",   "",   "",   "",   "", },
		{"wP", "wP", "wP", "wP", "", "wP", "wP", "wP"},
		{"wR", "wN", "wB", "wQ", "wK", "wB", "wN", "wR"}
	};



	Board tested_board1{
		{"bR", "bN", "bB", "bQ", "bK", "bB", "", "bR"},
		{"bP", "bP", "bP", "bP", "bP", "bP", "bP", ""},
		{"",   "",   "",   "",   "",   "",   "",   "", },
		{"",   "",   "",   "",   "",   "",   "",   "bP", },
		{"",   "",   "",   "",   "wP",   "",   "wQ",   "", },
		{"",   "",   "",   "",   "",   "",   "",   "", },
		{"wP", "wP", "wP", "wP", "", "wP", "wP", "wP"},
		{"wR", "wN", "wB", "",   "wK", "wB", "wN", "wR"}
	};

	//current_board = tested_board1;
	current_board = load_board("@1031213859");


	while (true) {
		situation_best_score.clear();
		Board tmp = current_board;

		cout << "thinking . . .\n\n";
		//pair<Board, int> solution = move_smart(current_board, 0,'w', numeric_limits<int>::min());
		current_board = computer_play(current_board);
		add_board(current_board);
		
		print_board(current_board);

		cout << get_notation(tmp, current_board) << "\n";
		current_board = humans_move(current_board);
		print_board(current_board);
		add_board(current_board);

	}

}

int main()
{
	//test_knights_move();
	//test_rooks_move();
	play_game();
	//Board test_board = load_board("@3324884253");
	//print_board(test_board);
	return 0;
}

void test_knights_move() {
	Board board = create_empty_board();

	board[4][4] = "wN";
	vector<Board> next_boards = knights_move(board, { 4, 4 });

	for (Board board : next_boards) {
		print_board(board);
	}

}

void test_rooks_move() {
	Board board = create_empty_board();

	board[4][4] = "bR";
	board[4][1] = "bP";
	board[4][6] = "wQ";
	vector<Board> next_boards = rooks_move(board, { 4, 4 });

	for (Board board : next_boards) {
		print_board(board);
	}
}