// the game's struct containing the window
// and the renderer
struct Game {
	SDL_Window* window;
	SDL_Renderer* renderer;
};

// The tobe position of the piece
struct Coord {
	int* row;
	int* col;
};

// the players score
struct PlayerScore {
	int* number_of_o;
	int* number_of_x;
};

// cell settings
struct Cell {
	int* cell_width;
	int* cell_height;
};

struct NewCoord {
	int* row;
	int* col;
};