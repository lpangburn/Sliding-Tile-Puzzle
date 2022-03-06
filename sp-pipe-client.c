#include "stdio.h"
#include "stdlib.h"
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "string.h"
#include <ctype.h> 
#include "globals.h"


/*
* sp-pipe-client.c has all the methods pertaining to the menu and display functionality of the game.
* Methods include clear, quit, display the main menu, display the game menu, check that inputs to the menus are correct, get menu selections from the user, 
* and initialize the game. 
*/

int size;
int tiles;
int arr[81];

/*
* clear the terminal
*/
void clear(void){
    printf("\e[1;1H\e[2J");
}



//SET AND DISPLAY THE BOARD

/*
* Displays the current state of the board after clearing any output on the terminal
*/
void displayBoard(){
    clear();
    int j,k;
    int index = 0;
    
    for(j=0; j<size; j++){
        for(k=0; k<size; k++){
            if(arr[index] == 0){
                printf("%2c\t", ' ');
                
            }else{
                printf("%d\t", arr[index]);
            }  
            index++;  
        }
        printf("\n");
    }

}

/*
*communicates with server to determine if the user won
*@return int indicating if the game is won, 1 = won, 0 = not won
*/
int checkIfWon(){
    char w = 'w';
    int won;
    write(client[1], &w, sizeof(w));
    read(server[0], &won, sizeof(won));

    //if game is won
    if(won == 1){
        return 1;
    }

    return 0;
}









//MENU AND VALID MENU SELECTIONS

/*
*displays the main menu
*n - new, p - previous, q - quit
*/
void mainMenu(){
    printf("Menu:   New Game [n]     Previous game [p]     Quit [q]\n");
    printf("Selection: ");
}

/*
*displays the game menu
*n - new, s - save, n - new game, q - quit
*/
void displayMenu(){
    printf("Menu:   Move [m]     Save game [s]      New game [n]     Quit [q]\n");
    printf("Selection: ");
}

/*
*verifies the main menu selection is valid
*@return int 1 if valid, 0 if invalid
*/
int validMainMenu(char ch){
    int check = 0;
    if ( ch == 'q' ||  ch == 'Q' || ch == 'p' || ch == 'P' || ch == 'n' || ch == 'N'){
        check = 1;
    }
    return check;

}

/*
*verifies the game menu selection is valid
*n - new, s - save, n - new game, q - quit
*@return int 1 if valid, 0 if invalid
*/
int validMenu(char ch){

    int check = 0;
    if (ch == 'm' || ch == 'q' || ch == 's' || ch == 'n' || ch == 'N' || ch == 'S' || ch == 'M' || ch == 'Q' ){
        check = 1;
    }
    return check;

}

/*
*gets the main menu choice
* n - new, p - previous game, q - quit
*@return char representing the main menu choice
*/
char getMainMenuChoice(){
    char c;
    
    scanf("\n%c", &c);
    while ((getchar()) != '\n');
    
    int valid = validMainMenu(c);
   
    while(validMainMenu(c) != 1){
        clear();
        printf("Invalid selection, please choose from the following menu.\n");
        mainMenu();
        c = getchar();
        while ((getchar()) != '\n');
    }
    
    return c;
}

/*
*displays the menu, gets the game menu selection
*checks if the menu is valid; n - new, p - previous game, q - quit 
*@return char for menu selection
*/
char getMenuChoice(){
    char c;
    char x;
    displayMenu();
    scanf("\n%c", &x);
    while ((getchar()) != '\n');
    
    c = tolower(x);
    int valid = validMenu(c);
   
    while(validMenu(c) != 1){
        clear();
        displayBoard();
        printf("Invalid selection, please choose from the following menu.\n");
        displayMenu();
        c = getchar();
        while ((getchar()) != '\n');

    }

    if(c == 'q' || c == 'Q'){
        //quit();
    }
    return c;
}









//VALID TILE FUNCTIONS

/*
*gets a tile that the user wants to move
*/
int getTile(){
    int t;
     
    printf("Tile to move: ");    
          
    scanf("%d", &t);        
    while ((getchar()) != '\n');

    return t;
}

/*
*check with the server that the selected tile is valid
*valid = is on the board and adjacent to the empty tile
*@param int desired tile to move
*@return int indicating if it is valid or not
*/
int checkValidClient(int selection){
    int tileSelection = selection;
    int valid = 0;
   
    write(client[1], &tileSelection, sizeof(tileSelection));
    read(server[0], &valid, sizeof(valid));
    return valid;
}










//FILE OPENING AND SAVING

/*
*get a file name from user input
*@return a string indicating the desired file name to open
*/
char * getFileName(){
    char *file = malloc(15);
    //asks user for potential file name
    printf("File name: ");
    scanf("%s",file);
    fflush(stdin);
    //while ((getchar()) != '\n');

    return file;

}

//determines if user would like to overwrite existing file, if their input is valid
int overwrite(char c){
    if (c == 'y' || c == 'Y'){
        return 1;
    }else if(c == 'n' || c == 'N'){
        return 2;
    }
    return 0;
}

/*
*gets user input determining if they would like to overwrite an existing file
*@param int returns an int indicating their selection, invalid = 0, yes = 1, no = 2
*/
int overwriteSelection(){
    char overwriteSelection;
    scanf("%c", &overwriteSelection);
    while ((getchar()) != '\n');
    int overwriteValid = overwrite(overwriteSelection);
    return overwriteValid;
}








//GAME PROCESSES

