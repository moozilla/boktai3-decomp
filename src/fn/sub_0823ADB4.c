#include "global.h"
struct LinkSession {
    u8 pad0[0x610];
    u16 f610;
    u16 f612;
    u8 pad614[0x628 - 0x614];
    u32 f628;
    u32 f62C;
};
extern struct LinkSession *gGameState;
u32 LinkPlayer_GetSessionPlayerData(u8 *);
void LinkPlayer_InitOptionalPlayerState(u8 *);
void LinkPlayer_LoadBaseStats(u8 *);
void LinkPlayer_InitHealth(u8 *);
void LinkPlayer_InitEnergy(u8 *);
void Player_SetTetherMode(u8 *, s32, s32);
void Player_InitializeInputHistory(u8 *);
static inline void Reset(struct LinkSession *s)
{
    s->f62C = 0;
    s->f610 = 0;
}
// confidence=low: resets a link player: session data, stats, tether and input state.
s32 sub_0823ADB4(u8 *p)
{
    u32 v = LinkPlayer_GetSessionPlayerData(p);
    *(u32 *)(p + 0x344) = v;
    *(u32 *)(p + 0x1c) = 1;
    *(u32 *)((u8 *)gGameState + 0x628) = 0;
    gGameState->f62C = 0;
    *(u16 *)((u8 *)gGameState + 0x610) = 0;
    gGameState->f612 = 0;
    LinkPlayer_InitOptionalPlayerState(p);
    LinkPlayer_LoadBaseStats(p);
    LinkPlayer_InitHealth(p);
    LinkPlayer_InitEnergy(p);
    Player_SetTetherMode(p, 0, 0);
    Player_InitializeInputHistory(p);
    *(u16 *)(p + 0x534) = 0;
    *(u16 *)(p + 0x536) = 0;
    *(u16 *)(p + 0x538) = 0;
    {
        u32 t = *(u32 *)(p + 0x18);
        *(u8 *)(p + 0x549) = t;
    }
    *(u16 *)(p + 0x54e) = 0;
    *(u32 *)(p + 0x550) = 0;
    return 0;
}
