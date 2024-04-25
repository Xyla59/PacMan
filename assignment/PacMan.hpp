#include <fstream>
#include <string>
#include "..\assignment\Entity.hpp"

using namespace std;

class PacMan{
    private:        
        char grid[cellAcross_][cellDown_]; //2d array to store original values (changes when pellets/energisers are eaten)
        char drawGrid[cellAcross_][cellDown_]; //2d array to display to user (changes with movement of entities/displays etc)
        int turns[cellAcross_][cellDown_]; //2d array to show where turns are required in the maze (for ghost AI)
        bool starting; //starting positions
        int score, ghostPoints; //overall score and how many points eating a ghost is worth
        int pEaten; //number of pellets eaten (for win condition and ghost moving out of home)
        int eEaten; //number of energisers eaten (for win condition)
        bool paused, moving;
        bool cherry; //if cherry is displayed
        int cherryTimer; //time cherry has been displayed, different to other timers
        bool win, lose, gameOver;
        int roundsLost;
        int spawnX, spawnY; //spawn location outside of home (ghosts move here when moving out of home)
        int outTimer, gameTimer, frightTimer; //timers for time taken to move out of home, overall game time (switching between chase and scatter), and time in frightened mode 
        bool outTimerActive, gameTimerActive, frightTimerActive; //if timers are active
        State gameState, lastGameState; //The states the entire game is in (chase, scatter or frightened)
    public:
        Pacman pacman; 
        Blinky blinky; //red ghost
        Pinky pinky; //pink ghost
        Inky inky; //blue ghost
        Clyde clyde; //orange ghost
        PacMan();
        void play();
        void pause();
        void currentPos(); //resets entity locations to starting ones
        void movePacman(); 
        void moveGhosts(); //checks if a state change in required for the ghosts and increments timers
        void moveBlinky();
        void movePinky();
        void moveInky();
        void moveClyde();
        char calcNext(char nextMove, int x, int y, int targetX, int targetY); //calculates available and most efficient move after the next move is made (targetted turning)
        char calcNext(char nextMove, int x, int y); //calculates available moves then executes in order (pseudo random turning)
        bool validMove(int x, int y, char move);
        void checkCell(int x, int y); //checks whats in a cell when pacman moves into it
        bool checkWinOrLose();
        void reset(); //resets upon losing a life
        void endWinScreen();
        void endLoseScreen();
        void reverse(); //calculates reverse direction of the current move and sets it as next move
        char getGridValue(int x, int y);
        int getCellAcross();
        int getCellDown();
        bool getStarting();
        int getScore();
        bool getCherry();
        bool getMoving();
        bool getPaused();
        bool getWin();
        bool getLose();
        bool getGameOver();
        State getGameState();
        int getPEaten();
        int getEEaten();
        int getRoundsLost();
        int getFrightTimer();
        void setMoving();
        void frightened(); //start of frightened state
        void frightened(char name); //frightened movements (for ghosts)
        void scatter(char name); //scattering movements (for ghosts)
        void chase(char name); //chasing movements (for ghosts)
        void dead(char name); //returning home (for ghosts)
        void home(char name); //waiting/moving out of the home (for ghosts)
        void spawnCherry();
        void updateState(State update);
};

PacMan::PacMan(){
    ifstream gridFill("../assignment/gridFill.txt"); //fills grid, draw grid and turns arrays with relevent info from text file
    string text;
    int x = 0, y = 0;
    if (gridFill.is_open()){
        while(getline(gridFill, text)){
            grid[x][y] = text[0];
            drawGrid[x][y] = text[0];
            turns[x][y] = (int)text[1];
            x++;
            if (x == cellAcross_){
                x = 0;
                y++;
            }
        }
    }
    //declarations for variables
    gridFill.close();
    paused = true;
    score = 0;
    ghostPoints = 200;
    pEaten = 0;
    eEaten = 0;
    roundsLost = 0;
    cherry = false;
    starting = true;
    moving = false;
    win = false;
    lose = false;
    gameOver = false;
    gameTimer = 0;
    gameTimerActive = true;
    outTimer = 0;
    outTimerActive = true;
    frightTimer = 0;
    frightTimerActive = false;
    gameState = State::scatter;
}

void PacMan::play(){
    starting = false;
    paused = false;
}

void PacMan::pause(){ //toggles pause
    paused = !paused;
}

