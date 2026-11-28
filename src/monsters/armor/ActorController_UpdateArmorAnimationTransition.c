#include "monsters/armor/scene_combat_colliders.h"

/* tentative: state machine on 0xB40 starting armor animation, playing songs by armor type */

extern u32 gUnk_0824EF00[];
void Armor_SetAnimationFromStart(struct SceneCombatActor *, const void *);
void Sound_PlaySongChecked(u32 a);
void Armor_InitPaletteBanks(void *p);
void DirectionalActor_TryBeginState(void *p, u32 a);

void ActorController_UpdateArmorAnimationTransition(struct SceneCombatActor *actor)
{
    u32 song;

    switch (actor->actionPhase) {
        case 0:
            Armor_SetAnimationFromStart(actor, &gUnk_0824EF00[((actor->heading + 0x60) >> 6) & 3]);
            actor->actionPhase++;
            actor->actionReady = 0;
            SceneCombatActor_SelectFirstCollider(actor);
            actor->flags &= ~0x20;
            switch (actor->kind) {
                case 3:
                case 5:
                case 7:
                case 9:
                case 13:
                case 15:
                    Armor_InitPaletteBanks(actor);
                    Sound_PlaySongChecked(0x14B);
                    break;
            }
            return;
        case 1:
            switch (actor->kind) {
                case 0:
                case 1:
                    if (actor->animationEvent != 3)
                        break;
                    song = 0xBE;
                    goto play;
                case 2:
                case 6:
                case 11:
                case 12:
                    if (actor->animationEvent != 3)
                        break;
                    song = 0x5AB;
                    goto play;
                case 4:
                case 8:
                case 14:
                    if (actor->animationEvent != 3)
                        break;
                    song = 0xBF;
                    goto play;
                case 3:
                case 5:
                case 7:
                case 9:
                case 13:
                case 15:
                    if (actor->animationEvent != 9)
                        break;
                    song = 0xC3;
                play:
                    Sound_PlaySongChecked(song);
                    actor->actionPhase++;
                    break;
            }
        case 2:
            if (actor->animationComplete != 0) {
                actor->actionReady = 1;
                DirectionalActor_TryBeginState(actor, 0);
            }
            break;
    }
}
