# Qualums
### About Qualums
Qualums exist on a grid. Every qualum has a certain position, x speed, y speed, and color. At each iteration, every qualum moves to the right by its x speed, and downwards by its y speed. Qualums interact when they collide, i.e. when they both occupy the same grid square at once. It is possible for more than two qualums to collide at once, however the use of these collisions is discouraged, as the outcome will depend on the order in which the qualums were created. Here are the rules for qualum collision:
1. If both qualums are red, they are both destroyed.
2. If one qualum is red, it destroys the other one.
3. If one qualum is cyan, the qualum is delayed (it cannot move or collide) for 1 iteration. This means that (assuming the cyan qualum is stationary) it will take a moving qualum 2 iterations to pass through a cyan qualum.
4. If one qualum is orange, the qualum is delayed for 16 iterations. This qualum is like a more extreme version of the cyan qualum, and it can be used to avoid very long chains of cyan qualums.
5. If one qualum is gray, the other qualum turns left.
6. If one qualum is either green or blue, and the other is also either green or blue, the two qualums will merge, forming one qualum whose speed is the sum of the two qualums' speed. The resulting qualum will be green unless both qualums were green. This is a chart of the color of the resulting qualum (this is NAND where green = true and blue = false):   
```
Qualum 1 | Qualum 2 | Output qualum
Blue       Blue       Green
Blue       Green      Green
Green      Blue       Green
Green      Green      Blue
```
7. If both qualums are purple, they will both be destroyed.
8. If one qualum is purple, it will destroy the other qualum, and create two copies of it. The copies will be sent out in two different directions, both of which are perpendicular to the original qualum's direction.
9. If one qualum is yellow, it will stop the other qualum. For example, if a qualum moving right collides with a yellow qualum, it will remain stationary on the square to the left of the yellow qualum.

So, you can think of giving these names to the colors of qualum:  
Red - A destroyer  
Cyan/Orange - A delayer  
Gray - A turner  
Blue/green - Bits of data (0 and 1 respectively)  
Purple - A splitter  
Yellow - A stopper
### Building
If you wish to build Qualums, you will need to install:
- g++
- make
- SDL2  

You can build it using `make clean && make all`.

You might want to modify the size of the output qualum grid and the size of the window. You can do this by changing `GRID_WIDTH` and `GRID_HEIGHT` in `src/Rendering.h` and you can change `RENDER_SCALE` in `src/Rendering.cpp` to change the size of qualums in pixels. Hopefully, the ability to change these variables without rebuilding (in .qua files or in command line options) will be available soon.
### Running
To run a .qua file, simply use the command `./qualums [your file].qua`.
The qualum repository includes several example .qua files (including an 8-bit adder!) which can be run with, for example,
```
./qualums add8test.qua
```
You can change the numbers added in `add8test.qua` by editing the file.
### .qua files
.qua files begin with a one-line header. **The first line of a .qua file is always ignored.** It can be helpful to add
```
x   y   speedX  speedY  color
```
To the top of the file so that the text is aligned.   
To add a qualum, just add a new line to the file, and separate the x position, y position, x speed, y speed, and color with any number of spaces/tabs.  
Each color is represented using a 2-letter code:
```
GA - Gray
RE - Red
BL - Blue
GE - Green
PU - Purple
YE - Yellow
CY - Cyan
OR - Orange
```
So, to create a green qualum at (10,50) that will move towards, then collide with (and be split by) a purple qualum at (50,50):
```
x   y   speedX  speedY  color
10  50  1       0       GE
50  50  0       0       PU # Comments begin with #; they can be put at the end of a line
# Or on a new line
```
#### Including other .qua files
To include a .qua file in another .qua file, put `![name of file]` in the including file. The x positions & speeds, and the y positions & speeds of the qualums in the file will be shifted by the numbers in the x, y, speedX, and speedY columns. For example,

`wall.qua`:
```
x   y   speedX  speedY  color
0   0   0       0       YE
1   0   0       0       YE
2   0   0       0       YE
# A horizontal wall to stop qualums
```
`anotherfile.qua`:
```
x   y   speedX  speedY  color
30  30   1       0       !wall.qua # The yellow qualums will be at (30, 30), (31, 30), and (32, 30)
# Since speedX = 1, the wall will be moving to the right.
41  20   0       1       GE # This qualum will move downwards and collide with the wall
```
