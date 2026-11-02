#include "result_tutorial.h"
int ResultTutorial_AnimateLevel(struct ResultTutorialLevel *tutorial)
{
    switch (tutorial->phase) {
        case 0:
            tutorial->phase = 1;
            tutorial->timer = 0;
            break;
        case 1:
            tutorial->level = (tutorial->timer >> 4) + 3;
            if (++tutorial->timer > 94) {
                ++tutorial->phase;
                tutorial->timer = 0;
            }
            break;
        case 2:
            tutorial->level = 8 - (tutorial->timer >> 4);
            if (++tutorial->timer > 78)
                return 1;
            break;
    }
    return 0;
}
