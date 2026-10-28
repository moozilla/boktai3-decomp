#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"
extern const s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);
s32 Div(s32, s32);
void CpuSet(const void *, void *, u32);
void sub_0809345C(void *, void *);
void Bee_SubVector(void *, void *, void *);
s32 Bee_GetMovementVariant(void *);
static inline u8 Take(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}
static inline u8 Test(u32 v, u32 m)
{
    if (v & m)
        return 1;
    return 0;
}
static inline s32 Shift(s32 n, u32 bits)
{
    if (n >= 0)
        return n >> bits;
    return -((-n) >> bits);
}
static inline u32 Rand(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return gRandomTable[gRandomIndex];
}
static inline void Polar(struct ContactPosition *out, u32 angle, s32 speed)
{
    out->x = Shift(gUnk_086149C4[(angle + 64) & 255] * speed, 12);
    out->y = 0;
    out->z = Shift(gUnk_086149C4[angle & 255] * speed, 12);
}
static inline void Delta(struct ContactPosition *out, const struct ContactPosition *from,
                         const struct ContactPosition *to)
{
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}
static inline void Add(struct ContactPosition *out, const struct ContactPosition *from)
{
    out->x = from->x + out->x;
    out->y = out->y + from->y;
    out->z = from->z + out->z;
}
static inline void Control(struct SceneActorRegistryResetView *a, void *cb, u32 kind)
{
    u32 zero = 0;
    a->field2B2 = 1;
    a->field2B0 = zero;
    a->field2B5 = kind;
    a->target = cb;
    a->field2C8 = zero;
    a->flags114 &= ~1;
    a->word104 = zero;
}
static inline void State(struct SceneActorRegistryResetView *a, u32 phase, u32 behavior)
{
    a->state2A8 = 0;
    a->phase2A9 = phase;
    a->behavior2AA = behavior;
    a->counter2AC = 0;
    a->field2B1 = 1;
}

#include "scene_actor_animation.h"
#include "engine.h"
#include "player.h"
#include "ram.h"
void Ghost_GetDirectionalFrame(u8, u32 *, u32 *);
s32 Mod(s32, s32);
void Player_SubtractEnergy(void *, u32);
void sub_0813D834(void *, u32, u32);
static inline void Select(struct SceneActorAnimationView *actor, u32 selection, u32 variant,
                          u32 flags)
{
    u32 force;
    u8 *restart;
    if (flags & 0x10) {
        force = 0x10;
        flags &= ~0x10;
    } else
        force = 0;
    if (!actor->drawableKind) {
        struct SceneActorAnimationSequenceDrawable *drawable;
        actor->selection = selection;
        drawable = actor->drawable;
        Anim_SetSequence(drawable->sequence, (void *)drawable->resource, (u16)selection,
                         (u8)variant, (u8)flags);
        restart = &actor->restart;
    } else {
        struct SceneActorAnimationTile *tile =
            &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile;
        u8 *savedSelection;
        selection += variant;
        savedSelection = &actor->selection;
        restart = &actor->restart;
        if (!force && *savedSelection == selection && !*restart) {
            if (flags == 1 && (tile->flags & 4))
                return;
            if (flags == 0 && !(tile->flags & 4))
                return;
        }
        *savedSelection = selection;
        {
            u32 mask = 4;
            u32 mode;
            if (flags & mask)
                mode = 6;
            else
                mode = 4;
            Anim_SetKeyframes(&((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile,
                              actor->drawable, (u16)selection, 2, (u16)mode);
            if (flags & 1)
                tile->flags |= mask;
            else
                tile->flags &= ~4;
        }
    }
    *restart = 0;
}

static inline u16 SetEffect(struct SceneActorAnimationView *a, u16 mask)
{
    u16 *p = (void *)((u8 *)a + 0x2D6);
    return *p = mask | *p;
}
static inline s32 WrapCounter(s32 n)
{
    return n - 256;
}
// confidence=low: drains player energy while tracking the player's wrapping progress counter.
void Ghost_UpdateEnergyDrainAnimation(struct SceneActorAnimationView *actor)
{
    u8 *body = *(u8 **)((u8 *)actor + 0x3D0);
    struct SceneActorProfileEntry *entry =
        (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, 1);
    u32 variant, flags, selection;
    s32 delta;
    u8 *elapsed;
    if (Take(&actor->directionalEntryFlag)) {
        {
            u32 value = ((u8 **)gPlayers)[0][0x4F4];
            u8 *p = body + 0x5BE;
            u32 zero = 0;
            *p = value;
            *(u16 *)(body + 0x562) = zero;
        }
        *(u16 *)((u8 *)actor + 0x2C2) = 16;
    }
    Ghost_GetDirectionalFrame(actor->direction, &variant, &flags);
    selection = entry->value;
    Select(actor, selection, variant, flags);
    if (*(u16 *)((u8 *)actor + 0x1E6) == 0) {
        if (Mod(*(u16 *)((u8 *)actor + 0x2CA), body[0x560]) == 0)
            Player_SubtractEnergy(((u8 **)gPlayers)[0], *(u16 *)((u8 *)actor + 0xB4));
    } else if (*(u16 *)((u8 *)actor + 0x1E6) == 1) {
        if (Mod(*(u16 *)((u8 *)actor + 0x2CA), body[0x560]) == 0)
            Player_SubtractEnergy(((u8 **)gPlayers)[0], *(u16 *)((u8 *)actor + 0xB4));
        if (((u8 **)gPlayers)[0])
            sub_0813D834(((u8 **)gPlayers)[0], 1, 900);
    }
    {
        s32 start = body[0x5BE], now = ((u8 **)gPlayers)[0][0x4F4];
        if (now < start) {
            s32 wrapped = WrapCounter(start);
            delta = now - wrapped;
        } else
            delta = now - start;
    }
    elapsed = body + 0x5BD;
    if (*elapsed != delta) {
        if (Mod(*elapsed, 4) == 0)
            SetEffect(actor, 32);
        *elapsed = delta;
    }
    if (body[0x5BD] > body[0x561])
        actor->unknown2B0 = 1;
}
