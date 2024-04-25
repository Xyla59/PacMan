#include <iostream>
#include <iomanip>
#include <vector>
#include <cstring>
#include <tuple>
#include <cmath>
#include <string>

#include <context.hpp>
#include <app.hpp>
#include <..\assignment\PacMan.hpp>

using namespace std;

const int width = 560; 
const int height = 720;
const int cellSize = 20;

class MyApp: public uwe::App {
    private:
        PacMan myPacman; //main class
        uwe::Image text; //to create text on screen
        uwe::Image score, enter, ready, go, gameOver, youWin; //phrases to be displayed
        uwe::Image pacmanLeft, pacmanRight, pacmanUp, pacmanDown, blinkySprite, pinkySprite, inkySprite, clydeSprite, eyes, scaredBlue, scaredWhite, cherry; //sprites
        int scoreWidth, scoreHeight, enterWidth, enterHeight, readyWidth, readyHeight, goWidth, goHeight, gameOverWidth, gameOverHeight, youWinWidth, youWinHeight; //for image rectangles
        int PLW, PLH, PRW, PRH, PUW, PUH, PDW, PDH, BSW, BSH, PSW, PSH, ISW, ISH, CSW, CSH, eyesW, eyesH, SBW, SBH, SWW, SWH, cherryW, cherryH; //sprite image rect sizes
        uwe::Rect scoreSrc, scoreDest, enterSrc, enterDest, readySrc, readyDest, goSrc, goDest, gameOverSrc, gameOverDest, youWinSrc, youWinDest; //image rectangles for text
        uwe::Rect PLSrc, PLDest, PRSrc, PRDest, PUSrc, PUDest, PDSrc, PDDest, BSSrc, BSDest, PSSrc, PSDest, ISSrc, ISDest, CSSrc, CSDest, 
            eyesSrc, eyesDest, SBSrc, SBDest, SWSrc, SWDest, cherrySrc, cherryDest; //image rectangles for sprites
        bool drawGridLines = false, input = false, winTick = false; //for displaying gridlines, detecting user input and timing for a win scenario
        long tickTimer; //tick timer to slow down calculations and screen updates
        uwe::Colour walls = uwe::Colour::blue(); 
        bool flash = false;
    public:
        MyApp(int width, int height, std::string title);
        ~MyApp();

        void begin() override;
        void update() override;
        void draw() override;

        void key_pressed(uwe::Scancode scancode, bool repeat) override;

        void mouse_pressed(int x, int y, uwe::Button button) override;
        void mouse_released(int x, int y, uwe::Button button) override;
        void mouse_moved(int x, int y) override;

        uwe::Image render_text(std::string msg); //from shapes.cpp
        char getText(int x, int y);
        void drawWin();
        void drawLose();
        void movePacMan();
};

MyApp::MyApp(int width, int height, std::string title) {
    init(width, height, title);
}

MyApp::~MyApp() {

}

