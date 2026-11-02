#include "global.h"
void ColorFade_QueueCommand(int, int, void *);
static inline u32 SceneCommands_PaletteBankMask(const s16 *args)
{
    u32 mask = 0;
    if (args[5])
        mask = 0xFFF;
    if (args[6])
        mask = (u16)mask | 0x1000;
    if (args[7])
        mask = (u16)mask | 0xE000;
    return mask;
}

void SceneCommands_FadePalette(void *p, void *unused, u8 *message)
{
    s16 *args = (s16 *)(message + 8);
    int out[7], mode;
    u32 mask = 0;
    mask = SceneCommands_PaletteBankMask(args);
    out[0] = 1;
    out[1] = args[1];
    out[2] = args[2];
    out[3] = args[3];
    out[4] = args[4];
    out[5] = (u16)mask;
    out[6] = 0;
    if (!out[2] && !out[3] && !out[4]) {
        out[2] = 4;
        out[3] = 4;
        out[4] = 4;
    }
    {
        int enabled = args[0];
        mode = 2;
        if (!enabled)
            mode = 1;
    }
    ColorFade_QueueCommand(mode, 7, out);
}
