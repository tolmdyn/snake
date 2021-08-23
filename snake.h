struct Segment{
	int px;
	int py;
	struct Segment *prev;
	struct Segment *next;
};

struct Snake{
	struct Segment *head;
	struct Segment *tail;
	int direction;
	int length;
};

struct Food{
	int px;
	int py;
};

enum dir{NONE, UP, DOWN, LEFT, RIGHT, NORMAL, COLL, FOOD, QUIT};

struct Snake *init_snake();
int move_snake(struct Snake *snake, struct Food *food, int direction_change);
struct Segment *new_segment(int px, int py, struct Segment *prev, struct Segment *next);

struct Food init_food(int px, int py);
void move_food(struct Food *food);

int process_input(int ch);
void print_snake(struct Snake *snake);

void draw_snake(struct Snake *snake);
void draw_food(struct Food *food);

void init_game(void);
void game_exit(int score);
