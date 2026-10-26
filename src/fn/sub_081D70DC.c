#include "global.h"
#include "render.h"
void TextSequence_SetState(u8 *, u32);
void Task_Kill(void *);
s32 Div(s32, s32);
void BestiaryDisplay_Hide(void *);
void BestiaryDisplay_Show(void *);
void BestiaryDisplay_SetEntry(void *, s32);
void BestiaryDisplay_SetPosition(void *, u32, u32);

struct TextSequenceEntry {
    u16 unknown0;
    u16 duration;
    u16 unknown4;
    u16 x, y;
    s16 id;
};

void sub_081D70DC(u8 *p, struct TextSequenceEntry *e)
{
    if (*(s16 *)(p + 0x322) == 0) {
        if (e->id == 0x68) {
            TextSequence_SetState(p, 3);
            BestiaryDisplay_Hide(*(void **)(p + 0x318));
            return;
        }
        if (e->id == -1) {
            Task_Kill(p);
            BestiaryDisplay_Hide(*(void **)(p + 0x318));
            return;
        }
        BestiaryDisplay_SetEntry(*(void **)(p + 0x318), e->id);
        BestiaryDisplay_Show(*(void **)(p + 0x318));
        BestiaryDisplay_SetPosition(*(void **)(p + 0x318), e->x, e->y);
        gObjFadeLevel = 0;
        gObjFadeColor = 0x1084;
    }
    if (++*(s16 *)(p + 0x322) >= e->duration) {
        gObjFadeLevel = 0x40;
        TextSequence_SetState(p, 1);
    } else {
        s32 level = Div(*(s16 *)(p + 0x322) << 6, e->duration);
        gObjFadeLevel = level;
    }
}
