
### About ###

This is a VN engine that reads from a movie-like script.
Trying to keep the script that generates the game as simple as possible.
Requires GLFW.

## Existing commands ##
```
at [image]                       // loads a background
place [character] [x pos]        // places a character at that x position (in % of screen width) 
fadein [character] [speed]       // fades in a character at that speed
wait [character] [time in s]     // wait for s seconds
move [character] [x pos] [speed] // moves character to x position (in % of screen width) at speed
exit [character] (left|right)    // exits the character stage left or right
sync                             // makes previous action run at the same time as the following one
> [text]                         // write dialog

```

## Example script ##
```
at station_a.jpg

place alice 50
place preppy 50
place swimsuit 50

> *audible frustration*

fadein preppy 0.05
wait preppy 0.5

> Hello.

move preppy 20 5
sync
move swimsuit 53 5
sync
fadein swimsuit 0.05
sync
fadein alice 0.02 

wait preppy 0.5 

> HUUAAH! Die mudcrab!

move swimsuit 20 20

> Hey!

move swimsuit 150 15 
sync
move alice -350 20
```

## Result ##
![](https://media.giphy.com/media/hpcP3e9JeHi4JGG0AM/giphy.gif)
