#include "global.h"
/* SceneActors_CopyAvailablePosition: Copies the available actor's two position coordinates to the output and reports failure if absent */
#define SceneActors_CopyAvailablePosition sub_08021BC0
#define SceneActors_CopyAvailablePosition sub_08021BC0
typedef struct {
    int a, b;
} Position;
u8 *SceneActors_FindAvailable(void);
int SceneActors_CopyAvailablePosition(int *out)
{
    u8 *p = SceneActors_FindAvailable();
    int result;
    if (p) {
        *(Position *)out = *(Position *)(p + 0x54);
        result = 0;
    } else
        result = -1;
    return result;
}
