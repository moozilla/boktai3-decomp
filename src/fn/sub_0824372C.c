#include "global.h"
extern u8 *gUnk_02000580[4];
extern u8 *gLinkSession;
s32 Script_GetOptionalIValue(void);
bool32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void LinkSession_CallPositionScript(u8 *, s32);
void Player_InitializeActionState(u8 *, u8 *);

void sub_0824372C(void)
{
    s32 i = Script_GetOptionalIValue();
    u8 *player = gUnk_02000580[i];
    if (player != NULL && Script_SeekToKeyword('w')) {
        s32 v = Script_GetValue();
        s8 *slot = (s8 *)(player + 0x549);
        s32 x;
        u8 *g;
        if (*slot >= 0)
            LinkSession_CallPositionScript(player + 0x30, *slot);
        *slot = v;
        x = *slot;
        g = gLinkSession;
        if (g == NULL || g[0x53] != 0 || x < 0)
            g = NULL;
        else
            g = &g[x * 32 + 0xe4];
        Player_InitializeActionState(player, g);
    }
}
