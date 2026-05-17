#ifdef _WIN32
        #include <windows.h>
        #define CLEAR_SCREEN "cls"
#else
        #include <unistd.h>
        #define CLEAR_SCREEN "clear"
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#define SQUARE(rank, file) (((rank) - 1) * 8 + ((file) - 1))
#define REVERSE_SQUARE(square, rank, file) do { \
        rank = (square) / 8 + 1; \
        file = (square) % 8 + 1; \
} while (0)

#define SET_BIT(bb, square) ((bb) |= (1ULL << (square)))
#define CLEAR_BIT(bb, square) ((bb) &= ~(1ULL << (square)))
#define IS_BIT_SET(bb, square) (((bb) >> (square)) & 1)

#define MOVE_TO(from, to, piece_bb) do { \
        CLEAR_BIT(piece_bb, from); \
        SET_BIT(piece_bb, to); \
} while (0)

#define EMPTY 0
#define PAWN 1
#define KNIGHT 2
#define BISHOP 3
#define ROOK 4
#define QUEEN 5
#define KING 6

#define WHITE 0
#define BLACK 1


void print_board();

struct chess_move {
        char move_str[10];
        int from;
        int from_file_hint;
        int to;
        int piece_type;
        int promotion_piece;
        int is_capture;
        int is_check;
        int is_castle_queenside;
        int is_castle_kingside;
};

uint64_t white_pawns = 0;
uint64_t white_knights = 0;
uint64_t white_bishops = 0;
uint64_t white_rooks = 0;
uint64_t white_queens = 0;
uint64_t white_king = 0;

uint64_t black_pawns = 0;
uint64_t black_knights = 0;
uint64_t black_bishops = 0;
uint64_t black_rooks = 0;
uint64_t black_queens = 0;
uint64_t black_king = 0;


int get_piece_at(int square)
{
        if (IS_BIT_SET(white_pawns, square) || IS_BIT_SET(black_pawns, square)) {
                return PAWN;
        } else if (IS_BIT_SET(white_knights, square) || IS_BIT_SET(black_knights, square)) {
                return KNIGHT;
        } else if (IS_BIT_SET(white_bishops, square) || IS_BIT_SET(black_bishops, square)) {
                return BISHOP;
        } else if (IS_BIT_SET(white_rooks, square) || IS_BIT_SET(black_rooks, square)) {
                return ROOK;
        } else if (IS_BIT_SET(white_queens, square) || IS_BIT_SET(black_queens, square)) {
                return QUEEN;
        } else if (IS_BIT_SET(white_king, square) || IS_BIT_SET(black_king, square)) {
                return KING;
        }

        return EMPTY;
}

int get_color_at(int square)
{
        if (IS_BIT_SET(white_pawns, square) ||
            IS_BIT_SET(white_knights, square) ||
            IS_BIT_SET(white_bishops, square) ||
            IS_BIT_SET(white_rooks, square) ||
            IS_BIT_SET(white_queens, square) ||
            IS_BIT_SET(white_king, square)
        ) {
                return WHITE;
        } else if (IS_BIT_SET(black_pawns, square) ||
                   IS_BIT_SET(black_knights, square) ||
                   IS_BIT_SET(black_bishops, square) ||
                   IS_BIT_SET(black_rooks, square) ||
                   IS_BIT_SET(black_queens, square) ||
                   IS_BIT_SET(black_king, square)
        ) {
                return BLACK;
        }

        return -1; /* empty */
}

int is_valid_pawn_move(int from, int to, int color)
{
        int from_rank, from_file, to_rank, to_file;
        REVERSE_SQUARE(from, from_rank, from_file);
        REVERSE_SQUARE(to, to_rank, to_file);

        int direction = (color == WHITE) ? 1 : -1;
        int start_rank = (color == WHITE) ? 2 : 7;

        int target_piece = get_piece_at(to);
        int target_color = get_color_at(to);

        if (to_file == from_file && to_rank == from_rank + direction) {
                return (target_piece == EMPTY);
        }

        if (to_file == from_file && to_rank == from_rank + (2 * direction) && from_rank == start_rank) {
                int intermediate_rank = from_rank + direction;
                int intermediate_square = SQUARE(intermediate_rank, from_file);
                return (target_piece == EMPTY && get_piece_at(intermediate_square) == EMPTY);
        }

        if (abs(to_file - from_file) == 1 && to_rank == from_rank + direction) {
                return (target_color != color && target_color != -1 && target_piece != EMPTY);
        }

        return 0;
}

