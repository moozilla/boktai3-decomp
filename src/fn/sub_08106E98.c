#include "global.h"
#include "engine.h"

struct SceneVec {
    s16 x, y, z;
    s16 pad;
};
struct Player;
void *SceneActorQuery_FindPlayerInBounds(const void *center, const void *radius, s32 slot);
u8 *sub_0806F06C(const void *center, const void *radius, u32 *cursor);
u8 TileTrigger_IsTetherNearby(struct TileTrigger *p);
s32 Player_BeginDirectionalCue(struct Player *p, s32 mode);
void TetherActor_BeginDetachedState(s32 mode);
void ScriptTriggerActor_DispatchPendingScripts(struct TileTrigger *p);
s32 EffectGroup_SpawnPart(void *, u32, void *, void *, void *, u32, u32);
void SceneActorReaction_SinkAndRemove(void);
extern const u16 gUnk_0203B400[];

#ifdef NONMATCHING
// fuzzy: 30.1%
// scope shape 204 (differences once low registers are set aside)
// confidence=low: per-frame tile trigger update that sweeps nearby actors and players, spawns a burst of sparks on each new actor, and sets the trigger state from the results
void sub_08106E98(struct TileTrigger *arg0)
{
    u8 *self = (u8 *)arg0;
    struct SceneVec radius;
    struct SceneVec vel;
    struct SceneVec scale;
    struct SceneVec pos;
    u32 cursor;
    u8 inRange[4];
    void *players[4];
    s32 anyTether;
    s32 found;
    s32 i;
    s32 n;
    s32 idx;
    u8 *a;

    cursor = 0;
    anyTether = 0;
    found = 0;
    radius.x = 0x80;
    radius.y = 0x10;
    radius.z = 0x80;
    for (i = 0; i < 4; i++) {
        inRange[i] = 0;
    }

    for (i = 0; i < 4; i++) {
        players[i] = SceneActorQuery_FindPlayerInBounds(self + 0x84, &radius, i);
        if (players[i] != 0) {
            inRange[i] = 1;
            *(u16 *)(self + 0xC0) |= 0x80;
        }
    }

    if (TileTrigger_IsTetherNearby(arg0) != 0) {
        anyTether = 1;
        *(u16 *)(self + 0xC0) |= 0x100;
    }

    while ((a = sub_0806F06C(self + 0x84, &radius, &cursor)) != 0) {
        if (((a[0x2B5] - 0x16) & 0xFF) <= 1) {
            continue;
        }
        a[0x2B2] = 1;
        a[0x2B0] = 0;
        a[0x2B5] = 0x16;
        *(void **)(a + 0x21C) = SceneActorReaction_SinkAndRemove;
        *(u16 *)(a + 0x2C8) = 0;
        *(u32 *)(a + 0x114) &= ~1;
        *(u32 *)(a + 0x104) = 0;
        a[0x2A8] = 3;
        a[0x2A9] = 0;
        a[0x2AA] = 0;
        *(u32 *)(a + 0x2AC) = 0;
        a[0x2B1] = 1;
        *(u16 *)(a + 0x2B8) = 0;
        if (*(u32 *)a == 1 && (*(u16 *)(a + 0x2D4) & 0x2000)) {
            for (n = 0; n < 3; n++) {
                idx = gRandomIndex;
                idx = (idx + 1) & 0x3FF;
                vel.x = (gUnk_0203B400[idx] & 0xF) - 7;
                idx = (idx + 1) & 0x3FF;
                vel.y = (gUnk_0203B400[idx] & 0x1F) - 16;
                idx = (idx + 1) & 0x3FF;
                gRandomIndex = idx;
                vel.z = (gUnk_0203B400[idx] & 0xF) - 7;
                scale.x = 5;
                scale.y = 10;
                scale.z = 5;
                {
                    u16 *src = *(u16 **)(a + 0x484);
                    pos.x = src[0];
                    pos.y = src[1] + 0x80;
                    pos.z = src[2];
                }
                EffectGroup_SpawnPart(a + 0x47C, 3, &pos, &vel, &scale, 40, 40);
            }
            *(u16 *)(a + 0xAA) = 0;
            *(u16 *)(a + 0x2B8) = 0;
        }
        found = 1;
    }

    if (inRange[0] || inRange[1] || inRange[2] || inRange[3]) {
        if ((gEngineFlags & 0x800) != 0) {
            if ((*(u16 *)(self + 0xC0) & 0x10) == 0) {
                *(u16 *)(self + 0xBE) = 5;
                self[0xC2] = 1;
            }
        } else {
            *(u16 *)(self + 0xBE) = 7;
        }
        if (anyTether) {
            for (i = 0; i < 4; i++) {
                if (inRange[i] && Player_BeginDirectionalCue(players[i], 1)) {
                    *(u16 *)(self + 0xC0) |= 0x400;
                }
            }
            TetherActor_BeginDetachedState(1);
        } else {
            for (i = 0; i < 4; i++) {
                if (inRange[i] && Player_BeginDirectionalCue(players[i], 0)) {
                    *(u16 *)(self + 0xC0) |= 0x400;
                }
            }
        }
    } else {
        if (anyTether) {
            ScriptTriggerActor_DispatchPendingScripts(arg0);
            TetherActor_BeginDetachedState(0);
            *(u16 *)(self + 0xBE) = 5;
            self[0xC2] = 1;
        } else if (found != 0) {
            if ((*(u16 *)(self + 0xC0) & 0x10) == 0) {
                *(u16 *)(self + 0xBE) = 5;
                self[0xC2] = 1;
            }
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08106E98);
#endif