void PacMan::currentPos(){
    for (int y = 0; y < cellDown_; y++){
        for (int x = 0; x < cellAcross_; x++){
            switch(grid[x][y]){
                case 'U':
                    pacman.setCurrent(x, y);
                    drawGrid[x][y] = 'U';
                    break;
                case 'S':
                    blinky.setCurrent(x, y);
                    spawnX = x; //spawn locations when moving out of home
                    spawnY = y;                   
                    drawGrid[x][y] = 'B';
                    break;
                case 'P':
                    pinky.setCurrent(x, y);
                    blinky.setHome(x, y); //blinky and pinky share the same home cell
                    pinky.setHome(x, y);
                    drawGrid[x][y] = 'P';
                    break;
                case 'I':
                    inky.setCurrent(x, y);
                    inky.setHome(x, y);
                    drawGrid[x][y] = 'I';
                    break;
                case 'C':
                    clyde.setCurrent(x, y);
                    clyde.setHome(x, y);
                    drawGrid[x][y] = 'C';
                    break;
            }
        }
    }
}

void PacMan::movePacman(){
    if(pacman.getMoveChange()){
        if(validMove(pacman.getX(), pacman.getY(), pacman.getNextMove())){
            pacman.setCurMove();
        }
    }
    if(validMove(pacman.getX(), pacman.getY(), pacman.getCurMove())){ //only pacman directly interacts with grid array, so checks are made so he doesnt overwrite important spots
        drawGrid[pacman.getX()][pacman.getY()] = 'b';
        if (grid[pacman.getX()][pacman.getY()] != 'U' && grid[pacman.getX()][pacman.getY()] != 'S' && grid[pacman.getX()][pacman.getY()] != 'c'){
            grid[pacman.getX()][pacman.getY()] = 'b';
        }
        pacman.move();
        checkCell(pacman.getX(), pacman.getY());
    }
}

void PacMan::moveGhosts(){
    //work out state
    if (gameState != State::frightened){
        if (gameTimer == 22 || gameTimer == 103 || gameTimer == 178 || gameTimer == 253){
            updateState(State::chase);
            reverse();
        }
        else if (gameTimer == 82 || gameTimer == 163 || gameTimer == 238){
            updateState(State::scatter);
            reverse();
        }
    }
    else{
        if (frightTimer == 26){
            updateState(lastGameState);
            gameTimerActive = true;
            frightTimerActive = false;
            ghostPoints = 200;
        }
    }
    //timer increments
    if(outTimerActive){
        outTimer++;
    }
    if(gameTimerActive){
        gameTimer++;
    }
    else if(frightTimerActive){
        frightTimer++;
    }
}

void PacMan::moveBlinky(){
    switch(blinky.getState()){
        case State::home:
            home('B');
            break;
        case State::chase:
            chase('B');
            break;
        case State::scatter:
            scatter('B');
            break;
        case State::frightened:
            frightened('B');
            break;
        case State::dead:
            dead('B');
            break;
    }
}

void PacMan::movePinky(){
    switch(pinky.getState()){
        case State::home:
            home('P');
            break;
        case State::chase:
            chase('P');
            break;
        case State::scatter:
            scatter('P');
            break;
        case State::frightened:
            frightened('P');
            break;
        case State::dead:
            dead('P');
            break;
    }
}

void PacMan::moveInky(){
    switch(inky.getState()){
        case State::home:
            home('I');
            break;
        case State::chase:
            chase('I');
            break;
        case State::scatter:
            scatter('I');
            break;
        case State::frightened:
            frightened('I');
            break;
        case State::dead:
            dead('I');
            break;
    }
}

void PacMan::moveClyde(){
    switch(clyde.getState()){
        case State::home:
            home('C');
            break;
        case State::chase:
            chase('C');
            break;
        case State::scatter:
            scatter('C');
            break;
        case State::frightened:
            frightened('C');
            break;
        case State::dead:
            dead('C');
            break;
    }
}