/*
*initializes the game by displaying the main menu, and getting user selection.
*user can either start a new game, load a new game, or quit
*@char character representing the user's choice
*/
char initialize(){

    mainMenu(); 
    
    char x = getMainMenuChoice(); 
    char c = tolower(x);

    if(c == 'q'){
        //quit(); close the pipe instead
    }else if (c == 'n'){
        clear();
        printf("Starting a new game.\n");
        return c;
    }else if(c == 'p'){
        clear();
        return c;
    }else{
        printf("Invalid input!\n");
        initialize();

    } 
}

/*
*starts a new game by getting the desired board size
*sets variables appropriately 
*@char c to pass to the server
*/
void newGame(char c){
    //pass n for new game to the server
    clear();
    write(client[1], &c, sizeof(c));

    //get a board size number
    int num;
    printf("Please choose a game board size between %d and %d tiles wide: ", BOARD_MIN, BOARD_MAX);
    scanf("%d", &num);
    write(client[1], &num, sizeof(num));
        
    int validBoard;
    read(server[0], &validBoard, sizeof(validBoard));

    //if validBoard returns invalid, enter a loop to get a valid input
    while(validBoard == 0){
        clear();
        printf("Invalid board size. Please choose a game board size between %d and %d tiles wide. Enter 'q' to quit. \nBoard size: ", BOARD_MIN, BOARD_MAX);
        scanf("%d", &num);
        write(client[1], &num, sizeof(num));
        read(server[0], &validBoard, sizeof(validBoard));
    }
    size = num;
    tiles = num * num;
}

/*
*gets a tile to move from the user 
*checks with the server that the tile is on the board, and that the move is valid
*@char c to pass to the server
*/
void moveTileClient(char c){
    //enter 'm' on server size to move tile
    write(client[1], &c, sizeof(c));

    int valid = 0;

    do{
        clear();
        displayBoard();

        switch(valid){
            case 2:
                printf("Invalid selection. Please choose a number on the board.\n");
                break;
            case 3:
                printf("Invalid selection. Please choose a number adjacent to the empty space.\n");
                break;
            default:  
                break;
        }
        
        int tileSelection = getTile();
        valid = checkValidClient(tileSelection);
        

    }while(valid != 1);

}

/*
*starts the game from a save file
*@char c to pass to the server
*/
void openGame(char c){
    //enter 'p' on server size to move tile
    fflush(stdout);
    write(client[1], &c, sizeof(c));

    //get a file name
    char fileName[15]; 
    char *data = getFileName();
    strcpy(fileName, data);
    write(client[1], &fileName, sizeof(fileName));

    int validFile;
    read(server[0], &validFile, sizeof(validFile));

    while(validFile == 0){
        clear();
        printf("File does not exist. Try a different file name.\n");
        strcpy(fileName, getFileName());

        write(client[1], &fileName, sizeof(fileName));
        read(server[0], &validFile, sizeof(validFile));
    }    
    size = validFile;
    tiles = size * size;

}

/*
*saves the current game state after asking the user for a file name
*@char c to pass to the server
*/
void saveGame(char c){
    //enter 's' on server size to move tile
    write(client[1], &c, sizeof(c));

    char fileName[15];
    strcpy(fileName, getFileName());
    write(client[1], &fileName, sizeof(fileName));

    int exists;
    read(server[0], &exists, sizeof(exists));

    if(exists == 1){
        displayBoard();
        printf("A file with this name already exists. Would you like to overwrite it?\n");
        printf("Yes [y]     No [n]: ");
        int selection = overwriteSelection();

        //continue to prompt for a y or n response
        while(selection == 0){
            displayBoard();
            printf("Invalid selection.\n");
            printf("File with this name already exists, would you like to overwrite it?\n");
            printf("Yes [y]     No [n]: ");
            selection = overwriteSelection();
        }

        write(client[1], &selection, sizeof(selection));
        

    }
    int successful;
    read(server[0], &successful, sizeof(successful));

    if(successful == 1){
        printf("File successfully saved.\n");
        exit(0);
    }else{
        printf("File not saved.\n");
    }

}










//MAIN CLIENT PROCESS

void clientProcess(){
    //close unnecessary pipes
    close(client[0]);
    close(server[1]);

    //clear stdout and terminal
    
    clear();
    fflush(stdout);

    char c = initialize();
    int quitInt = 0;

    //if a new game needs to be started
    if(c == 'n'){
        newGame(c); 
    }

    //load a game from a file
    if(c == 'p'){
        openGame(c);
    }

    //end the game
    if(c == 'q'){
        close(server[0]);
        close(client[1]);
        exit(1);
    }

    //play the game while the user doesnt enter 'q'
    while(quitInt == 0){
        
        char selection;
        char display = 'd';
        write(client[1], &display, sizeof(display));
        read(server[0], arr, sizeof(arr));
        displayBoard();

        selection = getMenuChoice();

        //move a tile
        if(selection == 'm'){
            moveTileClient(selection);
            int won = checkIfWon();

            //display a message indicating the player won
            if (won == 1){
                write(client[1], &display, sizeof(display));
                
                read(server[0], arr, sizeof(arr));
                displayBoard();
                
                printf("You won! Congratulations!\n");
                printf("Press ENTER to return to the main menu.\n");
                getchar();
                while(getchar()!='\n');
                
                quitInt = 1;
            }
        }
        
        //save the current game state
        if(selection == 's'){
            saveGame(selection);
        }

        //start a new game
        if(selection == 'n'){
            newGame(selection);
        }

        //quit the game
        if(selection == 'q'){
            quitInt = 1;
        }
        
        
    }

    clientProcess();
}