void MyApp::begin() {
    tickTimer = 0;
    //creating all needed images
    text = create_image("../assets/ZX_Spectrum_character_set_inv.png");
    score = render_text(to_string(myPacman.getScore()));
    enter = render_text("Press Enter!");
    ready = render_text("Ready!");
    go = render_text("Go!");
    gameOver = render_text("Game Over!");
    youWin = render_text("You Win!");
    pacmanLeft = create_image("../sprites/PacmanLeft.png");
    pacmanRight = create_image("../sprites/PacmanRight.png");
    pacmanUp = create_image("../sprites/PacmanUp.png");
    pacmanDown = create_image("../sprites/PacmanDown.png");
    blinkySprite = create_image("../sprites/Blinky.png");
    pinkySprite = create_image("../sprites/Pinky.png");
    inkySprite = create_image("../sprites/Inky.png");
    clydeSprite = create_image("../sprites/Clyde.png");
    eyes = create_image("../sprites/Eyes.png");
    scaredBlue = create_image("../sprites/ScaredBlue.png");
    scaredWhite = create_image("../sprites/ScaredWhite.png");
    cherry = create_image("../sprites/Cherry.png");
    get_image_size(score, &scoreWidth, &scoreHeight);
    get_image_size(enter, &enterWidth, &enterHeight);
    get_image_size(ready, &readyWidth, &readyHeight);
    get_image_size(go, &goWidth, &goHeight);
    get_image_size(gameOver, &gameOverWidth, &gameOverHeight);
    get_image_size(youWin, &youWinWidth, &youWinHeight);
    get_image_size(pacmanLeft, &PLW, &PLH);
    get_image_size(pacmanRight, &PRW, &PRH);
    get_image_size(pacmanUp, &PUW, &PUH);
    get_image_size(pacmanDown, &PDW, &PDH);
    get_image_size(blinkySprite, &BSW, &BSH);
    get_image_size(pinkySprite, &PSW, &PSH);
    get_image_size(inkySprite, &ISW, &ISH);
    get_image_size(clydeSprite, &CSW, &CSH);
    get_image_size(eyes, &eyesW, &eyesH);
    get_image_size(scaredBlue, &SBW, &SBH);
    get_image_size(scaredWhite, &SWW, &SWH);
    get_image_size(cherry, &cherryW, &cherryH);
    scoreSrc = uwe::make_rect(0, 0, scoreWidth, scoreHeight);
    enterSrc = uwe::make_rect(0, 0, enterWidth, enterHeight);
    readySrc = uwe::make_rect(0, 0, readyWidth, readyHeight);
    goSrc = uwe::make_rect(0, 0, goWidth, goHeight);
    gameOverSrc = uwe::make_rect(0, 0, gameOverWidth, gameOverHeight);
    youWinSrc = uwe::make_rect(0, 0, youWinWidth, youWinHeight);
    PLSrc = uwe::make_rect(0, 0, PLW, PLH);
    PRSrc = uwe::make_rect(0, 0, PRW, PRH);
    PUSrc = uwe::make_rect(0, 0, PUW, PUH);
    PDSrc = uwe::make_rect(0, 0, PDW, PDH);
    BSSrc = uwe::make_rect(0, 0, BSW, BSH);
    PSSrc = uwe::make_rect(0, 0, PSW, PSH);
    ISSrc = uwe::make_rect(0, 0, ISW, ISH);
    CSSrc = uwe::make_rect(0, 0, CSW, CSH);
    eyesSrc = uwe::make_rect(0, 0, eyesW, eyesH);
    SBSrc = uwe::make_rect(0, 0, SBW, SBH);
    SWSrc = uwe::make_rect(0, 0, SWW, SWH);
    cherrySrc = uwe::make_rect(0, 0, cherryW, cherryH);
    //sets current positions
    myPacman.currentPos();
}

void MyApp::movePacMan(){
    if (!(myPacman.getLose())){
        myPacman.movePacman();
        if (!myPacman.checkWinOrLose()){
            score = render_text(to_string(myPacman.getScore()));
            get_image_size(score, &scoreWidth, &scoreHeight);
            scoreSrc = uwe::make_rect(0, 0, scoreWidth, scoreHeight);
            if (!myPacman.getMoving()){
                myPacman.setMoving();
            }
        }
    }
    else{
        myPacman.reset();
        tickTimer = 0;
    }
}

