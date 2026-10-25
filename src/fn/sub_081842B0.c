#include "global.h"

/* tentative: 4-state armor defeat sequence with particle bursts */

struct ActorAnimationLayer {
    u16 unknown[3];
    u16 flags;
};
struct ActorRenderFlags {
    u32 unknown, flags;
};
extern const u32 gUnk_0824EE40[];
void Sound_PlaySongChecked(u32);
void Armor_SetAnimationFromStart(u32, u32);
void Armor_SpawnRandomDirectionalParticle(u8 *);
void Armor_SpawnDirectionalParticleFan(u8 *);
void Anim_UnlinkFromList(void *, u32);
void ScaledShadow_Disable(void *);
void ActorMarker_InitMovingSprite(void *);
void SceneCombatActor_SpawnBouncingEffect(u8 *);
void ActorMarker_LaunchFragments(u8 *, u32);
void SpriteColliderGroup_ResetActiveParts(void *);
void ActorMarker_InitOffsetPosition(u8 *);
static inline void Armor_CheckFlagRefresh(u8 *q)
{
    if (*(u16 *)(q + 0xb32) & 1)
        ActorMarker_InitOffsetPosition(q);
}
void Armor_UpdateDefeatSequence(u8 *p)
{
    u8 *s = p + 0xb40;
    u8 st = *s;
    switch (st) {
        case 0: {
            u32 four = 4;
            u32 one;
            Armor_SetAnimationFromStart((u32)p, (u32)&gUnk_0824EE40[((p[0xb39] + 0x60) >> 6) & 3]);
            (*s)++;
            p[0xb41] = st;
            ((struct ActorAnimationLayer *)(p + 0x238))->flags |= four;
            ((struct ActorAnimationLayer *)(p + 0x1e4))->flags |= four;
            ((struct ActorAnimationLayer *)(p + 0x28c))->flags |= four;
            ((struct ActorAnimationLayer *)(p + 0x190))->flags |= four;
            {
                struct ActorRenderFlags *render = (void *)(p + 0x180);
                one = 1;
                render->flags |= one;
            }
            *(u16 *)(p + 0xb32) &= ~0x20;
            Armor_CheckFlagRefresh(p);
        }
        case 1:
            if (!(p[0xb42] & 7))
                Armor_SpawnRandomDirectionalParticle(p);
            if (p[0xb42] > 14)
                p[0xb40]++;
            break;
        case 2:
            *(u32 *)(p + 0xbc) |= 1;
            Anim_UnlinkFromList(p + 0xb4, 0);
            ScaledShadow_Disable(p + 0x114);
            Armor_SpawnDirectionalParticleFan(p);
            Armor_SpawnDirectionalParticleFan(p);
            Armor_SpawnDirectionalParticleFan(p);
            ActorMarker_InitMovingSprite(p);
            SceneCombatActor_SpawnBouncingEffect(p);
            ActorMarker_LaunchFragments(p, 0);
            SpriteColliderGroup_ResetActiveParts(p + 0xb58);
            {
                u8 v = *s + 1;
                u8 zero = 0;
                *s = v;
                p[0xb42] = zero;
            }
        case 3:
            if (!(p[0xb42] & 3)) {
                Armor_SpawnRandomDirectionalParticle(p);
                Sound_PlaySongChecked(0x16e);
            }
            if (p[0xb42] > 0x1d)
                p[0xb40] = 0xff;
            break;
    }
}
