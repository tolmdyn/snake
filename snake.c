#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>

#include "snake.h"
#include "screen.h"

#define INITIAL_DELAY 100000

int main(int argc, char *argv[]){
	int score = 0;
	int ch;
	int chdir = NONE;
	int stat;
	int delay = INITIAL_DELAY;
	
	srand(1);
	
	struct Snake *snake = init_snake();
	struct Food food = init_food(rand() % WIDTH, rand() % HEIGHT);
	
	init_game();
	
	while(1){
		erase();
		draw_snake(snake);
		draw_food(&food);

		mvprintw(0, 0, "%d %d [SCORE %d]", snake->head->px, snake->head->py, score);
		refresh();
		
		if ((ch = getch()) != ERR){
			chdir = process_input(ch);
			if (chdir == QUIT){
				game_exit(score);
			}
		}
		
		if ((stat = move_snake(snake, &food, chdir)) != NORMAL){
			if (stat == FOOD){
				score++;
				move_food(&food);
				delay > 20000 ? delay -= 5000 : delay;
			} else if (stat == COLL){
				free(snake);
				game_exit(score);
			}
		}
		usleep(delay);
	}

	return 0;
}

struct Snake *init_snake(){
	struct Snake *snake = malloc(sizeof(struct Snake));
	
	snake->tail = new_segment(MID_X, MID_Y+1, NULL, NULL);
	snake->head = new_segment(MID_X, MID_Y, NULL, snake->tail);
	snake->tail->prev = snake->head;
	snake->direction = UP;
	
	return snake;
}

struct Segment *new_segment(int px, int py, struct Segment *prev, struct Segment *next){
	struct Segment *new = malloc(sizeof(struct Segment));
	
	new->px = px;
	new->py = py;
	new->prev = prev;
	new->next = next;
	
	return new;
}

int move_snake(struct Snake *snake, struct Food *food, int direction_change){
	int newpx, newpy;
	int status = NORMAL;
	
	
	if ((direction_change == UP && snake->direction == DOWN)			
		||	(direction_change == DOWN && snake->direction == UP)	
		||	(direction_change == LEFT && snake->direction == RIGHT) 
		||	(direction_change == RIGHT && snake->direction == LEFT)){
			direction_change = NONE;
			}
	
	/*
	* Alternate direction implementation 1 
	*if (direction_change == UP && snake->direction == DOWN) direction_change = NONE;
	*if (direction_change == DOWN && snake->direction == UP) direction_change = NONE;
	*if (direction_change == LEFT && snake->direction == RIGHT) direction_change = NONE;
	*if (direction_change == RIGHT && snake->direction == LEFT) direction_change = NONE;;
	*/
	
	/*
	 * * Alternate direction implementation 2
	*switch(direction_change){
	*	case NONE:
	*		break;
		case UP:
			if (snake->direction == DOWN)
				direction_change = NONE;
			break;
		case DOWN:
			if (snake->direction == UP)
				direction_change = NONE;
			break;
		case LEFT:
			if (snake->direction == RIGHT)
				direction_change = NONE;
			break;
		case RIGHT:
			if (snake->direction == LEFT)
				direction_change = NONE;
			break;
		default:
			break;
	}*/
	
	if (direction_change != NONE){
		snake->direction = direction_change;
	}
	
	switch(snake->direction){
		case UP:
			newpx = snake->head->px; 
			newpy = snake->head->py == 0 ? HEIGHT : (snake->head->py - 1);
			break;
		case DOWN:
			newpx = snake->head->px; 
			newpy = (snake->head->py + 1) % HEIGHT;
			break;
		case LEFT:
			newpx = snake->head->px == 0 ? WIDTH : (snake->head->px - 1); 
			newpy = snake->head->py;
			break;
		case RIGHT:
			newpx = (snake->head->px + 1) % WIDTH; 
			newpy = snake->head->py;
			break;
	}
	
	//check if collision with self;
	struct Segment *tmp_seg = snake->head;
	while (tmp_seg->next != NULL){
		if ((tmp_seg->px == newpx) && (tmp_seg->py == newpy)){
			return COLL;
		}
		tmp_seg = tmp_seg->next;
	}
	
	//check if collision with food
	if ((newpx == food->px) && (newpy == food->py)){
		status = FOOD;
	}
	
	snake->head = new_segment(newpx, newpy, NULL, snake->head);
	snake->head->next->prev = snake->head;
	
	if (status != FOOD){
		struct Segment *tmp_seg = snake->tail;
		snake->tail = snake->tail->prev;
		snake->tail->next = NULL;
		free(tmp_seg);
	}
	
	return status;

}

int process_input(int ch){
	switch(ch){
		case KEY_UP:
			return UP;
			break;
		case KEY_DOWN:
			return DOWN;
			break;
		case KEY_LEFT:
			return LEFT;
			break;
		case KEY_RIGHT:
			return RIGHT;
			break;
		case 'q':
			return QUIT;
			break;
		default:
			return NONE;
	}
	return NONE;
}

void print_snake(struct Snake *snake){
	struct Segment *tmp_seg = snake->head;
	printf("Head:");
	while (tmp_seg != NULL){
		printf("%d, %d\n", tmp_seg->px, tmp_seg->py);
		tmp_seg = tmp_seg->next;
	}
}

void draw_snake(struct Snake *snake){
	struct Segment *tmp_seg = snake->head;
	
	mvaddch(tmp_seg->py, tmp_seg->px, '0');
	tmp_seg = tmp_seg->next;
	
	while (tmp_seg != NULL){
		mvaddch(tmp_seg->py, tmp_seg->px, '#');
		tmp_seg = tmp_seg->next;
	}
}

struct Food init_food(int px, int py){
	struct Food temp;
	temp.px = px;
	temp.py = py;
	return temp;
}

void move_food(struct Food *food){
	food->px = rand()%WIDTH;
	food->py = rand()%HEIGHT;
}

void draw_food(struct Food *food){
	mvaddch(food->py, food->px, '&');
}

void init_game(){
	initscr();
	clear();
	noecho();
	cbreak();
	
	curs_set(0);
	keypad(stdscr, TRUE);
	
	mvprintw(MID_Y-1, MID_X-2, "SNAKE");
	mvprintw(MID_Y+2, MID_X-10, "press space to start");
	while(getch() != ' ');
	
	refresh();
	nodelay(stdscr, TRUE);
}

void game_exit(int score){
	mvprintw(MID_Y, MID_X-5, "GAME OVER");
	mvprintw(MID_Y+2, MID_X-5, "score: %d", score);
	nodelay(stdscr,FALSE);
	getch();
	getch();
	erase();
	clrtoeol();
	refresh();
	endwin();
	exit(EXIT_SUCCESS);
}