void MyApp::update(){
    if (!(myPacman.getPaused() || myPacman.getStarting() || myPacman.getGameOver() || myPacman.getWin())){ 
        if (tickTimer % 200 == 0 && myPacman.getGameState() != State::frightened){ //pacmans normal tickcheck is lower, therefore he moves faster
            movePacMan();
        }
        else if (tickTimer % 150 == 0 && myPacman.getGameState() == State::frightened){ //Pacman moves faster when energised
            movePacMan();
        }
        if (tickTimer % 250 == 0 && myPacman.getGameState() != State::frightened){ //ghosts normal tickcheck is higher, therefore they move slower
            if (!(myPacman.getLose())){
                myPacman.moveGhosts();
                myPacman.moveBlinky();
                myPacman.movePinky();
                myPacman.moveInky();
                myPacman.moveClyde();
                myPacman.checkWinOrLose();
            }
            else{
                myPacman.reset();
                tickTimer = 0;
            }
        }
        else if (myPacman.getGameState() == State::frightened){ //ghost move slower if frightened
            if (!(myPacman.getLose())){
                if (tickTimer % 350 == 0){
                    myPacman.moveGhosts();
                }
                if (tickTimer % 350 == 0 && myPacman.blinky.getState() == State::frightened){ //is frightened = moves slower
                    myPacman.moveBlinky();
                }
                else if (tickTimer % 250 == 0 && myPacman.blinky.getState() != State::frightened){ //isn't frightened but other ghosts are = moves normal speed
                    myPacman.moveBlinky();
                }
                if (tickTimer % 350 == 0 && myPacman.pinky.getState() == State::frightened){
                    myPacman.movePinky();
                }
                else if (tickTimer % 250 == 0 && myPacman.pinky.getState() != State::frightened){
                    myPacman.movePinky();
                }
                if (tickTimer % 350 == 0 && myPacman.inky.getState() == State::frightened){
                    myPacman.moveInky();
                }
                else if (tickTimer % 250 == 0 && myPacman.inky.getState() != State::frightened){
                    myPacman.moveInky();
                }
                if (tickTimer % 350 == 0 && myPacman.clyde.getState() == State::frightened){
                    myPacman.moveClyde();
                }
                else if (tickTimer % 250 == 0 && myPacman.clyde.getState() != State::frightened){
                    myPacman.moveClyde();
                }
                myPacman.checkWinOrLose();
            }
            else{
                myPacman.reset();
                tickTimer = 0;
            }
            switch(myPacman.getFrightTimer()){ //switching between white and blue to show end of frightened mode
                case 17:
                    flash = true;
                    break;
                case 19:
                    flash = true;
                    break;
                case 21:
                    flash = true;
                    break;
                case 23:
                    flash = true;
                    break;
                case 25:
                    flash = true;
                    break;
                default:
                    flash = false;
                    break;
            }
        }

    }
    else if (myPacman.getGameOver()){ //game over screen
        myPacman.reset();
        myPacman.endLoseScreen();
        tickTimer = 0;
        walls = uwe::Colour::red();
    }
    else if (myPacman.getWin()){ //win screen
        myPacman.reset();
        myPacman.endWinScreen();
        if (!winTick){
            tickTimer = 0;
            winTick = true;
        }
        if (tickTimer == 0 || tickTimer == 800 || tickTimer == 1600 || tickTimer == 2400){
            walls = uwe::Colour::green();
        }
        else if (tickTimer == 400 || tickTimer == 1200 || tickTimer == 2000){
            walls = uwe::Colour::blue();
        }
    }
    tickTimer++;
}

char MyApp::getText(int x, int y){ //Figures out what text to display, only one can display at any given time
    if (myPacman.getStarting() && !input){
        enterDest = uwe::make_rect((x-3)*cellSize, y*cellSize, enterWidth, enterHeight);
        return 'e';
    }
    else if (myPacman.getWin()){
        youWinDest = uwe::make_rect((x-3)*cellSize, y*cellSize, youWinWidth, youWinHeight);
        return 'w';
    }
    else if (myPacman.getGameOver()){
        gameOverDest = uwe::make_rect((x-3)*cellSize, y*cellSize, gameOverWidth, gameOverHeight);
        return 'l';
    }
    else if (myPacman.getStarting() && input && tickTimer < 1200){
        readyDest = uwe::make_rect((x-2)*cellSize, y*cellSize, readyWidth, readyHeight);
        return 'r';
    }
    else if (input && tickTimer >= 1200 && tickTimer < 2400){
        goDest = uwe::make_rect(x*cellSize, y*cellSize, goWidth, goHeight);
        myPacman.play();
        return 'g';
    }
    return ' ';
}

