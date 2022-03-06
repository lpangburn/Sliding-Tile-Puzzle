#include "stdio.h"
#include "stdlib.h"
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "string.h"
#include "time.h"
#include "globals.h"

int size;
int tiles;
int* mem;

/*
*This file has all the methods associated with the gameboard and maintaining the game state
*initiating the original board, saving and loading a game,
*/

//allocate memory based on size of the board
void allocateMem(){
    mem = (int*) malloc((size*size)*sizeof(int));
}

//free memory, set mem pointer to null
void freeMem(){
    free(mem);
    mem = NULL;
}










//BOARD GETTER, SETTER, WON CONDITIONS

//set the board row and column location to parameter value
void boardSet(int row, int col, int value){
    int location;

    //int location representing the number of spots in memory after the pointer
    location = row * size + col;
   *(mem + location) = value;
}

//get the value of an location in memory 
int boardGet(int row, int col){
    int value;
    //int location representing number of spots in memory after the pointer
    int location;
    location = row * size + col;
    value = *(mem + location);

    return value;
}

//small method to swap two numbers, using a temporary variable 
void swap(int *a, int *b){
    int temp = *a;
    *a = *b;
    *b = temp;
}

/*
*function to shuffle members of an array of a given size
*@param array[] to be shuffled
*@param int size representing the number of elements in the array
*/
void shuffleTiles(int array[], int size){

    //random seed based on time
    srand(time(NULL));

    //iterates through each member of the array, swaps the current tile with the tile at a random index
    for(int i = size; i > 0; i--){
        int j = rand()%(i);
        swap(&array[i], &array[j]);
    }
}

/*
*validBoardSize checking if the user inputted value for the size of the board is valid
*@param int i size to check
*@return int representing validity, 0 if invalid, 1 if valid
*/
int validBoardSize(int i){
    if (i > BOARD_MAX || i < BOARD_MIN){
        return 0;
    }
    return 1;
}

/*
* Initializes the original board by taking number from the user between the board_min and board_max, then adds each necessary tile to the board array.
* Then display the board that was made.
*@param int representing size
*/
void makeBoard(int s){
    int j,k; 
    freeMem();

    size = s;
    tiles = size*size -1;
    allocateMem();

    //puts each tile for the board in an array
    int tileArray[size*size];
    for(int i = 0; i < tiles +1; i++){
        tileArray[i] = i;
    }
    
    //shuffles the tiles in the array
    shuffleTiles(tileArray, tiles);
    
    //put the shuffled tile array in the board
    int numtile = tiles;
    for(j = 0; j < size; j++){
        for(k = 0; k < size; k++){
            boardSet(j,k, tileArray[numtile]);
            numtile--; 
        }
    }     
}


/*
* checks to see if the game is won
* @return int 1 if game is won, 0 else
*/
int won(){

    //verifies that the last tile is 0, or the empty space
    if(boardGet(size-1, size-1) != 0){
        return 0;
    }

    //iterates through the gameboard checking that the each tile is in descending order
    for(int i = 0; i<tiles; i++){
        if(*(mem+i) != i+1){
            return 0;
        }
    }
    return 1;
}










//MOVE TILE AND CHECK IF MOVE IS VALID

//location in memory of the '0' tile representing the empty space
int empty_index;

/*
*get the index of a number in the array
*@param int  
*@return int representing the row
*/
int getIndex(int selection)
{
    for (int i = 0; i < tiles; ++i) {
        if (*(mem + i) == selection) {
            return i;
        }
    }  
}

/*
*updates global 'empty space' information
*/
void emptySpace(){
    empty_index = getIndex(0);
}

/*
*confirms that selected number is a tile on the board
*@param int selected tile
*@return int representing if the number is on the board, 0 if it is not, 1 if it is
*/
int validSelectionServer(int selection){
    if (selection <= 0 || selection > tiles) {
        return 0;
    }
    return 1;
}


