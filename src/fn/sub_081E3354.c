#include "global.h"
// Provisional name: Presentation_ConfigureFrameRange
struct Sequence {
    u8 pad[0x218];
    u32 resource;
    u16 option;
    u8 gap[6];
    u16 first, last, timer, duration;
};
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
u32 Script_GetTextSourcePc(void);
void MessageWindow_SetPalette(s32);
void MessageWindow_SetMessageTable(u32);
void MessageWindow_SetBounds(s32, s32, s32, s32);
void MessageWindow_SetDisplayMode(s32);
void MessageWindow_ShowMessage(s32);
// Configure the scripted frame range and its duration.
void Presentation_ConfigureFrameRange(struct Sequence *p)
{
    if (Script_SeekToKeyword('r')) {
        p->resource = Script_GetTextSourcePc();
        if (Script_SeekToKeyword('m')) {
            p->first = Script_GetValue();
            p->last = p->first + Script_GetValue() - 1;
            MessageWindow_SetPalette(0x81b);
            MessageWindow_SetMessageTable(p->resource);
            if (!p->option)
                MessageWindow_SetBounds(1, 0, 30, 10);
            MessageWindow_SetDisplayMode(1);
            MessageWindow_ShowMessage(p->first);
            if (Script_SeekToKeyword('c'))
                p->duration = Script_GetValue();
            else
                p->duration = 60;
            p->timer = 0;
        }
    }
}