void MyApp::key_pressed(uwe::Scancode scancode, bool repeat) {
    switch (scancode) {
        case uwe::Scancode::A:
        case uwe::Scancode::LEFT:
            if (!myPacman.getStarting()){
                myPacman.pacman.setNextMove('l');
                cout << "Left key / A" << endl;
            }
            break;
        case uwe::Scancode::D:
        case uwe::Scancode::RIGHT:
            if (!myPacman.getStarting()){
                myPacman.pacman.setNextMove('r');
                cout << "Right key / D" << endl;
            }
            break;
        case uwe::Scancode::W:
        case uwe::Scancode::UP:
            if (!myPacman.getStarting()){
                myPacman.pacman.setNextMove('u');
                cout << "Up key / W" << endl;
            }
            break;
        case uwe::Scancode::S:
        case uwe::Scancode::DOWN:
            if (!myPacman.getStarting()){
                myPacman.pacman.setNextMove('d');
                cout << "Down key / S" << endl;
            }
            break;
        case uwe::Scancode::G:
            drawGridLines = !drawGridLines; //toggles gridlines
            cout << "G" << endl;
            break;
        case uwe::Scancode::RETURN:
            if (myPacman.getStarting()){
                input = true;
                tickTimer = 0;
            }
            cout << "Return key" << endl;
            break;
        case uwe::Scancode::P:
            myPacman.pause();
            cout << "P" << endl;
            break;
        default: {
            // nothing to see here
        }
    }
}

void MyApp::mouse_pressed(int x, int y, uwe::Button button) {
}
    
void MyApp::mouse_released(int x, int y, uwe::Button button) {
}
    
void MyApp::mouse_moved(int x, int y) {
}

