# fishinggame
This is a group project from my CPE353 course. It is done in C++ and intended for use with QtCreator. 

Credit is absolutely due to my teammates: Ben McAnulty, Jesse Cowart, and Jacob Smith.

# User Manual
- NOTE: This project is intended for Linux with Qt Creator 4.9.2 (Based on Qt 5.12.4). Any other versions, or operating systems may encounter stability issues. 

# CPE 353 Team 13 
# Fishing Simulator 2021 User Manual

- How to Compile Fishing Simulator 2021 on Linux/Lab Machines:

    1. Enter the fishinggame directory, and type “qmake”.

    2. Then type “make”.

    3. Finally, use “./fishinggame” to run the program (Server host may need to run 2 terminals to maintain both executables)

    4. Repeat steps 1-3 for the TcpServer directory as well.
  
- Order of Execution and Basic Game Setup:

    1. First, have one player open a copy of the TcpServer executable.

    2. Then, have all 4 players open a copy of the fishinggame executable. 

    3. Next, have the TcpServer host relay the IP and port information to the clients if needed.

    4. Once the clients have properly entered the host’s information, the game will begin. Have fun! 
  
- Summary of game objectives:

    + In Fishing Simulator 2021, the objective is to simply catch more fish than the other three opponents.

    + Catching a fish is done by colliding with the fish object. Once collided, a point will be added to the user’s current point total.

    + After the 2 minute timer is finished, the scores of all the players will be displayed on the scoreboard.
  
- Any setup or other quirks we must be aware of to play the game:
    There must be at least four players for the game to launch.
    
    + If the player disconnects during the match, that match will never complete.
    
    + The only method of replaying after a game ends is by restarting the application.
    
    + Please restart all software before attempting another game to preserve TcpServer database quality. 
    
    + Each player will be playing in their own instance. All fish objects are originally spawned in the same location for all players, but during the match they do not update for all players.

