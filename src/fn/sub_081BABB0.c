#define Registry3F74_InitFromScript sub_081BABB0
#include "global.h"

struct Registry3F74 {
    u8 unknown000[0x224];
    s32 selection;
    s32 previousSelection;
    u32 cursor;
    s32 entryScripts[11];
    s32 optionT;
};

void Video_SetRenderCallbacks(int, void *, void *, void *);
void sub_08217F4C(void);
void sub_08214B44(void);
void sub_08219624(void);
void sub_081BA924(struct Registry3F74 *);
void sub_081BA81C(struct Registry3F74 *);
s32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
void Registry3F74_CreateSelectionCursor(struct Registry3F74 *, s32);
void Registry3F74_StartOpeningFade(void *);

s32 sub_081BABB0(struct Registry3F74 *p)
{
    s32 i;
    s32 *entry;
    Video_SetRenderCallbacks(0, sub_08217F4C, sub_08214B44, sub_08219624);
    sub_081BA924(p);
    sub_081BA81C(p);
    if (Script_SeekToKeyword('p')) {
        entry = p->entryScripts;
        i = 10;
        do {
            *entry++ = Script_GetValue();
        } while (--i >= 0);
    } else {
        s32 zero = 0;
        for (i = 10; i >= 0; i--)
            p->entryScripts[i] = zero;
    }
    if (Script_SeekToKeyword('P'))
        p->selection = Script_GetValue();
    else
        p->selection = 0;
    if (Script_SeekToKeyword('t'))
        p->optionT = Script_GetValue();
    else
        p->optionT = 0;
    p->previousSelection = p->selection;
    Registry3F74_CreateSelectionCursor(p, p->selection);
    Registry3F74_StartOpeningFade(p);
    return 0;
}