/*
*confirms that selected number is a valid move, meaning that the tile is next to the empty space
*@param int selected tile
*@return int representing if the move is valid, 0 if it is not, 1 if it is
*/
int validMoveServer(int i){
    //ensure empty space is up to date
    emptySpace();

    //get index, row, and column information for the selected tile
    int index = getIndex(i);

    //compare the selected tile to the empty space, if any of the conditions are true, return 1
    if(index == empty_index + 1){
        //if the index is at the beginning of a row, don't swap the tiles
        if(index % size == 0){
            return 0;
        }
        return 1;

    }else if(index == empty_index - 1){
        //if the empty space is at the beginning of the row, don't swap the tiles
        if(empty_index % size == 0){
            return 0;
        }
        return 1;

    //if the int
    }else if(index == empty_index + size){
        return 1;

    }else if(index == empty_index - size){
        return 1;
    }

    return 0;
}


/*
*confirms that selected number is on the board and next to an empty space
*@param int selected tile
*@return int representing if the number is on the board and is a valid move, 0 if it is not, 1 if it is
*/
int checkValidServer(int i){
    int vs = validSelectionServer(i);
    int vm = validMoveServer(i);
    emptySpace();
    

    //if validselection (number is not on the board) returns 2
    if(vs == 0){
        
        return 2;

    }else if(vm == 0){    //if validmove (number is not adjacent to empty space) returns 3
      
        return 3;

    }

    return 1;     
}

/*
*move the selected tile by swapping it with the empty space
*@param int selected tile
*/
void moveTile(int t){
   //get current row and column of item to move
   int index = getIndex(t);
   
   //set the index of the moved tile to 0
   *(mem+index) = 0;

   //set the current empty index to the moved tile
   *(mem+empty_index) = t;

   //update the empty space variable
   emptySpace();
    
}










//FILE OPEN AND SAVING FUNCTIONS

/*
*adds .txt to the end of a string
*@param char pointer to name of a file
*@return char array with .txt appended
*/
char* appendTXT(char *fileName){
    //take the user provided filename, and append .txt to make a text file
    const char* name = fileName;
    const char* extension = ".txt";
    char* nameAndExtension;
    nameAndExtension = malloc(strlen(name) +1+4);
    strcpy(nameAndExtension, name);
    strcat(nameAndExtension, extension);

    return nameAndExtension;
}


/*
*determines if a .txt file exists, returns 1 if the file exists, 0 if it does not
*@param char *file name
*@return int representing if the file exists or not
*/
int fileExists(char *fileName){
    char* nameAndExtension = appendTXT(fileName);
    
    if (access(nameAndExtension, F_OK) == 0){
        return 1;
    }else{
        return 0;
    }
}


/*
*take the data from the file with the name fileName and creates the board state with that data
*@param char *file name
*/
void openFile(char *fileName){

    //adds the .txt file extension to fileName string
    char* nameAndExtension = appendTXT(fileName);
    freeMem();
    FILE *fp;
    char *line = NULL;
    size_t length = 0;
    ssize_t read;

    //opens the file "fileName.txt"
    fp = fopen(nameAndExtension, "r");
    
    //reads the first line of the file, which will always be the overall game board dimension
    read = getline(&line, &length, fp);
    size = atoi(line);
    allocateMem();

    //create the board based on the first line of the file, updates global variables with this size
    tiles = size*size - 1;

    //creates an array with size^2 elements
    int arr[tiles + 1];
    int arrIndex = 0;

    //while the file is not EOF, get the next line, convert it to an int, and add it to the array
    while((read = getline(&line, &length, fp)) != -1){
        int i = atoi(line);
        arr[arrIndex]= i;
        arrIndex++;
    }

    int j,k;
    arrIndex = 0;

    //add tiles from the array until the number of tiles is reached
    for(j = 0; j < size; j++){
        for(k = 0; k < size; k++){
            boardSet(j,k,arr[arrIndex]);
            arrIndex++;
        }
    } 

    fclose(fp);
}

/*
*saves the file with name passed as the parameter
*information saved includes: board size, current tiles and their order
*@param char filename indicating the name of the file to be saved
*/
void saveFile(char *fileName){
    //appends .txt extension to the desired filename
    char* nameAndExtension = appendTXT(fileName);
    FILE *fp;
    fp = fopen(nameAndExtension, "w");

    //saves the first line as the dimension of the board
    fprintf(fp, "%d\n", size);

    int j,k;
    
    //iterates through the gameboard and writes each tile to the file on its own line
    for(j=0; j<size; j++){
        for(k=0; k<size; k++){
            if(boardGet(j,k) == 0){
                fprintf(fp, "%d\n", boardGet(j,k));
            }else{
                fprintf(fp, "%d\n", boardGet(j,k));
            }
            
        }
    }

    //close the file, tell the user the file has been saved, exit the game
    fclose(fp);
}











