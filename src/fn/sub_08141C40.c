#define Player_EmitMovementStepEffects sub_08141C40
#include "global.h"
#include "player.h"

#ifdef NONMATCHING
// fuzzy: 99.3%
// Null-path branch and flag load/mask scheduling remain.
extern u8 *gUnk_02000710;
void Sound_PlaySong(u32);
void Player_PlayPositionEffect(struct Player *, void *, u32, u32, void *, u32, u32);
s32 RepeatedEffect_RequestNearby(void *);
s32 Player_IsNormalActionAllowed(u32);
static inline u32 Mask(u32 *p, u32 m)
{
    return *p & m;
}
static inline u32 HasFlag(struct Player *p, u32 m)
{
    if (!p)
        return 0;
    return p->actionFlags & m;
}
static inline u8 ModeFive(struct Player *p)
{
    u32 a = p->active;
    u8 r = 0;
    if (a && p->sceneMode == 5)
        r = 1;
    return r;
}
static inline u8 Enabled(u32 m)
{
    if (*(u32 *)(gUnk_02000710 + 0x868) & m)
        return 1;
    return 0;
}
void Player_EmitMovementStepEffects(struct Player *p, s32 silent)
{
    struct SceneVec *pos;
    if (p->actionCueFlags & 3)
        return;
    if (p->actionCueCounter)
        return;
    if (p) {
        if (p->active && p->sceneMode == 0)
            return;
        if (Mask(&p->actionFlags, 4))
            return;
    }
    if (HasFlag(p, 0x20000)) {
        if (p->actionCueFlags == 0)
            Player_PlayPositionEffect(p, &p->position, 1, 1, &p->position, 0, 0x5AE);
        else
            Player_PlayPositionEffect(p, &p->position, 1, 1, &p->position, 0, 0x5AD);
        RepeatedEffect_RequestNearby(&p->position);
        return;
    }
    pos = &p->position;
    if (RepeatedEffect_RequestNearby(pos)) {
        if (p->actionCueFlags == 0)
            Player_PlayPositionEffect(p, pos, 1, 1, pos, 0, 0x163);
        else
            Player_PlayPositionEffect(p, pos, 1, 1, pos, 0, 0x162);
        return;
    }
    if (Player_IsNormalActionAllowed(*(u16 *)((u8 *)p + 0x292)) && !(p && ModeFive(p))) {
        if (p->actionCueFlags == 0)
            Player_PlayPositionEffect(p, pos, 1, 1, pos, 0, 0x194);
        else
            Player_PlayPositionEffect(p, pos, 1, 1, pos, 0, 0x193);
        return;
    }
    if (!silent && !HasFlag(p, 0x20000)) {
        if (p && ModeFive(p)) {
            if (p->actionCueFlags == 0)
                Sound_PlaySong(0xC8);
            else
                Sound_PlaySong(0xC7);
        } else if (p->tetherStatusFlags || !p->velocityScaleAxis) {
            if (p->actionCueFlags == 0)
                Sound_PlaySong(0xCA);
            else
                Sound_PlaySong(0xC9);
        } else {
            if (p->actionCueFlags == 0)
                Sound_PlaySong(0x28E);
            else
                Sound_PlaySong(0x28D);
        }
    }
    if (Enabled(1))
        Player_PlayPositionEffect(p, pos, 1, 2, 0, 0, 0);
}

#else
INCLUDE_ASM("asm/nonmatching", sub_08141C40);
#endif
