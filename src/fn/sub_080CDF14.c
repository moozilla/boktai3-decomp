#include "global.h"
#include "scene_actor_reaction.h"
#include "player.h"
/* Golem: build up charge while the target player is not dazzled; charge at the partner when ready and in reach (kind 0x28), else approach (kind 0x23) (tentative) */
u8 Golem_InvokeEnabledBehavior(struct SceneActorRegistryResetView *actor);
u8 sub_0821D4F0(void *, void *, u32, u32);
void Golem_UpdateChargeWindup(void);
void Golem_UpdateRouteMotion(void);
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
static inline void SetControl(struct SceneActorRegistryResetView *actor, u32 kind, void *control)
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
void Golem_ChargeOrApproach(struct SceneActorRegistryResetView *actor)
{
    u8 *body = actor->context;
    if (TakeFlag2B1(actor))
        actor->field2B0 = 0;
    if (Golem_InvokeEnabledBehavior(actor))
        return;
    if (actor->field2B0) {
        Golem_Charge(body);
        if (CanCharge(body)) {
            void *target = Golem_UpdateChargeWindup;
            SetControl(actor, 0x28, target);
            {
                u32 phase = 2, behavior = 4;
                actor->state2A8 = 0;
                actor->phase2A9 = phase;
                actor->behavior2AA = behavior;
                actor->counter2AC = 0;
                actor->field2B1 = 1;
            }
            return;
        }
        {
            void *target = Golem_UpdateRouteMotion;
            SetControl(actor, 0x23, target);
        }
        {
            u32 phase = 2;
            actor->state2A8 = 0;
            actor->phase2A9 = phase;
            actor->behavior2AA = 1;
        }
        actor->counter2AC = 0;
        actor->field2B1 = 1;
    } else
        actor->counter2AC++;
}