int is_valid_knight_move(int from, int to, int color)
{
        int from_rank, from_file, to_rank, to_file;
        REVERSE_SQUARE(from, from_rank, from_file);
        REVERSE_SQUARE(to, to_rank, to_file);

        int rank_diff = abs(to_rank - from_rank);
        int file_diff = abs(to_file - from_file);

        return (rank_diff == 2 && file_diff == 1) || (rank_diff == 1 && file_diff == 2);
}

int is_valid_bishop_move(int from, int to, int color)
{
        int from_rank, from_file, to_rank, to_file;
        REVERSE_SQUARE(from, from_rank, from_file);
        REVERSE_SQUARE(to, to_rank, to_file);

        int rank_diff = abs(to_rank - from_rank);
        int file_diff = abs(to_file - from_file);

        if (rank_diff != file_diff) return 0;

        int rank_step = (to_rank > from_rank) ? 1 : -1;
        int file_step = (to_file > from_file) ? 1 : -1;

        int curr_rank = from_rank + rank_step;
        int curr_file = from_file + file_step;

        while (curr_rank != to_rank) {
                int curr_square = SQUARE(curr_rank, curr_file);
                if (get_piece_at(curr_square) != EMPTY) return 0;
                curr_rank += rank_step;
                curr_file += file_step;
        }

        return 1;
}

int is_valid_rook_move(int from, int to, int color)
{
        int from_rank, from_file, to_rank, to_file;
        REVERSE_SQUARE(from, from_rank, from_file);
        REVERSE_SQUARE(to, to_rank, to_file);

        if (from_rank != to_rank && from_file != to_file) return 0;
        
        if (from_rank == to_rank) {
                int file_step = (to_file > from_file) ? 1 : -1;
                int curr_file = from_file + file_step;
                while (curr_file != to_file) {
                        int curr_square = SQUARE(from_rank, curr_file);
                        if (get_piece_at(curr_square) != EMPTY) return 0;
                        curr_file += file_step;
                }
        } else {
                int rank_step = (to_rank > from_rank) ? 1 : -1;
                int curr_rank = from_rank + rank_step;
                while (curr_rank != to_rank) {
                        int curr_square = SQUARE(curr_rank, from_file);
                        if (get_piece_at(curr_square) != EMPTY) return 0;
                        curr_rank += rank_step;
                }
        }

        return 1;
}

int is_valid_queen_move(int from, int to, int color)
{
        return is_valid_bishop_move(from, to, color) || is_valid_rook_move(from, to, color);
}

int is_valid_king_move(int from, int to, int color)
{
        int from_rank, from_file, to_rank, to_file;
        REVERSE_SQUARE(from, from_rank, from_file);
        REVERSE_SQUARE(to, to_rank, to_file);

        int rank_diff = abs(to_rank - from_rank);
        int file_diff = abs(to_file - from_file);

        if (rank_diff <= 1 && file_diff <= 1) return 1;

        if (rank_diff == 0 && file_diff == 2) {
                int step = (to_file > from_file) ? 1 : -1;
                int rook_file = (to_file > from_file) ? 8 : 1;
                int curr_file = from_file + step;

                while (curr_file != rook_file) {
                        int curr_square = SQUARE(from_rank, curr_file);
                        if (get_piece_at(curr_square) != EMPTY) return 0;
                        curr_file += step;
                }
                return 1;
        }

        return 0;
}

void square_to_algebraic(int square, char *output)
{
        int rank, file;
        REVERSE_SQUARE(square, rank, file);
        output[0] = 'a' + file - 1;
        output[1] = '0' + rank;
        output[2] = '\0';
}

int parse_algebraic_square(const char *square)
{
        if (strlen(square) < 2) return -1;
        
        int file = square[0] - 'a' + 1;
        int rank = square[1] - '0';

        if (file < 1 || file > 8 || rank < 1 || rank > 8) return -1;

        return SQUARE(rank, file);
}

struct chess_move parse_move(const char *move_str, int color_to_move)
{
        struct chess_move move = {0};
        strcpy(move.move_str, move_str);
        move.from = -1;
        move.from_file_hint = -1;
        move.to = -1;
        move.promotion_piece = 0;
        move.is_capture = 0;
        move.is_check = 0;