char PacMan::calcNext(char nextMove, int x, int y, int targetX, int targetY){
    int nextCellX, nextCellY;
    float lDist, rDist, uDist, dDist;
    int intLDist, intRDist, intUDist, intDDist, shortest;
    bool left = false, right = false, up = false, down = false;
    char shortestDir; 
    switch(nextMove){ //works out cell to be moved into
        case 'l':
            nextCellX = x - 1;
            if (nextCellX < 0){
                nextCellX = cellAcross_ - 1;
            }
            nextCellY = y;
            break;
        case 'r':
            nextCellX = x + 1;
            if (nextCellX >= cellAcross_){
                nextCellX = 0;
            }
            nextCellY = y;
            break;
        case 'u':                    
            nextCellX = x;
            nextCellY = y - 1;
            break;
        case 'd':                    
            nextCellX = x;
            nextCellY = y + 1;
            break;
    }
    if (turns[nextCellX][nextCellY] != 0){
        switch(nextMove){ //checks possible turns without reversing
            case 'l':
                left = validMove(nextCellX, nextCellY, 'l');
                up = validMove(nextCellX, nextCellY, 'u');
                down = validMove(nextCellX, nextCellY, 'd');
                break;
            case 'r':
                right = validMove(nextCellX, nextCellY, 'r');
                up = validMove(nextCellX, nextCellY, 'u');
                down = validMove(nextCellX, nextCellY, 'd');
                break;
            case 'u':                    
                left = validMove(nextCellX, nextCellY, 'l');
                right = validMove(nextCellX, nextCellY, 'r');
                up = validMove(nextCellX, nextCellY, 'u');
                break;
            case 'd':                    
                left = validMove(nextCellX, nextCellY, 'l');
                right = validMove(nextCellX, nextCellY, 'r');
                down = validMove(nextCellX, nextCellY, 'd');
                break;
        }
        if (left){ //calculates distance from possible moves to target to find shortest path
            lDist = sqrt(pow((nextCellX - 1) - targetX, 2) + pow(nextCellY - targetY, 2));
            lDist *= 10000000;
            intLDist = (int)lDist;
            shortest = intLDist;
            shortestDir = 'l';  
        }
        if (right){
            rDist = sqrt(pow((nextCellX + 1) - targetX, 2) + pow(nextCellY - targetY, 2));
            rDist *= 10000000;
            intRDist = (int)rDist;
            if (left){
                if (shortest > intRDist){
                    shortest = intRDist;
                    shortestDir = 'r';
                }
            }
            else{
                shortest = intRDist;
                shortestDir = 'r';
            }
        }
        if (up){
            uDist = sqrt(pow(nextCellX - targetX, 2) + pow((nextCellY - 1) - targetY, 2));
            uDist *= 10000000;
            intUDist = (int)uDist;
            if (left || right){
                if (shortest > intUDist){
                    shortest = intUDist;
                    shortestDir = 'u';
                }
            }
            else{
                shortest = intUDist;
                shortestDir = 'u';
            }
        }
        if (down){
            dDist = sqrt(pow(nextCellX - targetX, 2) + pow((nextCellY + 1) - targetY, 2));
            dDist *= 10000000;
            intDDist = (int)dDist;
            if (left || right || up){
                if (shortest > intDDist){
                    shortest = intDDist;
                    shortestDir = 'd';
                }
            }
            else{
                shortest = intDDist;
                shortestDir = 'd';
            }
        }
    }
    else{
        shortestDir = nextMove;
    }
    return shortestDir;
}

char PacMan::calcNext(char nextMove, int x, int y){ //same as previous, except no shortest calculations, just pseudo random movements
    int nextCellX, nextCellY;
    bool left = false, right = false, up = false, down = false;
    switch(nextMove){
        case 'l':
            nextCellX = x - 1;
            if (nextCellX < 0){
                nextCellX = cellAcross_ - 1;
            }
            nextCellY = y;
            break;
        case 'r':
            nextCellX = x + 1;
            if (nextCellX >= cellAcross_){
                nextCellX = 0;
            }
            nextCellY = y;
            break;
        case 'u':                    
            nextCellX = x;
            nextCellY = y - 1;
            break;
        case 'd':                    
            nextCellX = x;
            nextCellY = y + 1;
            break;
    }
    if (turns[nextCellX][nextCellY] != 0){
        switch(nextMove){
            case 'l':
                left = validMove(nextCellX, nextCellY, 'l');
                up = validMove(nextCellX, nextCellY, 'u');
                down = validMove(nextCellX, nextCellY, 'd');
                break;
            case 'r':
                right = validMove(nextCellX, nextCellY, 'r');
                up = validMove(nextCellX, nextCellY, 'u');
                down = validMove(nextCellX, nextCellY, 'd');
                break;
            case 'u':                    
                left = validMove(nextCellX, nextCellY, 'l');
                right = validMove(nextCellX, nextCellY, 'r');
                up = validMove(nextCellX, nextCellY, 'u');
                break;
            case 'd':                    
                left = validMove(nextCellX, nextCellY, 'l');
                right = validMove(nextCellX, nextCellY, 'r');
                down = validMove(nextCellX, nextCellY, 'd');
                break;
        }
        if (up){
            return 'u';
        }
        else if (left){
            return 'l';
        }
        else if (down){
            return 'd';
        }
        else if (right){
            return 'r';
        }
    }
    return nextMove;
}

bool PacMan::validMove(int x, int y, char move){
    switch(move){
        case 'l':
            if (!(x == 0 && y == 17)){ //check for teleport across board
                if (grid[x - 1][y] != 'w'){
                    return true;
                }
            }
            else{
                return true;
            }     
            break;
        case 'r':
            if (!(x == 27 && y == 17)){ //check for teleport across board
                if (grid[x + 1][y] != 'w'){
                    return true;
                }
            }
            else{
                return true;
            }
            break;
        case 'u':
            if (grid[x][y - 1] != 'w'){
                return true;
            }
            break;
        case 'd':
            if (grid[x][y + 1] != 'w' && grid[x][y + 1] != 'd'){
                return true;
            }
            break;
    }
    return false;
}

