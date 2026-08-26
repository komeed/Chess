# Chess 

1400-1600 elo chess-bot where you play against a computer. Runs a PLY depth (every turn is one ply) of 8 at ~1 second delay.

--- 

## Introduction:

Hello! This was a 4 month long project I worked on in the second semester of my senior year of high school. I wanted a project to build and strengthen my C coding skills, so I built this! 
Also, I kind of screwed up with merging Chess and ChessEngine as the commits aren't visible here. All the commits for the actual bot are on ChessEngine.

---

## Problems I faced (and that I solved):
- my algorithm for checking for checkmate was very slow initially; I initially had my algorithm for checking whether the king was checked to go through each piece and find their valid moves, which resulted in that being run for every piece (extremely slow). I fixed this by using attack masks for every piece. These masks are generated as fixed, static initialized arrays that contain all possible moves of a piece in one position; that way, I can iterate through each possible move on each piece until it hits theirs or the opponent's piece, and find all attacking pieces of one player. This also worked extremely well for check detection.
- 

## Progression:

- I started with the UI first. I was already familiar with OpenGL through a small physics simulator I built in c++, so building the GUI for the chess board was fairly straight forward.
- I then transitioned to adding all chess functionalities like piece movement and game rules. However, upon starting with arrays of enums I quickly realized this was way too inefficient. So, I learned about bitboards, and implemented all game rules and piece movement through that.
- 

---

## What I learned:

1. I became super comfortable with C concepts.
- Because I represented the board as a bitboard, I used bitwise operations for every single chess move and operation
- Strengthened understanding and safety of raw pointers without unique_ptr like in c++ (or references in java)

2. Because my project at PLY of 8 requires running millions of chess moves every bot call, I used C to become very efficient.
- I learned the importance of operations like malloc() to offload memory creation in the heap, not the stack
- used macros or inline/static calls rather than functions to help offload work from the call stack (especially with functions that were called millions of times)
- 

3. 
