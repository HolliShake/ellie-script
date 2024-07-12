#!/bin/bash

gcc main.c .\utfc\*.c .\utfc\utf8proc\*.c .\core\*.c .\src\*.c .\libgc\gc.c .\quickjs\libqjs.a -lm -o main.exe