void PacMan::checkCell(int x, int y){ //checks cell contents and executes commands as appropriate
    switch(grid[x][y]){
        case 'p':
            score += 10;
            pEaten++;
            break;
        case 'e':
            score += 50;
            eEaten++;
            frightened();
            break;
        case 'c':
            if (cherry == true){
                score += 100;
                cherry = false;
            }
            break;
    }
    drawGrid[x][y] = 'U'; //draws pacman in current cell
    //toggling cherry
    if (cherry){
        if (cherryTimer >= 30){
            cherry = false;
        }
        cherryTimer++;
    }
    if ((pEaten == 70 && !cherry) || (pEaten == 170 && !cherry)){
        spawnCherry();
    }
}

bool PacMan::checkWinOrLose(){
    if (pEaten == 240 && eEaten == 4){ //win
        win = true;
    }
    if (pacman.getX() == blinky.getX() && pacman.getY() == blinky.getY() && (blinky.getState() == State::chase || blinky.getState() == State::scatter)){ //lose
        lose = true;
        roundsLost++;        
    }
    else if (pacman.getX() == blinky.getX() && pacman.getY() == blinky.getY() && blinky.getState() == State::frightened){
        blinky.setState(State::dead, false); //pacman eats ghost (not win or lose but efficient place to put commands)
        score += ghostPoints;
        ghostPoints *= 2;
    }
    else if (pacman.getX() == pinky.getX() && pacman.getY() == pinky.getY() && (pinky.getState() == State::chase || pinky.getState() == State::scatter)){
        lose = true;
        roundsLost++;
    }
    else if (pacman.getX() == pinky.getX() && pacman.getY() == pinky.getY() && pinky.getState() == State::frightened){
        pinky.setState(State::dead, false);
        score += ghostPoints;
        ghostPoints *= 2;
    }
    else if (pacman.getX() == inky.getX() && pacman.getY() == inky.getY() && (inky.getState() == State::chase || inky.getState() == State::scatter)){
        lose = true;
        roundsLost++;
    }
    else if (pacman.getX() == inky.getX() && pacman.getY() == inky.getY() && inky.getState() == State::frightened){
        inky.setState(State::dead, false);
        score += ghostPoints;
        ghostPoints *= 2;
    }
    else if (pacman.getX() == clyde.getX() && pacman.getY() == clyde.getY() && (clyde.getState() == State::chase || clyde.getState() == State::scatter)){
        lose = true;
        roundsLost++;
    }
    else if (pacman.getX() == clyde.getX() && pacman.getY() == clyde.getY() && clyde.getState() == State::frightened){
        clyde.setState(State::dead, false);
        score += ghostPoints;
        ghostPoints *= 2;
    }
    if(roundsLost >= 3){ //game over
        gameOver = true;
    }
    if (win || lose || gameOver){
        return true;
    }
    return false;
}

void PacMan::reset(){
    drawGrid[pacman.getX()][pacman.getY()] = 'b'; //removes entities
    grid[pacman.getX()][pacman.getY()] = 'b';
    drawGrid[blinky.getX()][blinky.getY()] = grid[blinky.getX()][blinky.getY()]; 
    drawGrid[pinky.getX()][pinky.getY()] = grid[pinky.getX()][pinky.getY()];
    drawGrid[inky.getX()][inky.getY()] = grid[inky.getX()][inky.getY()];
    drawGrid[clyde.getX()][clyde.getY()] = grid[clyde.getX()][clyde.getY()];
    currentPos(); //replaces entities in starting positions
    setMoving(); //toggles moving
    lose = false;
    starting = true;
    outTimer = 0;
    outTimerActive = true;
    gameTimer = 0;
    gameTimerActive = true;
    frightTimer = 0;
    frightTimerActive = false;
    ghostPoints = 200;
    gameState = State::scatter;
    pacman.reset(); //resets entity information
    blinky.reset();
    pinky.reset();
    inky.reset();
    clyde.reset();
}

void PacMan::endWinScreen(){ //removes all pellets, energisers, cherries and ghosts
    for (int y = 0; y < cellDown_; y++){
        for (int x = 0; x < cellAcross_; x++){
            switch(drawGrid[x][y]){
                case 'p':
                    drawGrid[x][y] = 'b';
                    break;
                case 'e':
                    drawGrid[x][y] = 'b';
                    break;
                case 'c':
                    drawGrid[x][y] = 'b';
                    break;
                case 'B':
                    drawGrid[x][y] = 'b';
                    break;
                case 'P':
                    drawGrid[x][y] = 'b';
                    break;
                case 'I':
                    drawGrid[x][y] = 'b';
                    break;
                case 'C':
                    drawGrid[x][y] = 'b';
                    break;
            }
        }
    }
}