void MyApp::draw() {
    int livesLoop = 2; //to figure out how many lives to display
    clear(uwe::Colour::black());
    for (int y = 0; y < myPacman.getCellDown(); y++){ //Draws map and entities
        for (int x = 0; x < myPacman.getCellAcross(); x++){
            switch(myPacman.getGridValue(x, y)){
                case 'w': //wall
                    set_draw_color(walls);
                    draw_rect_fill(x*cellSize, y*cellSize, cellSize, cellSize);
                    break;
                case 'p': //pellet
                    set_draw_color(uwe::make_colour(255, 185, 175, 255));
                    draw_rect_fill(x*cellSize + cellSize/2, y*cellSize + cellSize/2, cellSize/10, cellSize/10);
                    break;
                case 'e': //energiser
                    set_draw_color(uwe::make_colour(255, 185, 175, 255));
                    draw_rect_fill(x*cellSize + cellSize/4 + 1, y*cellSize + cellSize/4 + 1, cellSize/2, cellSize/2);
                    break;
                case 'd': //door
                    set_draw_color(uwe::make_colour(255, 184, 255, 255));
                    draw_rect_fill(x*cellSize, y*cellSize + cellSize/4, cellSize, cellSize/2);
                    break;
                case 'U': //Pacman (User)
                    if(myPacman.getStarting()){
                        PLDest = uwe::make_rect(x*cellSize + cellSize/2, y*cellSize, cellSize, cellSize);
                        draw_image(pacmanLeft, PLSrc, PLDest);
                    }
                    else{
                        switch(myPacman.pacman.getCurMove()){
                            case 'l':
                                PLDest = uwe::make_rect(x*cellSize, y*cellSize, cellSize, cellSize);
                                draw_image(pacmanLeft, PLSrc, PLDest);
                                break;
                            case 'r':
                                PRDest = uwe::make_rect(x*cellSize, y*cellSize, cellSize, cellSize);
                                draw_image(pacmanRight, PRSrc, PRDest);
                                break;
                            case 'u':
                                PUDest = uwe::make_rect(x*cellSize, y*cellSize, cellSize, cellSize);
                                draw_image(pacmanUp, PUSrc, PUDest);
                                break;
                            case 'd':
                                PDDest = uwe::make_rect(x*cellSize, y*cellSize, cellSize, cellSize);
                                draw_image(pacmanDown, PDSrc, PDDest);
                                break;
                        }
                    }
                    break;
                case 'B': //Blinky
                    BSDest = uwe::make_rect(x*cellSize, y*cellSize, cellSize, cellSize);
                    if (myPacman.getStarting()){
                        BSDest = uwe::make_rect(x*cellSize + cellSize/2, y*cellSize, cellSize, cellSize);
                        draw_image(blinkySprite, BSSrc, BSDest);
                    }
                    else if (myPacman.blinky.getState() == State::frightened){
                        if (flash){
                            SWDest = uwe::make_rect(x*cellSize, y*cellSize, cellSize, cellSize);
                            draw_image(scaredWhite, SWSrc, SWDest);
                        }
                        else{
                            SBDest = uwe::make_rect(x*cellSize, y*cellSize, cellSize, cellSize);
                            draw_image(scaredBlue, SBSrc, SBDest);
                        }
                    }
                    else if (myPacman.blinky.getState() == State::dead){
                        eyesDest = uwe::make_rect(x*cellSize, y*cellSize, cellSize, cellSize);
                        draw_image(eyes, eyesSrc, eyesDest);
                    }
                    else{
                        draw_image(blinkySprite, BSSrc, BSDest);
                    }
                    break;                                        
                case 'I': //Inky                    
                    ISDest = uwe::make_rect(x*cellSize, y*cellSize, cellSize, cellSize);
                    if(myPacman.getStarting()){
                        ISDest = uwe::make_rect(x*cellSize + cellSize/2, y*cellSize, cellSize, cellSize);
                        draw_image(inkySprite, ISSrc, ISDest);
                    }
                    else if (myPacman.inky.getState() == State::frightened){
                        if (flash){
                            SWDest = uwe::make_rect(x*cellSize, y*cellSize, cellSize, cellSize);
                            draw_image(scaredWhite, SWSrc, SWDest);
                        }
                        else{
                            SBDest = uwe::make_rect(x*cellSize, y*cellSize, cellSize, cellSize);
                            draw_image(scaredBlue, SBSrc, SBDest);
                        }
                    }
                    else if (myPacman.inky.getState() == State::dead){
                        eyesDest = uwe::make_rect(x*cellSize, y*cellSize, cellSize, cellSize);
                        draw_image(eyes, eyesSrc, eyesDest);
                    }
                    else{
                        draw_image(inkySprite, ISSrc, ISDest);
                    }
                    break;
                case 'P': //Pinky
                    PSDest = uwe::make_rect(x*cellSize, y*cellSize, cellSize, cellSize);
                    if(myPacman.getStarting()){
                        PSDest = uwe::make_rect(x*cellSize + cellSize/2, y*cellSize, cellSize, cellSize);
                        draw_image(pinkySprite, PSSrc, PSDest);
                    }
                    else if (myPacman.pinky.getState() == State::frightened){
                        if (flash){
                            SWDest = uwe::make_rect(x*cellSize, y*cellSize, cellSize, cellSize);
                            draw_image(scaredWhite, SWSrc, SWDest);
                        }
                        else{
                            SBDest = uwe::make_rect(x*cellSize, y*cellSize, cellSize, cellSize);
                            draw_image(scaredBlue, SBSrc, SBDest);
                        }
                    }
                    else if (myPacman.pinky.getState() == State::dead){
                        eyesDest = uwe::make_rect(x*cellSize, y*cellSize, cellSize, cellSize);
                        draw_image(eyes, eyesSrc, eyesDest);
                    }
                    else{
                        draw_image(pinkySprite, PSSrc, PSDest);
                    }
                    break;
                case 'C': //Clyde
                    CSDest = uwe::make_rect(x*cellSize, y*cellSize, cellSize, cellSize);
                    if(myPacman.getStarting()){
                        CSDest = uwe::make_rect(x*cellSize + cellSize/2, y*cellSize, cellSize, cellSize);
                        draw_image(clydeSprite, CSSrc, CSDest);
                    }
                    else if (myPacman.clyde.getState() == State::frightened){
                        if (flash){
                            SWDest = uwe::make_rect(x*cellSize, y*cellSize, cellSize, cellSize);
                            draw_image(scaredWhite, SWSrc, SWDest);
                        }
                        else{
                            SBDest = uwe::make_rect(x*cellSize, y*cellSize, cellSize, cellSize);
                            draw_image(scaredBlue, SBSrc, SBDest);
                        }
                    }
                    else if (myPacman.clyde.getState() == State::dead){
                        eyesDest = uwe::make_rect(x*cellSize, y*cellSize, cellSize, cellSize);
                        draw_image(eyes, eyesSrc, eyesDest);
                    }
                    else{
                        draw_image(clydeSprite, CSSrc, CSDest);
                    } 
                    break;                                                            
                case 'l': //lives
                    if(livesLoop > myPacman.getRoundsLost()){
                        PLDest = uwe::make_rect(x*cellSize, y*cellSize, cellSize, cellSize);
                        draw_image(pacmanLeft, PLSrc, PLDest);
                        livesLoop--;
                    }
                    break;   
                case 'c': //cherry
                    if(myPacman.getCherry()){
                        cherryDest = uwe::make_rect(x*cellSize, y*cellSize, cellSize, cellSize);
                        draw_image(cherry, cherrySrc, cherryDest);
                    }
                    break;
                case 's': //score
                    if(myPacman.getScore() >= 10000){
                        scoreDest = uwe::make_rect((x-4)*cellSize, y*cellSize, scoreWidth, scoreHeight);
                    }
                    else if(myPacman.getScore() >= 1000){
                        scoreDest = uwe::make_rect((x-3)*cellSize, y*cellSize, scoreWidth, scoreHeight);
                    }
                    else if(myPacman.getScore() >= 100){
                        scoreDest = uwe::make_rect((x-2)*cellSize, y*cellSize, scoreWidth, scoreHeight);
                    }
                    else if(myPacman.getScore() >= 10){
                        scoreDest = uwe::make_rect((x-1)*cellSize, y*cellSize, scoreWidth, scoreHeight);
                    }
                    else{
                        scoreDest = uwe::make_rect(x*cellSize, y*cellSize, scoreWidth, scoreHeight);
                    }                                        
                    draw_image(score, scoreSrc, scoreDest);
                    break;
                case 't': //text output (other than score)
                    switch(getText(x, y)){
                        case 'e':
                            draw_image(enter, enterSrc, enterDest);
                            break;
                        case 'r':
                            draw_image(ready, readySrc, readyDest);
                            break;
                        case 'g':
                            draw_image(go, goSrc, goDest);
                            break;
                        case 'w':
                            draw_image(youWin, youWinSrc, youWinDest);
                            break;
                        case 'l':
                            draw_image(gameOver, gameOverSrc, gameOverDest);
                            break;
                    }
                    break;
            }
        }
    }
    if(drawGridLines){
        //Draws gridlines 
        set_draw_color(uwe::Colour::green());
        for (int y = 0; y < myPacman.getCellDown(); y++){
            if (y != 0){
                draw_line(0, y*cellSize, width, y*cellSize);
            }
        }
        for (int x = 0; x < myPacman.getCellAcross(); x++){
            if (x != 0){
                draw_line(x*cellSize, 0, x*cellSize, height);
            }
        }
    }
}

