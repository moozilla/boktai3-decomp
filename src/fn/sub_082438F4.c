#include "global.h"
#include "engine.h"
#include "game_state.h"
#include "player.h"
struct LinkRef {
    u8 unk0[0x18];
    s16 bit;
};
extern struct LinkRef *gLinkSession;
void Player_ResetActionState(struct Player *);
s32 Player_ApplyTerrainDamage(struct Player *);
void LinkPlayer_ResetToIdleState(struct Player *);
void Player_TickDamageBlink(struct Player *);
void Player_InitActionCallback(struct Player *);
void Player_PlayLocalSong(s32, u32);
s32 Callback_InvokeActor(void *, void *);
void sub_0823A2E4(struct Player *);
void sub_0821DE30(void *);
static inline s32 LocalPlayer(void)
{
    if (!gLinkSession)
        return -1;
    return gLinkSession->bit;
}
#define F(T, off) (*(T *)((u8 *)p + (off)))
s32 Player_UpdateLinkCore(struct Player *p)
{
    u16 *clearPtr;
    Player_ResetActionState(p);
    if (p->kind == 1) {
        if (Player_ApplyTerrainDamage(p))
            LinkPlayer_ResetToIdleState(p);
        else
            Player_TickDamageBlink(p);
    }
    Player_InitActionCallback(p);
    if (p->actionRequestState) {
        Player_PlayLocalSong(p->slot, 0x393);
        p->actionRequestState = 0;
    }
    Callback_InvokeActor(p, p->updateCallback);
    sub_0823A2E4(p);
    gPlayerSlotStatus[p->slot] = p->kind;
    if (p->slot == LocalPlayer()) {
        *(u16 *)((u8 *)gGameState + 0x30) = p->position.x;
        *(u16 *)((u8 *)gGameState + 0x32) = p->position.y;
        *(u16 *)((u8 *)gGameState + 0x34) = p->position.z;
        *(u16 *)((u8 *)gGameState + 0x85E) = ((p->facing + 5) & 7) << 5;
    }
    clearPtr = (u16 *)((u8 *)p + 0x54E);
    {
        s32 zero = 0;
        *clearPtr = zero;
        F(u32, 0x550) = zero;
    }
    if (p->kind != 8) {
        F(u32, 0x298) = (u32)p + 0x30;
        sub_0821DE30(&F(u8, 0x294));
    }
    return 0;
}