void PacMan::endLoseScreen(){ //removes all pellets, energisers, cherries and pacman
    for (int y = 0; y < cellDown_; y++){
        for (int x = 0; x < cellAcross_; x++){
            switch(drawGrid[x][y]){
                case 'p':
                    drawGrid[x][y] = 'b';
                    break;
                case 'e':
                    drawGrid[x][y] = 'b';
                    break;
                case 'c':
                    drawGrid[x][y] = 'b';
                    break;
                case 'U':
                    drawGrid[x][y] = 'b';
                    break;
            }
        }
    }
}

void PacMan::reverse(){
    if (blinky.getState() != State::home && blinky.getState() != State::dead){ //reverses direction then decides on move after that
        blinky.setNextMove(blinky.getReverse());
        blinky.setDecision(calcNext(blinky.getNextMove(), blinky.getX(), blinky.getY(), blinky.getTargetX(), blinky.getTargetY()));
    }
    if (pinky.getState() != State::home && pinky.getState() != State::dead){
        pinky.setNextMove(pinky.getReverse());
        pinky.setDecision(calcNext(pinky.getNextMove(), pinky.getX(), pinky.getY(), pinky.getTargetX(), pinky.getTargetY()));
    }
    if (inky.getState() != State::home && inky.getState() != State::dead){
        inky.setNextMove(inky.getReverse());
        inky.setDecision(calcNext(inky.getNextMove(), inky.getX(), inky.getY(), inky.getTargetX(), inky.getTargetY()));
    }
    if (clyde.getState() != State::home && clyde.getState() != State::dead){
        clyde.setNextMove(clyde.getReverse());
        clyde.setDecision(calcNext(clyde.getNextMove(), clyde.getX(), clyde.getY(), clyde.getTargetX(), clyde.getTargetY()));
    }
}

void PacMan::chase(char name){ //chasing pacman, most seen behaviour
    switch(name){
        case 'B':
            if (grid[blinky.getX()][blinky.getY()] != 'U'){ //removing ghost
                drawGrid[blinky.getX()][blinky.getY()] = grid[blinky.getX()][blinky.getY()];
            }
            else{
                drawGrid[blinky.getX()][blinky.getY()] = 'b';
            }
            blinky.move();
            drawGrid[blinky.getX()][blinky.getY()] = 'B'; //redrawing ghost in new location
            blinky.setTarget(pacman.getX(), pacman.getY()); //recalculates target
            blinky.setDecision(calcNext(blinky.getNextMove(), blinky.getX(), blinky.getY(), blinky.getTargetX(), blinky.getTargetY())); //sets decision to move after 
            break;
        case 'P':
            if (grid[pinky.getX()][pinky.getY()] != 'U'){
                drawGrid[pinky.getX()][pinky.getY()] = grid[pinky.getX()][pinky.getY()];
            }
            else{
                drawGrid[pinky.getX()][pinky.getY()] = 'b';
            }
            pinky.move();
            drawGrid[pinky.getX()][pinky.getY()] = 'P';
            pinky.setTarget(pacman.getX(), pacman.getY(), pacman.getCurMove());
            pinky.setDecision(calcNext(pinky.getNextMove(), pinky.getX(), pinky.getY(), pinky.getTargetX(), pinky.getTargetY()));
            break;
        case 'I':
            if (grid[inky.getX()][inky.getY()] != 'U'){
                drawGrid[inky.getX()][inky.getY()] = grid[inky.getX()][inky.getY()];
            }
            else{
                drawGrid[inky.getX()][inky.getY()] = 'b';
            }
            inky.move();
            drawGrid[inky.getX()][inky.getY()] = 'I';
            inky.setTarget(pacman.getX(), pacman.getY(), pacman.getCurMove(), blinky.getX(), blinky.getY());
            inky.setDecision(calcNext(inky.getNextMove(), inky.getX(), inky.getY(), inky.getTargetX(), inky.getTargetY()));
            break;
        case 'C':
            if (grid[clyde.getX()][clyde.getY()] != 'U'){
                drawGrid[clyde.getX()][clyde.getY()] = grid[clyde.getX()][clyde.getY()];
            }
            else{
                drawGrid[clyde.getX()][clyde.getY()] = 'b';
            }
            clyde.move();
            drawGrid[clyde.getX()][clyde.getY()] = 'C';
            clyde.setTarget(pacman.getX(), pacman.getY());
            clyde.setDecision(calcNext(clyde.getNextMove(), clyde.getX(), clyde.getY(), clyde.getTargetX(), clyde.getTargetY()));
            break;
    }
}