uwe::Image MyApp::render_text(std::string msg) {//From Shapes.cpp
    int max_width = 0;
	int num_lines = 1;
	int current_width = 0;

	for (auto const c : msg) {
		if (c != '\n')
			current_width++;
		else {
			num_lines++;
			if (current_width > max_width)
				max_width = current_width;
			current_width = 0;
		}
	}
	if (current_width > max_width) {
		max_width = current_width;
    }
	int text_width = max_width * 16;
	int text_height = num_lines * 16;

    auto image = create_image(text_width, text_height);

    set_draw_color(uwe::Colour::white());
    set_renderer_taret(image, true);

    int x = 0; int y = 0;
	for (auto const c : msg) {
		if (c != '\n') {
			// Figure out what character it is
			int code = c - 32;
			if (c == '$') {
				code = 64;
            }
			uwe::Rect src{ (code % 16)*16, (code / 16)*16, 16, 16 };
			uwe::Rect dest{ x, y, 16, 16 };
			draw_image(text, src, dest);
			x += 16;
		} else {
			x = 0;
			y+=16;
		}
	}

    set_renderer_taret(nullptr, true);

    return image;
}

int main(int argc, char *argv[]) {
    uwe::App* app = new MyApp{width, height, "PacMan"};

    app->run();

    return 0;
}