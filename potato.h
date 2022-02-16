#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct potato_t {
    int player_list[512];
    int index = 0;
    int remain_hops;
} potato;