void PacMan::frightened(){ //start of frightened state
    gameTimerActive = false;
    frightTimer = 0;
    frightTimerActive = true;
    updateState(State::frightened);
    reverse();
}

void PacMan::frightened(char name){ //frightened behaviour and movements
    switch(name){
        case 'B':
            if (grid[blinky.getX()][blinky.getY()] != 'U'){
                drawGrid[blinky.getX()][blinky.getY()] = grid[blinky.getX()][blinky.getY()];
            }
            else{
                drawGrid[blinky.getX()][blinky.getY()] = 'b';
            }
            blinky.move();
            drawGrid[blinky.getX()][blinky.getY()] = 'B';
            blinky.setDecision(calcNext(blinky.getNextMove(), blinky.getX(), blinky.getY())); //pseudo random move after next
            break;
        case 'P':
            if (grid[pinky.getX()][pinky.getY()] != 'U'){
                drawGrid[pinky.getX()][pinky.getY()] = grid[pinky.getX()][pinky.getY()];
            }
            else{
                drawGrid[pinky.getX()][pinky.getY()] = 'b';
            }
            pinky.move();
            drawGrid[pinky.getX()][pinky.getY()] = 'P';
            pinky.setDecision(calcNext(pinky.getNextMove(), pinky.getX(), pinky.getY()));
            break;
        case 'I':
            if (grid[inky.getX()][inky.getY()] != 'U'){
                drawGrid[inky.getX()][inky.getY()] = grid[inky.getX()][inky.getY()];
            }
            else{
                drawGrid[inky.getX()][inky.getY()] = 'b';
            }
            inky.move();
            drawGrid[inky.getX()][inky.getY()] = 'I';
            inky.setDecision(calcNext(inky.getNextMove(), inky.getX(), inky.getY()));
            break;
        case 'C':
            if (grid[clyde.getX()][clyde.getY()] != 'U'){
                drawGrid[clyde.getX()][clyde.getY()] = grid[clyde.getX()][clyde.getY()];
            }
            else{
                drawGrid[clyde.getX()][clyde.getY()] = 'b';
            }
            clyde.move();
            drawGrid[clyde.getX()][clyde.getY()] = 'C';
            clyde.setDecision(calcNext(clyde.getNextMove(), clyde.getX(), clyde.getY()));
            break;
    }
}

void PacMan::scatter(char name){ //scattering away from pacman to corners, second most seen behaviour
    switch(name){
        case 'B':
            if (grid[blinky.getX()][blinky.getY()] != 'U'){
                drawGrid[blinky.getX()][blinky.getY()] = grid[blinky.getX()][blinky.getY()];
            }
            else{
                drawGrid[blinky.getX()][blinky.getY()] = 'b';
            }
            blinky.move();
            drawGrid[blinky.getX()][blinky.getY()] = 'B';
            blinky.setTarget(blinky.getScatterX(), blinky.getScatterY()); //change in target cell
            blinky.setDecision(calcNext(blinky.getNextMove(), blinky.getX(), blinky.getY(), blinky.getTargetX(), blinky.getTargetY()));
            break;
        case 'P':
            if (grid[pinky.getX()][pinky.getY()] != 'U'){
                drawGrid[pinky.getX()][pinky.getY()] = grid[pinky.getX()][pinky.getY()];
            }
            else{
                drawGrid[pinky.getX()][pinky.getY()] = 'b';
            }
            pinky.move();
            drawGrid[pinky.getX()][pinky.getY()] = 'P';
            pinky.setTarget(pinky.getScatterX(), pinky.getScatterY(), ' ');
            pinky.setDecision(calcNext(pinky.getNextMove(), pinky.getX(), pinky.getY(), pinky.getTargetX(), pinky.getTargetY()));
            break;
        case 'I':
            if (grid[inky.getX()][inky.getY()] != 'U'){
                drawGrid[inky.getX()][inky.getY()] = grid[inky.getX()][inky.getY()];
            }
            else{
                drawGrid[inky.getX()][inky.getY()] = 'b';
            }
            inky.move();
            drawGrid[inky.getX()][inky.getY()] = 'I';
            inky.setTarget(inky.getScatterX(), inky.getScatterY(), ' ', 0, 0);
            inky.setDecision(calcNext(inky.getNextMove(), inky.getX(), inky.getY(), inky.getTargetX(), inky.getTargetY()));
            break;
        case 'C':
            if (grid[clyde.getX()][clyde.getY()] != 'U'){
                drawGrid[clyde.getX()][clyde.getY()] = grid[clyde.getX()][clyde.getY()];
            }
            else{
                drawGrid[clyde.getX()][clyde.getY()] = 'b';
            }
            clyde.move();
            drawGrid[clyde.getX()][clyde.getY()] = 'C';
            clyde.setTarget(clyde.getScatterX(), clyde.getScatterY());
            clyde.setDecision(calcNext(clyde.getNextMove(), clyde.getX(), clyde.getY(), clyde.getTargetX(), clyde.getTargetY()));
            break;
    }
}

