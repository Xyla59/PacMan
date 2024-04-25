Pac-Man Project - Xyla Oldale

My project is a recreation of the original Pac-Man game, more specifically level 1. It includes the same characteristics and behaviours of the ghosts and pacman, aswell as the original game board and scoring system. The basic ghost AI is also replecated in my code. I will give a break down of how Pac-Man works and the different behaviour states, then the structure of my code.

The Basics:

Controls:
Up arrow / W = move up
Left arrow / A = move left
Down arrow / S = move down
Right arrow / D = move right
Return = start game (only works when game is on starting screen)
P = toggles pause (only works after game has started)
G = toggles gridlines


Pac-Man: 
The user controls Pac-man using the arrow keys. Once a key has been pressed, pac-man will move in that direction once there isn't a wall in his way. This means you can press an arrow key before a turn and Pacman will turn when he reaches the turning. Pacman must navigate the maze, eating all pellets and energisers to win. If he touches a ghost that isnt frightened, he loses a life. Lose 3 lives, its game over. 

Ghosts: 
There are 4 ghosts: Blinky (red), Pinky (pink), Inky (blue) and Clyde (orange), each with different targetting AI when chasing pacman. The ghosts have 5 behaviour states: Chase (where they target Pacman or their own designated target cell), Scatter (where they will scatter to the corners of the board for a short time), Frightened (after eating an energiser, Pacman can eat ghosts for bonus points), Dead (once eaten by Pacman, they are sent back to the ghost house for respawn) and Home (if they are in the ghost house). They switch between Chase and Scatter based on time playing and the others are used based on if the ghost is either in the home or Pacman creates the change. The rule for the Chase/Scatter changes are:

Scatter 7 seconds
Chase 20 seconds
Scatter 7
Chase 20
Scatter 5
Chase 20
Scatter 5
Chase indefinitely

The ghosts are slower than Pacman by default, and even more so when frightened (The ghosts slow down and Pacman speed up during this section). They also cannot reverse their own direction by choice. However they will be forced to reverse direction when changing states, moving between Chase, Scatter and Frightened.

Maze:
The maze is the same as the original, containing 240 pellets, 4 energisers and 2 cherries. There are also two tunnels located on the left and right side of the maze. Going through these will teleport you to the other side. The cherries spawn after eating 70 and then 170 pellets and are on screen for 10 seconds or until eaten. Pellets award 10 points, energisers 50, cherries 100 and Ghosts 200 x the amount of ghosts eaten in one frightened phase. Score is displayed in the top right, lives in the bottom left.

The Ghost Behaviours:

Blinky:
Blinky starts outside of the ghost house at spawn, and is the most basic of the ghosts. During Chase, Blinky will target the cell Pacman is currently in. During Scatter, he will target the top right corner wall, causing him to loop in a circle. When sent to the ghost house for respawn, Blinky shares the same cell as Pinky.

Pinky:
Pinky starts in the ghost house but leaves almost straight away. During Chase, pinky targets the cell 4 ahead of Pacmans current position, depending on which direction hes looking. I fixed the bug in the original game where if Pacman was facing up, Pinkys target was 4 up and 4 to the left due to overflow errors. This allows Pinky to ambush Pacman by blocking his path. During Scatter, Pinky targets the top left corner wall. 

Inky: 
Inky starts in the ghost house and can only leave after 30 pellets are eaten, and is objectively the most complex ghost to predict. During Chase, Inky draws a line from Blinky's current position to 2 spaces infront of Pacman. Then Inky doubles this line in length, and whichever cell it points to is the target cell. I fixed the same bug with Inky that pinky had. This means he can use Blinky to corner or ambush Pacman. During Scatter, he targets the bottom right corner wall.

Clyde:
Clyde starts in the ghost house and only leaves after 80 pellets have been eaten. During Chase, Clyde draws a line from him to Pacman. If it is greater than or equal to 8 cells, he targets Pacmans current cell like Blinky, but if its shorter than 8 cells, he targets his scatter cell, therefore "running away" from Pacman. However, if Pacman is in the bottom left corner, Clyde will head towards him as during scatter, he targets the bottom left corner wall.

Basic AI:
The ghosts all have a basic AI, able to find the shortest path to Pacman. when reaching a turn, it calculates the shortest, direct line from that possible cell to their target. It does this for all legal turns, finds the shortest line and moves in that direction. However, it is only basic because the direct line is straight, threfore the route they take may be longer than a route in the wrong direction, allowing Pacman to trick the ghosts into taking a longer path, allowing him to escape.

Frightened:
When Pacman eats an energiser, the ghosts enter the Frightened state. They all turn blue, move slower and the timer for changing state stops. This means Pacman is able to eat them for bonus points. They have no target cell in this state, so take their turns pseudo randomly, going in order of priority: up, left, down, right. If eaten, they enter the Dead state, moving twice as fast back to home to respawn. Once repawned, even if the other ghosts are still frightened, the previously dead ghost will enter the state from before they became frightened. They then flash 5 times before re-entering their previous state. 

My Code:

There are 2 base classes in my game: PacMan and Entity. PacMan controls all of the file handling, board manipulation, movement verification and calculations, win and lose conditions and AI. It is the main class that interacts with assignment.cpp mostly. Entity is a base class for storing Pacman and the ghosts (the entities) information like current position, next movement, targetting cells, home cells etc. It has two derived classes, Pacman and Ghost, which itself has 4 derived classes, 1 for each ghost (called Blinky, Pinky, Inky, and Clyde). It is structured as an OOP model, using getters, setters and constructors for manipulation, keeping (most) variables private (the Pacman and Ghost variables in the PacMan class are public so info on their states can be passed to the draw function, but these are defensive variables due to the use of getters and setters).

The game loads the grid in from a text file, gridFill.txt, filling the characters into the 2d array grids for drawing and manipulation, and the numbers into another 2d array for turn information. This could technically be editted to produce different mazes, however some elements of the maze have been hard coded in, only because having them in the file would make it very difficult to provide their functioality (teleporting tunnels, scatter locations etc). If these were in the same place, the maze could be manipulated for other levels though.

My code is commented to explain what functions do (unless self explanatory).


Link to onedrive file for walkthrough video:

https://uweacuk-my.sharepoint.com/:v:/g/personal/alexander2_oldale_live_uwe_ac_uk/EQunPZ2EG8hBojMl42bHErIB7t9KnwtiI_kjOS_cebkJ5w?e=kUdlT5

On the right hand side will be the game screen and on the left will be the console, outputting my key presses as they are pressed.