#include "global.h"
#include "scene_input.h"

s32 Mod(s32, s32);
s32 Div(s32, s32);

static inline void SceneHud_SetPosition(struct SceneInputPosition *pos, s32 x, s32 y)
{
    pos->x = x;
    pos->y = y;
    pos->z = 0;
    pos->pad = 0;
}

void SceneHud_SetActionCursorPosition(struct SceneInputPosition *pos, s32 index)
{
    if (index <= 19) {
        if (index <= 7) {
            s32 column = index & 1;
            s32 row = index >> 1;
            SceneHud_SetPosition(pos, column * 24 + 0x50, row * 24 + 0x20);
        } else {
            s32 i = index - 8;
            s32 column = Mod(i, 3);
            s32 row = Div(i, 3);
            SceneHud_SetPosition(pos, column * 24 + 0x88, row * 24 + 0x20);
        }
    } else if (index == 39) {
        SceneHud_SetPosition(pos, 0xd8, 0x50);
    } else if (index == 38) {
        SceneHud_SetPosition(pos, 0xd8, 0x38);
    } else if (index == 36 || index == 40) {
        SceneHud_SetPosition(pos, 0xd8, 0x68);
    } else if (index == 32) {
        SceneHud_SetPosition(pos, 0xd8, 0x20);
    } else {
        pos->x = 0x90;
        pos->y = 0x20;
    }
}