        if (strcmp(move_str, "0-0") == 0 || strcmp(move_str, "0-0+") == 0) {
                move.is_castle_kingside = 1;
                if (color_to_move == WHITE) {
                        move.from = SQUARE(1, 5); /* e1 */
                        move.to = SQUARE(1, 7); /* g1 */
                } else {
                        move.from = SQUARE(8, 5); /* e8 */
                        move.to = SQUARE(8, 7); /* g8 */
                }
                move.piece_type = KING;
                return move;
        }

        if (strcmp(move_str, "0-0-0") == 0 || strcmp(move_str, "0-0-0+")) {
                move.is_castle_queenside = 1;
                if (color_to_move == WHITE) {
                        move.from = SQUARE(1, 5); /* e1 */
                        move.to = SQUARE(1, 3); /* c1 */
                } else {
                        move.from = SQUARE(8, 5); /* e8 */
                        move.to = SQUARE(8, 3); /* c8 */
                }
                move.piece_type = KING;
                return move;
        }

        const char *ptr = move_str;

        char piece_char = toupper(*ptr);
        switch (piece_char) {
                case 'N': move.piece_type = KNIGHT; ptr++; break;
                case 'B': move.piece_type = BISHOP; ptr++; break;
                case 'R': move.piece_type = ROOK; ptr++; break;
                case 'Q': move.piece_type = QUEEN; ptr++; break;
                case 'K': move.piece_type = KING; ptr++; break;
                default: move.piece_type = PAWN; break;
        }

        if (move.piece_type == PAWN && strlen(ptr) >= 2 && isalpha(ptr[0]) && ptr[1] == 'x') {
                move.from_file_hint = ptr[0] - 'a' + 1;
                move.is_capture = 1;
                ptr += 2;
        } else {
                const char *x_pos = strchr(ptr, 'x');
                if (x_pos) {
                        move.is_capture = 1;
                        ptr = x_pos + 1;
                }
        }

        /* parse destination square */
        if (strlen(ptr) >= 2 && isalpha(ptr[0]) && isdigit(ptr[1])) {
                move.to = parse_algebraic_square(ptr);
                ptr += 2;
        }

        /* check for promotion */
        const char *eq_pos = strchr(ptr, '=');
        if (eq_pos) {
                char promo = toupper(*(eq_pos + 1));
                switch (promo) {
                        case 'N': move.promotion_piece = KNIGHT; break;
                        case 'B': move.promotion_piece = BISHOP; break;
                        case 'R': move.promotion_piece = ROOK; break;
                        case 'Q': move.promotion_piece = QUEEN; break;
                }
                ptr = eq_pos + 2;
        }

        if (*ptr == '+') move.is_check = 1;
        if (*ptr == '#') move.is_check = 2; /* checkmate */

        return move;
}

int find_from_square_for_move(struct chess_move *move, int color)
{
        uint64_t pieces = 0;

        if (color == WHITE) {
                switch (move->piece_type) {
                        case PAWN: pieces = white_pawns; break;
                        case KNIGHT: pieces = white_knights; break;
                        case BISHOP: pieces = white_bishops; break;
                        case ROOK: pieces = white_rooks; break;
                        case QUEEN: pieces = white_queens; break;
                        case KING: pieces = white_king; break;
                }
        } else {
                switch (move->piece_type) {
                        case PAWN: pieces = black_pawns; break;
                        case KNIGHT: pieces = black_knights; break;
                        case BISHOP: pieces = black_bishops; break;
                        case ROOK: pieces = black_rooks; break;
                        case QUEEN: pieces = black_queens; break;
                        case KING: pieces = black_king; break;
                }
        }

        int found_count = 0;
        int found_squares[64] = {0};

        if (move->piece_type == PAWN && move->from_file_hint >= 0) {
                int expected_file = move->from_file_hint;
                for (int rank = 1; rank <= 8; rank++) {
                        int square = SQUARE(rank, expected_file);
                        if (IS_BIT_SET(pieces, square)) {
                                if (is_valid_pawn_move(square, move->to, color)) {
                                        int target_color = get_color_at(move->to);
                                        if (move->is_capture) {
                                                if (target_color != color && target_color != -1) found_squares[found_count++] = square;
                                        } else {
                                                if (target_color == -1) found_squares[found_count++] = square;
                                        }
                                }
                        }
                }
        } else {
                for (int square = 0; square < 64; square++) {
                        if (!IS_BIT_SET(pieces, square)) continue;

                        int is_valid = 0;

                        switch (move->piece_type) {
                                case PAWN: is_valid = is_valid_pawn_move(square, move->to, color); break;
                                case KNIGHT: is_valid = is_valid_knight_move(square, move->to, color); break;
                                case BISHOP: is_valid = is_valid_bishop_move(square, move->to, color); break;
                                case ROOK: is_valid = is_valid_rook_move(square, move->to, color); break;
                                case QUEEN: is_valid = is_valid_queen_move(square, move->to, color); break;
                                case KING: is_valid = is_valid_king_move(square, move->to, color); break;
                        }

                        if (is_valid) {
                                int target_color = get_color_at(move->to);
                                if (move->is_capture) {
                                        if (target_color != color && target_color != -1) found_squares[found_count++] = square;
                                } else {
                                        if (target_color == -1) found_squares[found_count++] = square;
                                }
                        }
                }
        }

        if (found_count == 1) {
                move->from = found_squares[0];
                return 1;
        } else if (found_count > 1) {
                move->from = found_squares[0];
                return 1;
        }

        return 0;
}


