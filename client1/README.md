# TCP CHESS-GAME

Steps to compile and run CHESS game on your ubuntu system:

1. Install SFML and JSON-C. Run these commands in your terminal:
   "sudo apt-get install libsfml-dev"
   "sudo apt-get install libjson-c-dev"

2. CD to client folder in your terminal.

3. Then for compiling the game, run this command:
   "g++ -std=c++14 client.cpp -o client -lsfml-graphics -lsfml-window -lsfml-system -ljson-c"

4. Write "./client" in the terminal and the game will run.

NOTE: Make sure the server is running. While playing game, you need to select the piece with RIGHT-MOUSE-KEY, and select the position on which you want to move that piece with LEFT-MOUSE-KEY.
