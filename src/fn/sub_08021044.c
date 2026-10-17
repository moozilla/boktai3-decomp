#include "global.h"

void sub_082279A8(s32, s32, s32, s32, s32, s32, s32);

/* Screen command 3 (fade): args are on, speed, r, g, b, three layer-group
   switches and a last value; "on" picks mode 3 over mode 2. */
void SceneCommands_Fade(void *p, void *unused, s16 *message)
{
    s16 *args = message + 4;
    u16 mask = 0;
    s32 mode;

    if (args[5])
        mask = 0xFFF;
    if (args[6])
        mask |= 0x1000;
    if (args[7])
        mask |= 0xE000;
    {
        s32 enabled = message[4];
        mode = 3;
        if (!enabled)
            mode = 2;
    }
    sub_082279A8(mode, args[1], args[2], args[3], args[4], mask, args[8]);
}
