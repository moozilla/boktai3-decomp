#include "stacked_actor.h"
#include "input.h"
#include "engine_globals.h"
#include "player.h"
extern u32 gUnk_02000560;
s32 StackedActor_IsPlayerNearby(struct StackedActor *);
void Sound_PlaySong(u32);
void AnimData_SelectPaletteBank(void *, u32);
void PlayerCue_WalkTo(u8 *, struct StackedActorPosition *, u32, u32);
void StackedActor_TickIdle(struct StackedActor *actor)
{
    if (!(gEngineFlags & 0x200) && actor->interactionDelay <= 9)
        actor->interactionDelay++;
    {
        s32 nearby = StackedActor_IsPlayerNearby(actor);
        if (nearby) {
            if (!actor->interactionEnabled) {
                u32 mask = 1;
                if ((gUnk_02000560 & mask) && (*(u8 **)gPlayers)[0x457] != 0x1D &&
                    (*(u8 **)gPlayers)[0x457] != 0x1E && (gPadState[1] & 1))
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
                    if ((gUnk_02000560 & mask) && (gPadState[1] & 1)) {
                        u8 *player = (*(u8 **)gPlayers);
                        u32 playerMask = 0x100000;
                        if (!(*(u32 *)(player + 0x20) & playerMask) &&
                            actor->interactionDelay > 9) {
                            actor->animationPhase = 8;
                            PlayerCue_WalkTo((*(u8 **)gPlayers), &actor->world, 0, 4);
                            StackedActor_SetUpdate(actor, StackedActor_BeginPlayerInteraction);
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
        AnimData_SelectPaletteBank(actor->graphics, actor->paletteBase + 2);
    else if (actor->animationPhase)
        AnimData_SelectPaletteBank(actor->graphics, actor->paletteBase + 1);
    else
        AnimData_SelectPaletteBank(actor->graphics, actor->paletteBase);
}