void parse_moves(const char *moves)
{
        char *moves_copy = malloc(strlen(moves) + 1);
        if (!moves_copy) return;
        strcpy(moves_copy, moves);

        char *token;
        int move_number = 1;
        int color_to_move = WHITE;
        int first_board = 1;

        token = strtok(moves_copy, " ");
        while (token != NULL) {
                if (strchr(token, '.') != NULL) {
                        token = strtok(NULL, " ");
                        continue;
                }

                if (!first_board) {
                        printf("Press Enter for next move...");
                        getchar();
                        system(CLEAR_SCREEN);
                }
                first_board = 0;

                printf("Move %d.%s: ", move_number, (color_to_move == WHITE ? "" : ".."));
                printf("%s\n", token);
        
                struct chess_move move = parse_move(token, color_to_move);

                if (move.from == -1) {
                        if (!find_from_square_for_move(&move, color_to_move)) {
                                printf("Error: could not find source square for move %s\n", token);
                                free(moves_copy);
                                return;
                        }
                }

                if (move.from != -1 && move.to != -1) {
                        if (move.is_capture) {
                                int captured_color = get_color_at(move.to);
                                if (captured_color != -1) {
                                        if (captured_color == WHITE) {
                                                if (IS_BIT_SET(white_pawns, move.to)) CLEAR_BIT(white_pawns, move.to);
                                                else if (IS_BIT_SET(white_knights, move.to)) CLEAR_BIT(white_knights, move.to);
                                                else if (IS_BIT_SET(white_bishops, move.to)) CLEAR_BIT(white_bishops, move.to);
                                                else if (IS_BIT_SET(white_rooks, move.to)) CLEAR_BIT(white_rooks, move.to);
                                                else if (IS_BIT_SET(white_queens, move.to)) CLEAR_BIT(white_queens, move.to);
                                                else if (IS_BIT_SET(white_king, move.to)) CLEAR_BIT(white_king, move.to);
                                        } else {
                                                if (IS_BIT_SET(black_pawns, move.to)) CLEAR_BIT(black_pawns, move.to);
                                                else if (IS_BIT_SET(black_knights, move.to)) CLEAR_BIT(black_knights, move.to);
                                                else if (IS_BIT_SET(black_bishops, move.to)) CLEAR_BIT(black_bishops, move.to);
                                                else if (IS_BIT_SET(black_rooks, move.to)) CLEAR_BIT(black_rooks, move.to);
                                                else if (IS_BIT_SET(black_queens, move.to)) CLEAR_BIT(black_queens, move.to);
                                                else if (IS_BIT_SET(black_king, move.to)) CLEAR_BIT(black_king, move.to);
                                        }
                                }
                        }

                        if (color_to_move == WHITE) {
                                switch (move.piece_type) {
                                        case PAWN: MOVE_TO(move.from, move.to, white_pawns); break;
                                        case KNIGHT: MOVE_TO(move.from, move.to, white_knights); break;
                                        case BISHOP: MOVE_TO(move.from, move.to, white_bishops); break;
                                        case ROOK: MOVE_TO(move.from, move.to, white_rooks); break;
                                        case QUEEN: MOVE_TO(move.from, move.to, white_queens); break;
                                        case KING: MOVE_TO(move.from, move.to, white_king); break;
                                }
                        } else {
                                switch (move.piece_type) {
                                        case PAWN: MOVE_TO(move.from, move.to, black_pawns); break;
                                        case KNIGHT: MOVE_TO(move.from, move.to, black_knights); break;
                                        case BISHOP: MOVE_TO(move.from, move.to, black_bishops); break;
                                        case ROOK: MOVE_TO(move.from, move.to, black_rooks); break;
                                        case QUEEN: MOVE_TO(move.from, move.to, black_queens); break;
                                        case KING: MOVE_TO(move.from, move.to, black_king); break;
                                }
                        }
                }
        
                print_board();

                if (color_to_move == WHITE) {
                        color_to_move = BLACK;
                } else {
                        color_to_move = WHITE;
                        move_number++;
                }

                token = strtok(NULL, " ");
        }

        free(moves_copy);
}

