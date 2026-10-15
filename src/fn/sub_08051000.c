#include "orbit_background.h"
#define OrbitBackground_Create sub_08051000
s32 sub_08050F40(struct OrbitBackground *, u32, u32, u32);
s32 sub_08050E90(void);
struct OrbitBackground *OrbitBackground_Create(u32 id, u32 player, u32 enabled)
{
    struct OrbitBackground *background = gUnk_0200048C;
    if (!background) {
        background = Task_Create(9, 0x38);
        if (background) {
            Task_SetCallbacks(background, (void (*)(void *))sub_08050DBC,
                              (void (*)(void *))sub_08050E90);
            if (sub_08050F40(background, id, player, enabled) < 0) {
                Task_Kill(background);
                return 0;
            }
        }
    }
    return background;
}
