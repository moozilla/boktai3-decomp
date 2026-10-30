#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"
#include "player.h"
/* Golem_ChargeOrReturnHome: builds up charge and cools the charge timer; if the target player is dazzled goes idle (kind 0x27), else charges at the partner when ready (kind 0x28); otherwise walks back to the centre of its home cell, idling there once within reach (tentative) */
u8 Golem_InvokeEnabledBehavior(struct SceneActorRegistryResetView *actor);
u8 sub_0821D4F0(void *, void *, u32, u32);
void Golem_UpdateChargeWindup(void);
void Golem_UpdateRouteMotion(void);
void Golem_UpdateChargeStop(void);
void sub_080C47B8(struct ContactPosition *);
s32 sub_080C47DC(u8 *);
s32 Math_Atan2(s32, s32);
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
/* Player slot of an actor (byte +8), or -1 when it is not a player (> 3). */
static inline s32 PlayerSlot(u8 *actor)
{
    if (actor[8] <= 3)
        return actor[8];
    return -1;
}
static inline u8 TakeFlag2B1(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2B1) {
        actor->field2B1 = 0;
        return 1;
    }
    return 0;
}
/* Charge up while the target player is not dazzled (flag 0x20000): the target is the partner's
 * player slot in co-op modes, else player 0. The charge grows faster the closer the body. */
static inline void Golem_Charge(void *ctx)
{
    u8 *body = ctx;
    u8 *player;
    if (Test(gEngineFlags, 0x4800)) {
        u8 *partner = *(u8 **)(body + 0xDC);
        s32 index;
        if (!partner)
            goto charge;
        index = PlayerSlot(partner);
        if (index < 0)
            goto charge;
        player = gPlayers[index];
    } else
        player = gPlayers[0];
    {
        u32 mask = 0x20000;
        if (*(u32 *)(player + 0x20) & mask)
            return;
    }
charge: {
    u32 mask = 0x80;
    if (*(u16 *)(body + 0x2D4) & mask) {
        s32 gain = 0x400000 - *(u32 *)(body + 0xF0);
        u16 *charge;
        if (gain < 0)
            gain = 0;
        charge = (u16 *)((u8 *)ctx + 0x688);
        *charge += gain >> 16;
        if (*charge > 400)
            *charge = 400;
    }
}
}
static inline void SetControl(struct SceneActorRegistryResetView *actor, void *control, u32 kind)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = control;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = 0;
}
static inline void SetPhase(struct SceneActorRegistryResetView *actor, u32 phase, u32 behavior)
{
    actor->state2A8 = 0;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
/* Can the body charge at its partner: enough stored charge, closer than its range, in reach? */
static inline s32 CanCharge(void *ctx)
{
    u8 *body = ctx;
    u8 *pos = body + 0x50;
    u8 ready;
    if (!*(void **)(body + 0xDC))
        return 0;
    ready = 0;
    if (*(u16 *)(body + 0x1E6) == 0) {
        if (*(u16 *)(body + 0x688) > 399)
            ready = 1;
    } else if (*(u16 *)(body + 0x688) > 199)
        ready = 1;
    if (ready && *(u32 *)(body + 0xF0) < *(u32 *)(body + 0xF8) &&
        sub_0821D4F0(pos + 0xC, *(u8 **)(body + 0xDC) + 0xC, 10, 256))
        return 1;
    return 0;
}
void Golem_ChargeOrReturnHome(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)&actor->group;
    u8 *body = actor->context;
    u8 coop;
    if (TakeFlag2B1(actor))
        actor->field2B0 = 0;
    if (Golem_InvokeEnabledBehavior(actor))
        return;
    if (actor->field2B0) {
        Golem_Charge(body);
        *(u16 *)(body + 0x680) -= 0x1E;
        coop = Test(gEngineFlags, 0x4800);
        if (coop) {
            u8 *partner = (u8 *)actor->tracking.target;
            s32 index;
            if (!partner)
                goto check;
            index = PlayerSlot(partner);
            if (index < 0)
                goto check;
            {
                u8 *player = gPlayers[index];
                u32 mask = 0x20000;
                if (!(*(u32 *)(player + 0x20) & mask))
                    goto check;
            }
            SetControl(actor, Golem_UpdateChargeStop, 0x27);
            SetPhase(actor, 2, 3);
            return;
        } else {
            u8 *player = gPlayers[0];
            u32 mask = 0x20000;
            if (!(*(u32 *)(player + 0x20) & mask))
                goto check;
            SetControl(actor, Golem_UpdateChargeStop, 0x27);
            SetPhase(actor, 2, 3);
            return;
        }
    check:
        if (*(s16 *)(body + 0x680) <= 0) {
            if (!CanCharge(body)) {
                SetControl(actor, Golem_UpdateChargeStop, 0x27);
                SetPhase(actor, 2, 3);
                return;
            }
        } else if (!CanCharge(body))
            goto approach;
        SetControl(actor, Golem_UpdateChargeWindup, 0x28);
        SetPhase(actor, 2, 4);
        return;
    approach: {
        struct ContactPosition center, delta;
        SceneActorTerrain_GetCenter(actor->terrainCell, &center);
        sub_080C47B8(&center);
        SceneActorTerrain_Subtract(&delta, &actor->body.position, &center);
        if (delta.x * delta.x + delta.z * delta.z <= 0xFFF) {
            sub_080C47DC(body);
            SetPhase(actor, 2, 6);
            return;
        }
        *(struct ContactPosition *)(*(u8 **)(group + 0x1FC) + 0x1C) = center;
        SceneActorTerrain_Subtract(&delta, &center, &actor->body.position);
        {
            u8 direction = ((Math_Atan2(delta.x, delta.z) + 0x20) & 0xFF) >> 6 << 6;
            ((u8 *)actor)[0x59] = direction;
            body[0x68E] = direction;
        }
        SetControl(actor, Golem_UpdateRouteMotion, 0x23);
        SetPhase(actor, 2, 1);
    }
    } else
        actor->counter2AC++;
}
