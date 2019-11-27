
### About ###


* This is the DevilDoctor Visual Novel Engine being developed by einburger, zbotman, and eyercoolguy
* [Link](https://docs.google.com/document/d/1VqWRa8LitZEDNb6UsouAU0HqVtUOEN08fAdZhMpT4J4/edit?ts=5a224b3c) to the design document
* Version: Weaboo Slim


### Coding Style ###


* Gotos are fine if used sparingly


* Functions ought to only do one thing and should fit within a page (about 60 lines)
    - split large functions into smaller ones (helper functions)
        - UNLESS you are programming a performance critical section
          since having large functions with a waterfall of statements really 
          helps debugging.
          
          
* For the love of god, never do shit like this: 
```C++
        if (!!something) {...}
```


* Try to have descriptive and grammatically correct boolean names 
```C++
        if (holding_left_mouse) {...} 
        if (!holding_left_mouse) {...}
```


* Fuck exceptions and programs that use them. I’m the type of person that just crashes a program if you go full retard


* If you think using build tools to build more build tools is a cool thing, you might have a stick up your ass


* Getters and setters are cancer


* Don’t give functions stupid little type hats like this:

```C++
        int
        main() {...}
```

### Syntax Example ###
```C++
int booger_joe = 0;
        
// note: function '{' is on the line after function signature
int add(int x, int y) 
{
    return x + y;
}

===========================================================================

// note: '{' is on the same line as the if
label_one:
    do_something();

if (blah) {
    do_something();
    goto label_one;
} else if (blah) {
    do_something(do_more_things()
               , do_more_things()
               , do_more_things());
}

===========================================================================

// leaving brackets out is ok for lone single line if statements
if (blah)
    do_something();


switch(booger_joe) {
case 0:
    do_something();
break;
}

===========================================================================

struct Position {
    double x; 
    double y;
};
```# weeaboo_point
