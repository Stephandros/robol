# Robol

## Install flex and bison (Windows)

1. flex-2.5.4a-1.exe -> http://gnuwin32.sourceforge.net/packages/flex.htm
2. bison-2.4.1-setup.exe -> http://gnuwin32.sourceforge.net/packages/bison.htm
* After that, do a full install in a directory of your preference without spaces in the name. I suggest C:\GnuWin32. Do not install it in the default (C:\Program Files (x86)\GnuWin32) because bison has problems with spaces in directory names, not to say parenthesis.
* Also, consider installing Dev-CPP in the default directory (C:\Dev-Cpp) -> http://www.bloodshed.net/dev/devcpp.html
* After that, set the PATH variable to include the bin directories of gcc (in C:\Dev-Cpp\bin) and flex\bison (in C:\GnuWin32\bin). To do that, copy this:  ;C:\Dev-Cpp\bin;C:\GnuWin32\bin and append it to the end of the PATH variable.



## Build

1. bison -y -d robol.y
* flex robol.l
* gcc -c y.tab.c lex.yy.c
* gcc y.tab.o lex.yy.o robol.c -o robol.exe

## LINUX

gcc y.tab.o lex.yy.o robol.c -o robol.out
to run it ./robol.out
you can create an alias:
alias compile='bison -y -d robol.y; flex robol.l; gcc -c y.tab.c lex.yy.c; gcc y.tab.o lex.yy.o robol.c -o robol.out'