//MENU FUNCTIONS

/*
*validBoardSize checking if the user inputted value for the size of the board is valid
*@param int i size to check
*@return int representing validity, 0 if invalid, 1 if valid
*/
int serverNewGame(){
    int num;
    read(client[0], &num, sizeof(num));

    int validBoard = validBoardSize(num);

    while(validBoard == 0){
        write(server[1], &validBoard, sizeof(validBoard));
        read(client[0], &num, sizeof(num));
        validBoard = validBoardSize(num);
    };
        
    makeBoard(num);
    return validBoard;
        
}

/*
*moveTileServer check if desired tile is a valid move, then moves that tile
*/
int moveTileServer(){
    //read the tile intended to move
    int tileSelection;
    read(client[0], &tileSelection, sizeof(tileSelection));
    
    //check if this tile is valid
    int validTile = checkValidServer(tileSelection);
    
    //if it is not valid, tell the client why this move is invalid, then read from the pipe and check valid until the tile is valid
    while(validTile != 1){
        write(server[1], &validTile, sizeof(validTile));
        read(client[0], &tileSelection, sizeof(tileSelection));
        validTile = checkValidServer(tileSelection);
    }

    //once tile is valid, move that tile
    moveTile(tileSelection);

    //write success code back to the client
    return validTile;
}

/*
*checks to see if the game is won, returns the value to the client
*/
int gameWon(){
    int w = won();
    return w;

}

/*
*opens a file based on user input
*/
int openGameServer(){
    char fileName[15];

    read(client[0], &fileName, sizeof(fileName));

    int validFile = fileExists(fileName);
    
    while(validFile != 1){
        write(server[1], &validFile, sizeof(validFile));
        read(client[0], &fileName, sizeof(fileName));
        validFile = fileExists(fileName);
    }

    openFile(fileName);
    return size;
}


int serverSaveGame(){
    char fileName[15];
    read(client[0], &fileName, sizeof(fileName));

    int exists = fileExists(fileName);
    write(server[1], &exists, sizeof(exists));

    if (exists == 1){
       int selection;
       read(client[0], &selection, sizeof(selection));

       if (selection == 1){
           saveFile(fileName);
           int saved = 1;
           write(server[1], &saved, sizeof(saved));
       }else{
           int saved = 0;
           write(server[1], &saved, sizeof(saved));
       }

    }

    saveFile(fileName);
    return 1;
    
}









//MAIN SERVER PROCESS

void serverProcess(){
    close(client[1]);
    close(server[0]);
    char c;
    read(client[0], &c, sizeof(c));
    if (c == 'n' || c == 'N'){
        //read board size from client process
        int n = serverNewGame();
        write(server[1], &n, sizeof(n));
    }

    //make a board from a file
    if(c == 'p' || c == 'P'){
        //read game file name
        int o = openGameServer();
        write(server[1], &o, sizeof(o));
    }

    if(c == 0){
        exit(0);
    }


    if(c == 'd' || c == 'D'){
        int array[tiles+1];
        int j,k;
        int n = 0;

        //put all items currently in memory into an array, pass the array through the pipe
        for(j=0; j<size; j++){
            for(k=0; k<size; k++){
                int value;
                //int location representing number of spots in memory after the pointer
                int location;
                location = j * size + k;
                value = *(mem + location);
                array[n] = value;
                n++;
            }
        }
                    
        write(server[1], &array, sizeof(array));  
    }

    //if m is read from the pipe, user wants to move a tile
    if(c == 'm' || c == 'M'){
        int m = moveTileServer();
        write(server[1], &m, sizeof(m));
    }
        
    if(c == 's' || c == 'S'){
        int s = serverSaveGame();
        write(server[1], &s, sizeof(s));
    }

    //check if the game is won
    if(c == 'w' || c == 'W'){
        int w = gameWon();
        write(server[1], &w, sizeof(w));
    }

    //if n is read from the pipe, user wants to start a new game
    if(c == 'n' || c == 'N'){
        int n = serverNewGame();
        write(server[1], &n, sizeof(n));
    }

    
    serverProcess();
}