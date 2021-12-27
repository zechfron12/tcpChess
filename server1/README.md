# TCP CHESS-GAME

Steps to get the server running on your ubuntu system:

1. Install SFML and JSON-C. Run these commands in your terminal:
   "sudo apt-get install libsfml-dev"
   "sudo apt-get install libjson-c-dev"

2. CD to server folder in your terminal.

3. Then for compiling the game, run this command:
   "g++ -std=c++14 server.cpp -o server -ljson-c"

4. Write "./server" in the terminal and the server is set for a game of chess.

P.S Errors might stil happen (especially segmentation faults).
