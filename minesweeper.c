#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MINE '+'
#define FLAG 'P'
#define REMOVE 'R'
#define STRIKE 'S'
#define EMPTY '.'
#define DEFAULT '#'


// malloc of game field by giving rows and cols
char** generate(int *rows, int *cols){

    // for each pointer create an array of char
    char **m = (char**)malloc(*rows * sizeof(char*));
    for(int i = 0; i < *rows; i++)
        m[i] = (char*)malloc(*cols * sizeof(char));

    return m;
}


// initialize game field with specified element
void fill(char **m, int rows, int cols, char element){

    // initialize with specified element
    for(int i = 0; i < rows; i++)
        for(int j = 0; j < cols; j++)
            m[i][j] = element;
}


// matrice, numero righe, numero colonne, numero di mine, simbolo mina
void solve(char **m, int rows, int cols, int mines){
    
    int x, y, i = 0;

    // initialize game field with EMPTY element
    fill(m, rows, cols, EMPTY);

    // randomly place mines around game field
    while(i < mines){
        x = rand() % rows;
        y = rand() % cols;
        if(m[x][y] != MINE){
            m[x][y] = MINE;
            i++;
       }
    }

    // calculates, converts into char and place numbers in the game field
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            if(m[i][j] != MINE){
                int count = 0;
                for(int x = i - 1; x <= i + 1; x++)
                    for(int y = j - 1; y <= j + 1; y++)
                        if((x >= 0 && x < rows) && (y >= 0 && y < cols) && (m[x][y] == MINE))
                            count++;
                if(count > 0)
                    m[i][j] = '0' + count;
            }
        }
    }
}


// print game field
void print(char **m, int *rows, int *cols){

    // Clear the screen
    //system("clear");
    putchar('\n');
    putchar('\n');

    // Print column numbers header
    fprintf(stdout, "     ");
    for (int i = 0; i < *cols; i++)
        fprintf(stdout, "%3d", i);
    putchar('\n');

    // Print horizontal separator
    fprintf(stdout, "    +");
    for (int i = 0; i < *cols; i++)
        fprintf(stdout, "---");
    putchar('\n');

    // Print row numbers and row elements
    for (int i = 0; i < *rows; i++) {
        fprintf(stdout, "%4d|", i);
        for (int j = 0; j < *cols; j++)
            fprintf(stdout, "%3c", m[i][j]);
        putchar('\n');
    }

}



// libera lo spazio vuoto
void free_space(char **sol, char **user, int i, int j, int cols, int rows){

    if (i < 0 || i >= rows || j < 0 || j >= cols || user[i][j] != DEFAULT || sol[i][j] == MINE) {
        return;  // Return if cell is out of bounds, already processed, or a mine
    }

    user[i][j] = sol[i][j];

    // If the revealed cell is not empty, don't reveal its neighbors
    if (sol[i][j] != EMPTY) {
        return;
    }

    for (int x = i - 1; x <= i + 1; x++) {
        for (int y = j - 1; y <= j + 1; y++) {
            if (x != i || y != j) {  // Exclude the current cell
                free_space(sol, user, x, y, cols, rows);
            }
        }
    }
}

// player move
int uncover(char **sol, char **user, char mode, int i, int j, int cols, int rows, int *left, int *mines, int *flags){

	switch(mode){
		case STRIKE:
			if(user[i][j] != FLAG){
				if(sol[i][j] == MINE){
					user[i][j] = MINE;
					fprintf(stdout, "You've blowned up!\n");
					return 0;
				}else if(sol[i][j] == EMPTY)
					free_space(sol, user, i, j, cols, rows);
				else
					user[i][j] = sol[i][j];
			}
			break;
			
		case FLAG:
			if(user[i][j] == DEFAULT){
				user[i][j] = FLAG;
                *flags = *flags + 1;
				if(sol[i][j] == MINE){
					*left = *left - 1;
					if(*left == 0 && *flags == *mines){
						fprintf(stdout, "\n\n     | You won the game! |\n");
						return 0;
					}
				}
			}
			break;
			
		case REMOVE:
			if(user[i][j] == FLAG){
                *flags = *flags - 1;
				user[i][j] = DEFAULT;
				if(sol[i][j] == MINE){
					*left = *left + 1;
                    if(*left == 0 && *flags == *mines){
                        fprintf(stdout, "\n\n     | You won the game! |\n");
                        return 0;
                    }
                }
			}
			break;

		default:
			fprintf(stderr, "Invalid move! Please retry...\n");
	}
	return 1;
}


// free up space for both game fields
void freeup_space(char **sol, char **user, int *rows){
    for(int i = 0; i < *rows; i++){
        free(sol[i]);
        free(user[i]);
	}
    free(sol);
    free(user);
}


int main(int argc, char *argv[]){

    int rows = atoi(argv[1]);
    int cols = atoi(argv[2]);
    int mines = atoi(argv[3]);
    int left = mines;
    int flags = 0;

    srand(time(NULL));    
    
    char **sol = generate(&rows, &cols);
    char **user = generate(&rows, &cols);
    solve(sol, rows, cols, mines);
    fill(user, rows, cols, DEFAULT);


    system("clear");
    do{
        int x, y;
        char mode;
        
		print(user, &rows, &cols);
        putchar('\n');
        fprintf(stdout, "\t> ");

        scanf(" %c", &mode);
        scanf("%d %d", &x, &y);
        system("clear");

        if(!uncover(sol, user, mode, x, y, rows, cols, &left, &mines, &flags))
            break;
    }while(1);
    
    print(sol, &rows, &cols);
    putchar('\n');
    
    freeup_space(sol, user, &rows);
    return 0;
}
