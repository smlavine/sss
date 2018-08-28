#include <stdio.h>

#define CFG_PATH "assets/cfg"

int main(void)
{
    int windowedMode, windowWidth, windowHeight, vsync, aa;
    FILE *f = fopen(CFG_PATH, "r");
    fscanf(f, "%d%d%d", &windowedMode, &windowWidth, &windowHeight);
    fscanf(f, "%d%d", &vsync, &aa);
    fclose(f);
    printf("%d %d %d\n", windowedMode, windowWidth, windowHeight);
    printf("%d %d\n", vsync, aa);
}