void PacMan::dead(char name){ //if eaten by pacman
    for (int i = 0; i < 2; i++){ //runs twice, returns home at twice the speed
        switch(name){
            case 'B':
                if (blinky.getX() == spawnX && blinky.getY() == spawnY){ //if at the spawn cell (target), teleport into home
                    drawGrid[blinky.getX()][blinky.getY()] = 'b';
                    blinky.setCurrent(blinky.getHomeX(), blinky.getHomeY());
                    drawGrid[blinky.getX()][blinky.getY()] = 'B';
                    blinky.setState(State::home, true);
                }
                else if (blinky.getState() != State::home){
                    if (grid[blinky.getX()][blinky.getY()] != 'U'){
                        drawGrid[blinky.getX()][blinky.getY()] = grid[blinky.getX()][blinky.getY()];
                    }
                    else{
                        drawGrid[blinky.getX()][blinky.getY()] = 'b';
                    }
                    blinky.move();
                    drawGrid[blinky.getX()][blinky.getY()] = 'B';
                    blinky.setTarget(spawnX, spawnY);
                    blinky.setDecision(calcNext(blinky.getNextMove(), blinky.getX(), blinky.getY(), blinky.getTargetX(), blinky.getTargetY()));
                    }
                break;
            case 'P':
                if (pinky.getX() == spawnX && pinky.getY() == spawnY){
                    drawGrid[pinky.getX()][pinky.getY()] = 'b';
                    pinky.setCurrent(pinky.getHomeX(), pinky.getHomeY());
                    drawGrid[pinky.getX()][pinky.getY()] = 'P';
                    pinky.setState(State::home, true);
                }
                else if (pinky.getState() != State::home){
                    if (grid[pinky.getX()][pinky.getY()] != 'U'){
                        drawGrid[pinky.getX()][pinky.getY()] = grid[pinky.getX()][pinky.getY()];
                    }
                    else{
                        drawGrid[pinky.getX()][pinky.getY()] = 'b';
                    }
                    pinky.move();
                    drawGrid[pinky.getX()][pinky.getY()] = 'P';
                    pinky.setTarget(spawnX, spawnY, ' ');
                    pinky.setDecision(calcNext(pinky.getNextMove(), pinky.getX(), pinky.getY(), pinky.getTargetX(), pinky.getTargetY()));
                    }
                break;
            case 'I':
                if (inky.getX() == spawnX && inky.getY() == spawnY){
                    drawGrid[inky.getX()][inky.getY()] = 'b';
                    inky.setCurrent(inky.getHomeX(), inky.getHomeY());
                    drawGrid[inky.getX()][inky.getY()] = 'I';
                    inky.setState(State::home, true);
                }
                else if (inky.getState() != State::home){
                    if (grid[inky.getX()][inky.getY()] != 'U'){
                        drawGrid[inky.getX()][inky.getY()] = grid[inky.getX()][inky.getY()];
                    }
                    else{
                        drawGrid[inky.getX()][inky.getY()] = 'b';
                    }
                    inky.move();
                    drawGrid[inky.getX()][inky.getY()] = 'I';
                    inky.setTarget(spawnX, spawnY, ' ', 0, 0);
                    inky.setDecision(calcNext(inky.getNextMove(), inky.getX(), inky.getY(), inky.getTargetX(), inky.getTargetY()));
                    }
                break;
            case 'C':
                if (clyde.getX() == spawnX && clyde.getY() == spawnY){
                    drawGrid[clyde.getX()][clyde.getY()] = 'b';
                    clyde.setCurrent(clyde.getHomeX(), clyde.getHomeY());
                    drawGrid[clyde.getX()][clyde.getY()] = 'C';
                    clyde.setState(State::home, true);
                }
                else if (clyde.getState() != State::home){
                    if (grid[clyde.getX()][clyde.getY()] != 'U'){
                        drawGrid[clyde.getX()][clyde.getY()] = grid[clyde.getX()][clyde.getY()];
                    }
                    else{
                        drawGrid[clyde.getX()][clyde.getY()] = 'b';
                    }
                    clyde.move();
                    drawGrid[clyde.getX()][clyde.getY()] = 'C';
                    clyde.setTarget(spawnX, spawnY);
                    clyde.setDecision(calcNext(clyde.getNextMove(), clyde.getX(), clyde.getY(), clyde.getTargetX(), clyde.getTargetY()));
                    }
                break;
        }
    }
}

