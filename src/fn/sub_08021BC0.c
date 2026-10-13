#include "global.h"
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
