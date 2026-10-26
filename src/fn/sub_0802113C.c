#include "global.h"
#include "render.h"
void ColorFade_QueueCommand(int, int, void *);
void SceneCommands_BlendPalette(void *p, void *unused, u8 *message)
{
    s16 *args = (s16 *)(message + 8);
    int out[7], mode;
    out[0] = 2;
    out[1] = args[1];
    out[2] = args[2];
    out[3] = args[3];
    out[4] = args[4];
    out[5] = 0;
    out[6] = *(s16 *)(message + 0x12);
    if (*(s16 *)(message + 0x14) == 1)
        gObjPaletteEffect = 1;
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
