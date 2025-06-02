/*
	Vakina REAMDE raha te icompile / tsy nisy ilay
	version compiler niaraka t@ source code
	TODO:	farany zaraina anaty module sahaza azy
			fa tena lasa feno be ilay izy no sady misy
			fonction manana paramètre mihotra ny 5 LOZA!!

	// TODO: what if the player goes in the same direction again?
			=> use pointer
*/
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "header.h"

int SDL_main(int argc, char* argv[]) {
	struct Game game = { NULL };
	// keeping track of the state/whereabout of the event later on
	struct Coord coord = { -1, -1 };
	struct NewCoord new_coord = { -1, -1 };
	struct PlayerScore player_score = { 22, 22 };

	char board[ROWS][COLS] = {
		{ 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O' },
		{ 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O' },
		{ 'O', 'X', 'O', 'X', ' ', 'O', 'X', 'O', 'X' },
		{ 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X' },
		{ 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X' }
	};
	// TODO: misy olana fa tsy mety ilay izy tokony O => X
	char players[] = { 'X', 'O' };
	int current_player = 0;

	if (!game_sdl_init(&game))
		return 1;
	int cell_width = WINDOW_WIDTH / COLS;
	int cell_height = WINDOW_HEIGHT / ROWS;
	struct Cell cell = { cell_width, cell_height };
	// putting the window in the middle of the scrren
	SDL_SetWindowPosition(game.window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	SDL_Event event;
	bool running = true;

	while (running) {
		// TODO: mitady background dia ovana ity
		SDL_SetRenderDrawColor(game.renderer, 60, 60, 60, 150);
		SDL_RenderClear(game.renderer);

		while (SDL_PollEvent(&event))
			// TODO: Mitady solution mba tsy mihotra ny 5 arguments
			// tss solution hatreto
			event_handler(&event, &game, &cell, &coord, &new_coord,
				&player_score, current_player, players, board);

		draw_board(&game, board, cell_width, cell_height);
		SDL_RenderPresent(game.renderer);
		SDL_Delay(16);
		if (!can_move(board, players[current_player]))
			current_player = (current_player + 1) % 2; // ovaina ny player
		game_won(&game, &player_score, &running);
	}

	game_free(&game);
	return 0;
}

bool game_sdl_init(struct Game* game) {
	if (!SDL_Init(SDL_INIT_VIDEO))
		return false;

	game->window = SDL_CreateWindow(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
	game->renderer = SDL_CreateRenderer(game->window, NULL);
	return true;
}

void draw_board(struct Game* game, char board[ROWS][COLS], int cell_width, int cell_height) {
	SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 0);

	for (int i = 0; i <= ROWS; i++)
		SDL_RenderLine(game->renderer, 0, i * cell_height, WINDOW_WIDTH - 9, i * cell_height);

	for (int j = 0; j <= COLS; j++)
		SDL_RenderLine(game->renderer, j * cell_width, 0, j * cell_width, WINDOW_HEIGHT);

	for (int row = 0; row < ROWS; row++) {
		for (int col = 0; col < COLS; col++) {
			char piece = board[row][col];
			
			int center_x = col * cell_width + cell_width / 2;
			int center_y = row * cell_height + cell_height / 2;
			int radius = ((cell_width < cell_height ? cell_width : cell_height) / 2) - 5;

			SDL_Color color;
			if (piece == 'X')
				color = (SDL_Color){255, 0, 0, 255};
			else if (piece == 'O')
				color = (SDL_Color){255, 255, 255, 255};
			else
				continue;

			draw_pieces(game->renderer, center_x, center_y, radius, color);
		}
	}
}

void draw_pieces(SDL_Renderer* renderer, const int center_x, const int center_y,
	const int radius, SDL_Color color) {
	// tao @ stackoverflow (ts amp ny documentation SDL)
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	for (int w = 0; w < radius * 2; w++) {
		for (int h = 0; h < radius * 2; h++) {
			int dx = radius - w;
			int dy = radius - h;
			if ((dx * dx + dy * dy) <= (radius * radius))
				SDL_RenderPoint(renderer, center_x + dx, center_y + dy);
		}
	}
}

void event_handler(SDL_Event* event, struct Game* game, struct Cell* cell, struct Coord* position
	, struct NewCoord* new_position, struct PlayerScore* player_score,
	int current_player, char players[2], char board[ROWS][COLS])
{
	int cell_width = cell->cell_width;
	int cell_height = cell->cell_height;
	int row, col;
	if (event->type == SDL_EVENT_QUIT)
		game_free(&game);

	if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
		int new_row = event->button.y / cell_height;
		int new_col = event->button.x / cell_width;

		if (position->row == -1 && position->col == -1) {
			// ialana @verification voalohany io.
			// eg: position(-1, -1) => position(1, 3);
			if (board[new_row][new_col] != players[current_player]) {
				printf("Please wait your turn\n");
				position->row = position->col = -1;
				return; // tapahana (miverina misafid)
			}
			position->row = new_row;
			position->col = new_col;
		} else {
			row = position->row;
			col = position->col;

			// raha tss anty dia misy bug ilay jeux!!!
			// ts aiko fa : board[row][col] = ' ' => position(-1, -1)

			if (row < 0 || col < 0) {
				printf("An error occured please try again\n");
				position->row = position->col = -1;
				return; // tapahana (miverina misafid)
			}

			int absx = fabs(new_row - row);
			int absy = fabs(new_col - col);
			// jerena hoe movement valid ve ialy izy
			// misy method mety mbika kokoa fa ito no itako t@2maraina
			if (!((absx == 1 || absx == 0) && (absy == 1 || absy == 0)) || board[new_row][new_col] != ' ') {
				printf("Invalid impossible\nTry again\n");
				position->row = position->col = -1;
				return; // tapahana (miverina misafid)
			}

			board[new_row][new_col] = board[row][col];
			if (new_position->row == -1 || new_position->col == -1) {
				new_position->row = new_row;
				new_position->col = new_col;
			}
			int other_player = (current_player + 1) % 2;
			take_pieces(position, new_position, player_score, board, players[other_player]);
			board[row][col] = ' ';
			// Averina @valeur de vérification satria novaitsika izy
			position->row = position->col = new_position->row = new_position->col = -1;
		}
	}
}

bool can_move(char board[ROWS][COLS], char curren_player) {
	// TODO: afoezina fa lava loatra important!
	for (int row = 0; row < ROWS; row++) {
		for (int col = 0; col < COLS; col++) {
			char piece = board[row][col];
			if (piece == curren_player) {
				if (row + 1 < ROWS && col + 1 < COLS) {
					if (board[row + 1][col + 1] == ' ')
						return true;
				}
				else if (row + 1 < ROWS) {
					if (board[row + 1][col] == ' ')
						return true;
				}
				else if (col + 1 < COLS) {
					if (board[row][col + 1] == ' ')
						return true;
				}
				else if (row - 1 >= 0 && col - 1 >= 0) {
					if (board[row - 1][col - 1] == ' ')
						return true;
				}
				else if (row - 1 >= 0) {
					if (board[row - 1][col] == ' ')
						return true;
				}
				else if (col - 1 >= 0) {
					if (board[row][col - 1] == ' ') {
						return true;
					}
				}
			}
		}
	}
	return false; // raha tss marina ireo zay fo tonga ato
}

bool can_take(char board[ROWS][COLS], const int row, const int col, char other_player) {
	return board[row][col] == other_player;
}

void take_pieces(struct Coord* position, struct NewCoord* new_coord,
	struct PlayerScore* player_score, char board[ROWS][COLS], char other_player) {

	// TODO: misy erreur ato mila amboarina
	int new_coord_row = new_coord->row;
	int new_coord_col = new_coord->col;

	int position_row = position->row;
	int position_col = position->col;
	// mitov if (true) dx = ... else dx = ...
	int dx = new_coord_row - position_row < 0 ? -1 : (new_coord_row - position_row == 0 ? 0 : 1);
	int dy = new_coord_col - position_col < 0 ? -1 : (new_coord_col - position_col == 0 ? 0 : 1);

	int x = new_coord_row + dx;
	int y = new_coord_col + dy;
	while (x >= 0 && x < ROWS && y >= 0 && y < COLS) {
		if (board[x][y] == other_player) {
			board[x][y] = ' ';
			if (other_player == 'O')
				player_score->number_of_o--;
			else
				player_score->number_of_x--;
		}
		else
			break;
		x += dx;
		y += dy;
	}
}

void game_won(struct Game* game, struct PlayerScore* player_score, bool* running) {
	if (player_score->number_of_o <= 0) {
		printf("Player 1 won the game\n");
		running = false;
		game_free(game);
	}
	else if (player_score->number_of_x <= 0) {
		printf("Player 2 won the game\n");
		running = false;
		game_free(game);
	}
}

void game_free(struct Game* game) {
	if (game->window)
		SDL_DestroyWindow(game->window);
	if (game->renderer)
		SDL_DestroyRenderer(game->renderer);
	SDL_Quit();
	exit(0);
}