void PacMan::home(char name){
    switch(name){
        case 'B':
            if (outTimer >= 3){ //blinky moves out in 3 ticks
                drawGrid[blinky.getX()][blinky.getY()] = 'b';
                blinky.setCurrent(spawnX, spawnY);
                drawGrid[blinky.getX()][blinky.getY()] = 'B';
                if (blinky.getLastState() != State::dead){
                    blinky.setState(gameState, true); //comes out as frightened if other ghosts are
                }
                else{
                    if (lastGameState != State::frightened){
                        blinky.setState(lastGameState, true);
                    }
                    else{
                        blinky.setState(gameState, true);
                    }
                }
                outTimer = 0; //once out, it resets
                outTimerActive = false;
            }
            if (!outTimerActive){
                outTimerActive = true;
            }
            break;
        case 'P':
            if (outTimer >= 3 && blinky.getState() != State::home){ //pinky also three ticks if blinky isnt already in the home
                drawGrid[pinky.getX()][pinky.getY()] = 'b';
                pinky.setCurrent(spawnX, spawnY);
                drawGrid[pinky.getX()][pinky.getY()] = 'P';
                if (pinky.getLastState() != State::dead){
                    pinky.setState(gameState, true);
                }
                else{
                    if (lastGameState != State::frightened){
                        pinky.setState(lastGameState, true);
                    }
                    else{
                        pinky.setState(gameState, true);
                    }
                }
                outTimer = 0;
                outTimerActive = false;
            }
            if (!outTimerActive && blinky.getState() != State::home){ //activates if blinky is not in the home
                outTimerActive = true;
            }
            break;
        case 'I':
            if (outTimer >= 5){ //inky 5 ticks (like turning the corner in the home and moving out)
                drawGrid[inky.getX()][inky.getY()] = 'b';
                inky.setCurrent(spawnX, spawnY);
                drawGrid[inky.getX()][inky.getY()] = 'I';
                if (inky.getLastState() != State::dead){
                    inky.setState(gameState, true);
                }
                else{
                    if (lastGameState != State::frightened){
                        inky.setState(lastGameState, true);
                    }
                    else{
                        inky.setState(gameState, true);
                    }
                }
                outTimer = 0;
                outTimerActive = false;
            }
            if (pEaten >= 30 && !outTimerActive && pinky.getState() != State::home){ //activates if pinky not in home and 30 pellets eaten
                outTimerActive = true;
            }
            break;
        case 'C':
            if (outTimer >= 6){ //clyde 6 ticks (like turning the corner in the home and moving out)
                drawGrid[clyde.getX()][clyde.getY()] = 'b';
                clyde.setCurrent(spawnX, spawnY);
                drawGrid[clyde.getX()][clyde.getY()] = 'C';
                if (clyde.getLastState() != State::dead){
                    clyde.setState(gameState, true);
                }
                else{
                    if (lastGameState != State::frightened){
                        clyde.setState(lastGameState, true);
                    }
                    else{
                        clyde.setState(gameState, true);
                    }
                }
                outTimer = 0;
                outTimerActive = false;
            }
            if (pEaten >= 80 && !outTimerActive && inky.getState() != State::home){ //activates if inky not in home and 80 pellets eaten
                outTimerActive = true;
            }
            break;
    }
}

void PacMan::spawnCherry(){
    cherry = true;
    cherryTimer = 0;
}

void PacMan::updateState(State update){
    if (gameState != update){
        lastGameState = gameState;
    }
    gameState = update;
    blinky.setState(update, false);
    pinky.setState(update, false);
    inky.setState(update, false);
    clyde.setState(update, false);
}

//getters and setters
char PacMan::getGridValue(int x, int y){
    return drawGrid[x][y];
}

int PacMan::getCellAcross(){
    return cellAcross_;
}

int PacMan::getCellDown(){
    return cellDown_;
}

bool PacMan::getStarting(){
    return starting;
}

int PacMan::getScore(){
    return score;
}

bool PacMan::getCherry(){
    return cherry;
}

bool PacMan::getMoving(){
    return moving;
}

bool PacMan::getPaused(){
    return paused;
}

int PacMan::getPEaten(){
    return pEaten;
}

int PacMan::getEEaten(){
    return eEaten;
}

int PacMan::getRoundsLost(){
    return roundsLost;
}

int PacMan::getFrightTimer(){
    return frightTimer;
}

bool PacMan::getWin(){
    return win;
}

bool PacMan::getLose(){
    return lose;
}

bool PacMan::getGameOver(){
    return gameOver;
}

State PacMan::getGameState(){
    return gameState;
}

void PacMan::setMoving(){
    moving = !moving; //toggle moving
}