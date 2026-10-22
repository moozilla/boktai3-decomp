#include "global.h"
s32 Script_SeekToKeyword(s32);
u8 *sub_08227E9C(void);
void Script_SetPc(u8 *);
s32 Script_GetValue(void);
u32 BestiaryDisplay_Create(u32, u32);
void BestiaryDisplay_Hide(void);
void BestiaryDisplay_SetPresentationMode(u32, u16);

struct SequenceEntry {
    s16 id, x, y;
    s16 a, b, c;
};
struct TextSequence {
    u8 pad0[0x18];
    struct SequenceEntry entries[64];
    u32 display;   /* 0x318 */
    s32 lastIndex; /* 0x31C */
    u16 unk320, unk322;
    u8 unk324;
};

s32 sub_081D7300(struct TextSequence *p, u32 arg)
{
    struct SequenceEntry *e;
    s32 i;
    e = p->entries;
    if (Script_SeekToKeyword(0x74)) {
        Script_SetPc(sub_08227E9C());
        for (i = 0; i < 64; i++) {
            e->id = Script_GetValue();
            Script_GetValue();
            Script_GetValue();
            Script_GetValue();
            Script_GetValue();
            e->x = Script_GetValue();
            e->y = Script_GetValue();
            if (e->id == -1) {
                p->lastIndex = i - 1;
                break;
            }
            e++;
        }
    }
    e = p->entries;
    if (Script_SeekToKeyword(0x63)) {
        Script_SetPc(sub_08227E9C());
        for (i = 0; i < 64; i++) {
            e->a = Script_GetValue();
            e->b = Script_GetValue();
            if ((e->c = Script_GetValue()) == -1)
                break;
            e++;
        }
    }
    p->unk322 = 0;
    p->unk320 = 0;
    p->unk324 = 0;
    p->display = BestiaryDisplay_Create(arg, 0);
    BestiaryDisplay_Hide();
    BestiaryDisplay_SetPresentationMode(p->display, 1);
    return 0;
}
