#include "stacked_actor.h"
#define StackedActor_TickIdle sub_08052A40
extern u32 gUnk_030053F4, gUnk_02000560;
extern u8 *gUnk_02000580;
extern u16 gUnk_03005260[];
s32 sub_08052330(struct StackedActor *);
void Sound_PlaySong(u32);
void AnimData_SelectTiles(void *, u32);
void sub_0815B0A8(u8 *, struct StackedActorPosition *, u32, u32);
void StackedActor_TickIdle(struct StackedActor *actor)
{
    if (!(gUnk_030053F4 & 0x200) && actor->interactionDelay <= 9)
        actor->interactionDelay++;
    {
        s32 nearby = sub_08052330(actor);
        if (nearby) {
            if (!actor->interactionEnabled) {
                u32 mask = 1;
                if ((gUnk_02000560 & mask) && gUnk_02000580[0x457] != 0x1D &&
                    gUnk_02000580[0x457] != 0x1E && (gUnk_03005260[1] & 1))
                    Sound_PlaySong(0x124);
            } else {
                u16 *state = &actor->proximityState;
                if (*state != 1) {
                    Sound_PlaySong(0x198);
                    *state = 1;
                }
                if (actor->animationPhase <= 7)
                    actor->animationPhase++;
                if (actor->lift <= 22)
                    actor->lift += 2;
                else
                    actor->lift = 24;
                {
                    u32 mask = 1;
                    if ((gUnk_02000560 & mask) && (gUnk_03005260[1] & 1)) {
                        u8 *player = gUnk_02000580;
                        u32 playerMask = 0x100000;
                        if (!(*(u32 *)(player + 0x20) & playerMask) &&
                            actor->interactionDelay > 9) {
                            actor->animationPhase = 8;
                            sub_0815B0A8(gUnk_02000580, &actor->world, 0, 4);
                            sub_08052628(actor, sub_08052C28);
                        }
                    }
                }
            }
        } else {
            u16 *state = &actor->proximityState;
            if (*state) {
                Sound_PlaySong(0x199);
                *state = nearby;
            }
            if (actor->animationPhase)
                actor->animationPhase--;
            if (actor->lift > 2)
                actor->lift -= 2;
            else
                actor->lift = nearby;
        }
    }
    if (!actor->lift)
        actor->sprite.flags |= 1;
    else {
        actor->sprite.flags &= ~1;
        actor->sprite.position.y = (u16)actor->world.y + actor->lift;
    }
    if (actor->animationPhase > 7)
        AnimData_SelectTiles(actor->graphics, actor->tileBase + 2);
    else if (actor->animationPhase)
        AnimData_SelectTiles(actor->graphics, actor->tileBase + 1);
    else
        AnimData_SelectTiles(actor->graphics, actor->tileBase);
}
