const int cellAcross_ = 28; //size of grid across
const int cellDown_ = 36; //size of grid 

enum State {home, chase, scatter, frightened, dead}; //ghost states

class Entity{
    protected:
        int x_, y_; //current location
        char curMove; //current movement direction
        char nextMove; //next movement direction
        bool ghost; //ghost or Pacman
    public:
        Entity(){
            nextMove = 'l';
        }

        virtual void move(){
            switch(getCurMove()){
                case 'l':
                    if (!(getX() == 0 && getY() == 17)){
                        setCurrent(getX() - 1, getY());
                    }
                    else{
                        setCurrent(27,17); //teleport across board
                    }
                    break;
                case 'r':
                    if (!(getX() == 27 && getY() == 17)){
                        setCurrent(getX() + 1, getY());
                    }
                    else{
                        setCurrent(0,17); //teleport across board
                    }
                    break;
                case 'u':                    
                    setCurrent(getX(), getY() - 1);
                    break;
                case 'd':                    
                    setCurrent(getX(), getY() + 1);
                    break;
            }
        }

        virtual void reset(){
            nextMove = 'l';
        }

        void setCurrent(int x, int y){
            x_ = x;
            y_ = y;
        }

        virtual void setNextMove(char move){
            nextMove = move;
        }

        void setCurMove(){
            curMove = nextMove; //current move will only be what the next move is
        }

        char getReverse(){
            switch(curMove){
                case 'l':
                    return 'r';
                    break;
                case 'r':
                    return 'l';
                    break;
                case 'u':
                    return 'd';
                    break;
                case 'd':
                    return 'u';
                    break;
                default:
                    return ' ';
                    break;
            }
        }

        int getX(){
            return x_;
        }

        int getY(){
            return y_;
        }

        char getCurMove(){
            return curMove;
        }

        char getNextMove(){
            return nextMove;
        }

        bool getGhost(){
            return ghost;
        }
};

class Pacman: public Entity{
    private:
        bool moveChange; //user input for change, requires verification of legal move
    public:
        Pacman(){
            curMove = 'l';
            moveChange = false;
            ghost = false;   
        }

        void setNextMove(char move) override{
            Entity::setNextMove(move);
            moveChange = true;
        }

        void reset() override{
            Entity::reset();
            curMove = 'l';
            moveChange = false;
        }

        bool getMoveChange(){
            return moveChange;
        }     
};

class Ghost: public Entity{
    protected:
        int targetX, targetY; //ghost target location
        State state_, lastState; //ghost state and previous state (when changed)
        char decision; //decision on if it turns based on next tile
        int scatterX, scatterY; //target for scatter behaviour
        int homeX, homeY; //home position
    public:
        Ghost(){
            ghost = true;
            decision = 'l';
        }

        virtual void setTarget(int x, int y){//passed target location for verification
            if(x >= cellAcross_){
                x = cellAcross_ - 1;
            }
            else if(x < 0){
                x = 0;
            }
            if(y >= cellDown_){
                y = cellDown_ - 1;
            }
            else if(y < 0){
                y = 0;
            }
            targetX = x;
            targetY = y;
        }

        virtual void reset() override{
            Entity::reset();
            decision = 'l';
            lastState = home;
        }

        void setState(State state, bool override){ //check can be overriden in certaion scenarios
            if (state_ != dead && state_ != home || override){
                lastState = state_;
                state_ = state;
            }
        }

        int getTargetX(){
            return targetX;
        }

        int getTargetY(){
            return targetY;
        }

        int getScatterX(){
            return scatterX;
        }

        int getScatterY(){
            return scatterY;
        }

        int getHomeX(){
            return homeX;
        }

        int getHomeY(){
            return homeY;
        }

        State getState(){
            return state_;
        }

        State getLastState(){
            return lastState;
        }

        void setNextMove(){
            nextMove = decision; //next move will only be the decision from calculation...
        }

        void setNextMove(char move){ //...unless overwritten
            nextMove = move;
        }

        void setDecision(char move){
            decision = move;
        }

        void setHome(int x, int y){
            homeX = x;
            homeY = y;
        }

        void move() override{
            setCurMove();
            Entity::move();
            setNextMove();
        }
};

class Blinky: public Ghost{
    public:
        Blinky(){
            state_ = scatter;
            scatterX = 27;
            scatterY = 3;
        }

        void reset() override{
            Entity::reset();
            Ghost::reset();
            setState(scatter, false);
        }
};

class Pinky: public Ghost{
    public:
        Pinky(){
            state_ = home;
            scatterX = 0;
            scatterY = 3;
        }

        void reset() override{
            Entity::reset();
            Ghost::reset();
            setState(home, false);
        }

        void setTarget(int x, int y, char dir){ //passed location and current direction (if chasing pacman)
            if (state_ == chase){ //if chasing (therefore targetting 4 ahead of pacman)
                switch(dir){
                    case 'l':
                        x -= 4;
                        Ghost::setTarget(x, y);
                        break;
                    case 'r':
                        x += 4;
                        Ghost::setTarget(x, y);
                        break;
                    case 'u':                    
                        y -= 4;
                        Ghost::setTarget(x, y);
                        break;
                    case 'd':                    
                        y += 4;
                        Ghost::setTarget(x, y);
                        break;
                }
            }
            else{ //for scatter and dead
                Ghost::setTarget(x, y);
            }
        }
};

class Inky: public Ghost{
    public:
        Inky(){
            state_ = home;
            scatterX = 27;
            scatterY = 33;
        }

        void reset() override{
            Entity::reset();
            Ghost::reset();
            setState(home, false);
        }

        void setTarget(int x, int y, char dir, int bX, int bY){ //passed pacman location and direction and blinky location (if chasing pacman)
            if (state_ == chase){
                switch(dir){
                    case 'l':
                        x -= 2;
                        Ghost::setTarget(x, y); //for verificaton of cell, targets overwritten later
                        break;
                    case 'r':
                        x += 2;
                        Ghost::setTarget(x, y);
                        break;
                    case 'u':                    
                        y -= 2;
                        Ghost::setTarget(x, y);
                        break;
                    case 'd':                    
                        y += 2;
                        Ghost::setTarget(x, y);
                        break;
                }
                x = x + (x - bX); //target is blinkys location mirrored perpendicular on point two spaces infront of pacman
                y = y + (y - bY);
                Ghost::setTarget(x, y);
            }
            else{
                Ghost::setTarget(x, y);
            }
        }
};

class Clyde: public Ghost{
    public:
        Clyde(){
            state_ = home;
            scatterX = 0;
            scatterY = 32;
        }

        void reset() override{
            Entity::reset();
            Ghost::reset();
            setState(home, false);
        }

        void setTarget(int x, int y) override{ //runs away from pacman if closer than 8 cells, otherwise targets pacman
            int distance;
            if (state_ == chase){
                distance = (int)sqrt(pow(x_ - x, 2) + pow(y_ - y, 2));
                if (distance >= 8){
                    Ghost::setTarget(x, y);
                }
                else{
                    Ghost::setTarget(scatterX, scatterY);
                }
            }
            else{
                Ghost::setTarget(x, y);
            }
        }
};