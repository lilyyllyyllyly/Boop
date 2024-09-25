# Boop
This is a recreation of "boop.", a deceptively challenging abstract strategy game, in C. [(Official Product Page)](https://www.smirkanddagger.com/product-page/boop)

To quote the manual:
> Every time you place a kitten on the bed, it goes “**boop.**” Which is to say that it pushes every other kitten next to it on the board one space away. Line up three kittens in a row to graduate them into cats… and then, get three cats in a row to win.
> But that isn’t easy with both you AND your opponent constantly “booping” kittens around. It’s like… ***herding cats!***
> Can you “boop” your cats into position to win? Or will you just get “booped” right off the bed?

## Gameplay
Check out the official manual [here](https://drive.google.com/file/d/15W8ugnaJ7ojxS_lW2I9JHgw7FG0eRpKh/view?usp=sharing) for the full rules and illustrations.

### Overview
Each player begins with **8 kittens** on their hand, and **no cats**. The player with the orange pieces plays first. <br>
The orange player's hand is displayed on the top left, and the black player's on the top right, outside the board. <br>
The players take turns placing a kitten or cat from their hand on an open square on the board.
The kittens will graduate into cats when forming a line of 3 in any direction.
**The player who lines up 3 cats first wins.**

### Booping
When a kitten is placed onto the board, it will "boop" adjacent kittens in all 8 directions, pushing them one space away.
Kittens can be pushed off the board (and will be placed back into the corresponding player's hand), but cannot be pushed into each other.
The pushed pieces do not cause a chain reaction, they will not boop other pieces after being moved.

### Graduating
When 3 kittens of the same color are lined up in any direction (checked *after* booping), they will graduate into cats.
That is, they will be removed from the board and 3 cats will be added to the corresponding player's hand. <br>
If a line is formed with a *combination* of cats and kittens, it will still be removed from the board, and the cats will be placed back onto your hand while the kittens will graduate normally. <br>
Alternatively, if all 8 of your pieces are already on the board, you can choose 1 kitten to graduate, or remove 1 cat from the board and place it back onto your hand.

If there is a situation where multiple lines of 3 are formed, you will be prompted to choose which line you will graduate by clicking the 3 pieces that compose it.
Right clicking will reset the selection, and so will cliking a piece that does not form a line with the previous selections.

### Cats & Winning
Cats behave like regular kittens except they cannot be booped by kittens, only other cats. <br>
If you line up 3 cats in a row in any direction, you win.
You can also win by having 8 cats placed on the board.
(both win conditions are checked *after* booping!)

### Possible diversions from the manual
Only your pieces are checked for graduation on your turn after booping occurs.
This means that, if you push the opponent's pieces to form a line on your turn, those kittens will only graduate after the opponent moves.
This is not explicitly stated in the official manual, and it is a best guess trying to follow the wording of the manual as accurately as possible.

When all 8 of your pieces are on the board *and* a line of kittens is formed, you should be able to choose wether to graduate the line or a single kitten.
I have not yet found a way to implement this that is intuitive for the user, so currently the line will always be promoted.

## Running
Currently, the only method of installation is building from source. <br>
This application has only been tested on linux, and this section assume you are using it.

### Dependencies
For building, you will need gnu `make` and `clang`, or any other C compiler if you set the `CC` environment variable appropriately when running `make`. <br>
The required libraries are the same as [raylib](https://github.com/raysan5/raylib)'s. You should check it's [wiki section for dependencies](https://github.com/raysan5/raylib/wiki/Working-on-GNU-Linux#install-required-libraries).
Also the [wayland section](https://github.com/raysan5/raylib/wiki/Working-on-GNU-Linux#wayland) if you are using that.

### Building from source
You can choose to either link with raylib statically or dynamically.
By default, the makefile will build raylib and link with it statically, but if you already have it installed you can link dynamically by setting certain environment variables.

#### Statically linking with raylib
The first step is to clone the repository:
```bash
$ git clone --recurse-submodules https://github.com/lilyyllyyllyly/Boop
```
> [!IMPORTANT]
> You must include the `--recurse-submodules` options here.

Then you can simply run `make` in the cloned directory:
```bash
$ cd Boop
$ make
```
> [!NOTE]
> Any environment variables that affect raylib's compilation work here, such as setting PLATFORM to change the target platform, although building with such options has not been tested.
> Refer to [raylib's wiki page on building](https://github.com/raysan5/raylib/wiki/Working-on-GNU-Linux) for details.

Done! Now you can run boop with `./boop`.

#### Dynamically linking with raylib
Clone the repository skipping the raylib submodule:
```bash
$ git -c submodule.raylib.update=none clone --recurse-submodules https://github.com/lilyyllyyllyly/Boop
```
Now, when running `make`, certain environment variables must be set:
`RAYLIB_LIBTYPE` must be set to `DYNAMIC`, `RAYLIB_INCLUDE_DIR` must be set to the directory where the raylib headers are located, and `LIBS` must be set to additional options to link with raylib (-l and -L). <br>
This is made easier with `pkg-config`, which can provide the necessary values:
```bash
$ cd Boop
$ RAYLIB_LIBTYPE=DYNAMIC RAYLIB_INCLUDE_DIR=$(pkg-config --cflags raylib) LIBS=$(pkg-config --libs raylib) make
```
Done! Now you can run boop with `./boop`.

