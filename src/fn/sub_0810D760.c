#include "global.h"
#include "engine.h"
#include "ram.h"

s32 TextWindow_Create(u32, u32, u32, u32);
s32 TextWindow_SetScript(s32 id, const u8 *script);
s32 TextWindow_Refresh(s32 id, s32 index);
void TextWindow_StartPlayback(s32);
u8 StateFlags_TestIndexedFlag(s32);

struct RecordPanel {
    u8 pad0[0xcec];
    const u8 *script;
    u8 padCF0[0xd08 - 0xcf0];
    s32 win[7];
    u8 padD24[2];
    s8 page;
};

void sub_0810D760(struct RecordPanel *p)
{
    s32 i, n;
    struct RecordPanel *q;
    u32 mask;
    p->win[0] = TextWindow_Create(1, 3, 10, 2);
    p->win[1] = TextWindow_Create(13, 5, 16, 2);
    p->win[2] = TextWindow_Create(13, 7, 16, 2);
    p->win[3] = TextWindow_Create(13, 9, 16, 2);
    p->win[4] = TextWindow_Create(13, 11, 16, 2);
    p->win[5] = TextWindow_Create(13, 13, 16, 2);
    p->win[6] = TextWindow_Create(13, 15, 16, 2);
    mask = 0x800;
    q = p;
    if ((gEngineFlags | gScenePauseFlags) & mask) {
        for (i = 0; i <= 5; i++) {
            TextWindow_SetScript(p->win[i + 1], p->script);
            n = p->page * 6 + i;
            if (StateFlags_TestIndexedFlag(n))
                TextWindow_Refresh(q->win[i + 1], n);
            else
                TextWindow_Refresh(q->win[i + 1], 12);
            TextWindow_StartPlayback(p->win[i + 1]);
        }
    } else {
        for (i = 0; i <= 5; i++) {
            TextWindow_SetScript(p->win[i + 1], p->script);
            if (StateFlags_TestIndexedFlag(i))
                TextWindow_Refresh(q->win[i + 1], i);
            else
                TextWindow_Refresh(q->win[i + 1], 12);
            TextWindow_StartPlayback(p->win[i + 1]);
        }
    }
}
