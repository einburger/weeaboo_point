
### About ###

This is a VN engine that reads from a movie-like script and can be edited live.

## Compiling on windows ##

This project requires you to have cmake 3.17.2 and 
a C++17 compatible compiler.

I recommend using git bash, it makes this process easier

    git clone https://github.com/einburger/weeaboo_point
    cd weeaboo_point && mkdir build && cd build && cmake ../ && start devenv WeeabooPoint.sln

Right click WeeabooPoint in in the solution explorere window and click "Set as StartUp Project"
    
## Other Systems ##

Specify the makefile you want cmake to generate via the -G flag. I use MSYS2 so I type:
``` cmake -G "MSYS Makefiles" ../ ```

