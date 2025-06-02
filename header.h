#ifndef HEADER
	#include <SDL3/SDL.h>
#endif

#include "const.h"
#include "settings.h"


/*
Miasa ity function ity iverifiena fa mande tsara/installer SDL3

\param game a struct object

\return true and create a window if SDL3 is initiated
\return else return false;
*/
bool game_sdl_init(struct Game* game);

/*
Drawing board

\param game a struct object
\param board ilay tableau ampiasaina mba ametrahan ilay pieces
\param cell_width longeur anilay cell/container
\param cell_height hauter anilay cell/container
*/
void draw_board(struct Game* game, char board[ROWS][COLS], int cell_width, int cell_height);

/*
Function mba ahafana mdraw anilay pieces.

\param renderer ilay asina azy ?
\param center_x center X
\param center_y center Y
\param radius radius of the piece
\param color the color of the piece
*/
void draw_pieces(SDL_Renderer* renderer, int center_x, int center_y, int radius, SDL_Color color);

/*
This function will handle all possible event of the game

\param game a struct object containing (window, renderer)
\param board the board of the game
\param cell_width width of the cell
\param cell_height height of the cell
*/
void event_handler(SDL_Event* event, struct Game* game, struct Cell* cell, struct Coord* position
	, struct NewCoord* new_position, struct PlayerScore* player_score,
	int current_player, char players[2], char board[ROWS][COLS]);

/*
This function will verify each piec of the current player
if one of the piece can make valid move (fanorona 9 rull)

\param board the current board
\param current_player the current player

\return true as said above else false
*/
bool can_move(char board[ROWS][COLS], char players);

/*
* This function will look if the current piece
* on the can_move function can take or not
* 
\param board the current_board of the game
\param row the row recieved from can_move
\param col the col recieved from can_move
*/
bool can_take(char board[ROWS][COLS], const int row, const int col, char other_player);
/*
function to take the pieces

\param position struct that contains the coordonate of the piece
\param new_coord struct that contains the new_position of the piece
\param board the board of the game
\param current_player the current_player 
	a character that identify the current player
*/
void take_pieces(struct Coord* position, struct NewCoord* new_coord, struct PlayerScore* player_score,
	char board[ROWS][COLS], char current_player);

/*
Cleaning SDL

\param game the game's struct
*/
void game_free(struct Game* game);

// verify if one of the player won the game
void game_won(struct Game* game, struct PlayerScore* player_score, bool* running);