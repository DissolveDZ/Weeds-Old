#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <raylib.h>
#include <raymath.h>
#include "def.h"
#include "init.c"
#include "update.c"
#include "inventory.c"
#include "draw.c"

int main()
{
    init();
    while (!WindowShouldClose())
    {
        update();
        draw();
    }
    CloseWindow();
    return 0;
}