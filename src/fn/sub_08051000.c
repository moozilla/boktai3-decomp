#include "orbit_background.h"
s32 OrbitBackground_Initialize(struct OrbitBackground *, u32, u32, u32);
s32 OrbitBackground_ClearSingleton(void);
struct OrbitBackground *OrbitBackground_Create(u32 id, u32 player, u32 enabled)
{
    struct OrbitBackground *background = gOrbitBackground;
    if (!background) {
        background = Task_Create(9, 0x38);
        if (background) {
            Task_SetCallbacks(background, (void (*)(void *))OrbitBackground_Tick,
                              (void (*)(void *))OrbitBackground_ClearSingleton);
            if (OrbitBackground_Initialize(background, id, player, enabled) < 0) {
                Task_Kill(background);
                return 0;
            }
        }
    }
    return background;
}
