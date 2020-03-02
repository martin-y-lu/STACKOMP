```bash
v>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>v
> rstack.....v >.p..v        >  p   >OO   d    v  >Smpv v
^ S     p<<<<<   A     CCC   , K   O^  O  bM  p<  p  lR v
^  SSS     T    A A   C      SK    O   O  y MM v  .Qpu< v
^     S    T   rcomp..C.>    ^ K   O   O  v    v  S     v
^ SSSS  >  T  A^    <  CCC   K   K  OOO   >    >  ^     v
^<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
```
# STACKOMP: A "Stack based" 2d, reflective, esoteric programming language

Stackcomp, short for stack compression, or maybe stack computation, is a fungoid esoteric language, meaning symbols exist on a 2d playfield, in which the insruction pointer (IP) traverses. The starts at the top left of the playfield, and moves right. Just like Befunge, Stackomp uses the <>^v characters to change the direction of the IP's movement. The IP also loops around, if it moves off of the playfield. But unlike Befunge, Stackomp does not have a stack based memory system at its disposal- instead the IP only has  single cell value as memory, which it can write to, and read from.
##### The Stackomp playfield
The Stackomp playfield contains a 2d array of cells (at most 1000 wide and 100 tall). Each cell either stores a character, a number, or white space. The playfield is loaded from a .sk text file. Space and newline characters are considered whitespace. The characters 0 through 9, X, C and M are considered numbers. Everything else is considered a character. Of these characters, several of them are instruction characters (like <>^v), but they behave and can be manipulate in the same way as other characters.
##### Data manipulation
The S, u, r, d, l characters are used for reading data off of the playfield.
_S:_ When the IP runs into the S character, it'll store the next (non whitespace) cell into it's memory. This includes instruction characters also, effectivley ignoring them.
_u,r,d,l:_ Passing by these lower case directional instuction "pulls" from the stack pointing in that direction, storing the nearest cell into memory, and shifting all cells following it toards the character (stopping at whitespace). Pulling cells will also loop around the playfield, so insure there is always whitespace between things you want to keep seperate.
For example (& represents the IP): If the IP is moving right in this example,
```bash
&> d
   H
   I
   >
```
the result would be
```bash
 > d  &
   I
   >

```
with the IP storing an H character. The main gimmic of Stackomp is, the items in the playfield act as stacks that the IP can manipulate.

The U,R,D,L characters (Upper case directional instructions) push from the stack pointing in each direction. If the memory and stack contain only characters, it acts exactly like you would expect (the opposite of pushing), other than the fact that the IP retains its memory.
So if the IP was storing K in this example:
```bash
&> D
   H
   I
   >

```
the result would be
```bash
 > D &
   K
   H
   I
   >
```
But the interaction is different for number cells. Numbers and characters will never interact, but if a smaller number is pushed into the cell of a larger number, the larger number will subtract the smaller (and the push will stop)
So if the IP was storing numerical 2 in this case:
```bash
&> D
   5
   9
   >
```
the result would be
```bash
 > D  &  
   3
   9
   >
```
, but if the IP was storing numerical 8, the result would be:
```bash
 > D  &  
   8
   4
   >
```
(the 5 gets pushed into the 9)
This operation is the only means of doing calculations, or logical branching.
An example of logical branching would be if pushing a larger number into a smaller one moves a 'v' out of the way of the IP's path, causing a branch, compared to if the original number was smaller. Doing conditional branching requires the code to change itself, and most of the time, destroy itself.
##### Input and output
The K character is used for user input, and the p and P characters are used for displaying to the screen.
### Full instruction Breakdown:
Character| Instruction
---|---
_< > ^ v_: | Change the direction of the intruction pointer movement
_S_:|Read the following cell into memory
_u r d l_:| Pull from the stack in the specified direction
_U R D L_:| Push into the stack in specified direction (with conditional subtractions)
_K_:| Read the askii value of the user inputed character as a numerical value
_p_:| Print the current value in memory as a character
_P_:| Print the current value in memory as a decimal number


## Installation

Inside of the folder, there are 3 versions of the STACKOMP interpreter. All three work. The first two use pure c, and can be compiled into an EXE file to run by using make.

```bash
make STACKOMP_interpreter
```
```bash
make STACKOMP_interpreter_ver2
```
The third version uses c++ so it can use the ncursors library, compile by running
```bash
g++ -o int_v3 STACKOMP_interpreter_ver3.cpp -lncurses
```
If your computer complains about a lack of the ncurses library you can homebrew it.
At this point you should have the executable.

## Usage
Versons 1 and 2 take no parameters, and can be run like so
```bash
./STACKOMP_interpreter
```
```bash
./STACKOMP_interpreter_ver2
```
They will then ask for a filename. Input the name of an existing .sk file. Several come predownloaded, but you can of course make your own. Once you inputted that, It should begin to run.
Versions 1 and 2 are functionally identical, and flicker a lot, so I would recommend using version 3.
Version three takes four parameters- the filename, the framedelay in microseconds, 1/0 boolean for drawing the rendering, and a boolean for drawing the debug info. Here is an example:
```bash
./int_v3 logo.sk 40000 1 1
```
It'll display some debug info, enter through it and the code should start running.

##### Have some fun, and see what you can do with it,
##### - Martin lu

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.
## License
[MIT](https://choosealicense.com/licenses/mit/)