void print_board()
{
        char piece_symbols[7] = {' ', 'P', 'N', 'B', 'R', 'Q', 'K'};

        printf("  +-----------------+\n");
        for (int rank = 8; rank >= 1; rank--) {
                printf("%d |", rank);
                for (int file = 1; file <= 8; file++) {
                        int square = SQUARE(rank, file);
                        int piece = get_piece_at(square);
                        int color = get_color_at(square);

                        if (piece == EMPTY) {
                                printf(" .");
                        } else if (color == WHITE) {
                                printf(" %c", piece_symbols[piece]);
                        } else {
                                /* lowercase for black (+32) */
                                printf(" %c", piece_symbols[piece] + 32);
                        }
                }
                printf(" |\n");
        }
        printf("  +-----------------+\n");
        printf("    a b c d e f g h\n\n");
}

void init_board()

{
        for (int file = 1; file <= 8; file++) {
                SET_BIT(white_pawns, SQUARE(2, file));
                SET_BIT(black_pawns, SQUARE(7, file));
        }

        SET_BIT(white_rooks, SQUARE(1, 1)); /* a1 */
        SET_BIT(white_knights, SQUARE(1, 2)); /* b1 */
        SET_BIT(white_bishops, SQUARE(1, 3)); /* c1 */
        SET_BIT(white_queens, SQUARE(1, 4)); /* d1 */
        SET_BIT(white_king, SQUARE(1, 5)); /* e1 */
        SET_BIT(white_bishops, SQUARE(1, 6)); /* f1 */
        SET_BIT(white_knights, SQUARE(1, 7)); /* g1 */
        SET_BIT(white_rooks, SQUARE(1, 8)); /* h1 */

        SET_BIT(black_rooks, SQUARE(8, 1)); /* a8 */
        SET_BIT(black_knights, SQUARE(8, 2)); /* b8 */
        SET_BIT(black_bishops, SQUARE(8, 3)); /* c8 */
        SET_BIT(black_queens, SQUARE(8, 4)); /* d8 */
        SET_BIT(black_king, SQUARE(8, 5)); /* e8 */
        SET_BIT(black_bishops, SQUARE(8, 6)); /* f8 */
        SET_BIT(black_knights, SQUARE(8, 7)); /* g8 */
        SET_BIT(black_rooks, SQUARE(8, 8)); /* h8 */
}

int main() {
        init_board();
        parse_moves("1. Nf3 d5 2. c4 Qd6 3. cxd5 Qxd5 4. Nc3 c5 5. Nxd5 Nc6 6. Qa4 h6 7. Ne5 f5 8. Nxc6 bxc6 9. Qxc6+ Kf7 10. Qxa8 e6 11. Qxc8 exd5 12. e4 fxe4 13. Qd7+ Ne7 14. Be2 Kg6 15. b3 d4 16. Bb2 Rg8 17. O-O Kh7 18. Bc4 Kg6 19. Bxg8 Nxg8 20. Qe8+ Kg5 21. Qxf8 h5 22. Qxg7+ Kf4 23. f3 Kf5 24. g4+ Kf4 25. fxe4+ Kxe4 26. Rae1+ Kd3 27. Qg6+ Kxd2 28. Rd1+ Ke3 29. Qd3#");
        
        return 0;
}
