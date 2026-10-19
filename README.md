# Cupboards

[На русском](README-ru.md)

### Assignment: Mini-Game “Cupboards”

* The initial arrangement of the tokens is given (the tokens are shown as colored squares in the diagram).
* The goal is to move the tokens into the target arrangement shown in the bottom-left corner of the diagram.
* Tokens may move only along the paths shown in the diagram (there are 9 possible positions for tokens in total).
* A token cannot move past another token occupying a position along its path.

![scheme](.github/scheme.png)

### Implementation requirements

1. Programming language: C++.
   - Standard: C++17.
   - C++/CX and C++/CLI must not be used.
2. Token movement must be smooth.
3. In a single move, a token may travel across multiple connections, provided the path selected by the user is not blocked by other tokens.
4. Positions to which the selected token can move must be highlighted.
5. The selected token must be highlighted.
6. The token positions and level topology (connections) must be specified in an external text file. The positions, number of tokens, and paths can be arbitrary; the diagram shows just one example.
7. The game must be controlled using the mouse.
8. Pathfinding must be implemented programmatically.
9. The program must have a software architecture based on object-oriented programming (OOP).
10. The program must include several input-file examples with different numbers of tokens and connections.

Below is an example input file corresponding to the diagram above (the point coordinates may be changed as desired):

```text
6
9
100,100
200,100
300,100
100,200
200,200
300,200
100,300
200,300
300,300
1,2,3,7,8,9
7,8,9,1,2,3
8
1,4
2,5
3,6
4,5
5,6
4,7
5,8
6,9
```

Explanation:

- Line 1: Number of tokens.
- Line 2: Number of positions where a token can be placed.
- Lines 3–11: Coordinates of the possible token positions.
- Line 12: Initial token arrangement, specified by position.
- Line 13: Winning arrangement, specified by position.
- Line 14: Number of connections.
- Lines 15–22: List of connections between pairs of positions.

### Building 

```shell
cmake -S . -B build
cmake --build build
```