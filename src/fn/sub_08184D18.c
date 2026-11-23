#include "scene_combat_colliders.h"

/* tentative: state machine on 0xB40 starting armor animation, playing songs by armor type */

extern u32 gUnk_0824EF00[];
void Armor_SetAnimationFromStart(struct SceneCombatActor *, const void *);
void Sound_PlaySongChecked(u32 a);
void Armor_InitPaletteBanks(void *p);
void DirectionalActor_TryBeginState(void *p, u32 a);

void ActorController_UpdateArmorAnimationTransition(struct SceneCombatActor *actor)
{
    u8 *p = (void *)actor;
    u8 *s = p + 0xB40;
    u8 st = *s;
    u32 song;

    switch (st) {
        case 0: {
            Armor_SetAnimationFromStart(actor, &gUnk_0824EF00[((p[0xB39] + 0x60) >> 6) & 3]);
            (*s)++;
            p[0xB41] = st;
            SceneCombatActor_SelectFirstCollider((struct SceneCombatActor *)p);
            *(u16 *)(p + 0xB32) &= ~0x20;
            switch (p[0xB38]) {
                case 3:
                case 5:
                case 7:
                case 9:
                case 13:
                case 15:
                    Armor_InitPaletteBanks(p);
                    Sound_PlaySongChecked(0x14B);
                    break;
            }
            return;
        }
        case 1:
            switch (p[0xB38]) {
                case 0:
                case 1:
                    if (*(u16 *)(p + 0xC8) != 3)
                        break;
                    song = 0xBE;
                    goto play;
                case 2:
                case 6:
                case 11:
                case 12:
                    if (*(u16 *)(p + 0xC8) != 3)
                        break;
                    song = 0x5AB;
                    goto play;
                case 4:
                case 8:
                case 14:
                    if (*(u16 *)(p + 0xC8) != 3)
                        break;
                    song = 0xBF;
                    goto play;
                case 3:
                case 5:
                case 7:
                case 9:
                case 13:
                case 15:
                    if (*(u16 *)(p + 0xC8) != 9)
                        break;
                    song = 0xC3;
                play:
                    Sound_PlaySongChecked(song);
                    p[0xB40]++;
                    break;
            }
        case 2:
            if (p[0xB43] != 0) {
                p[0xB41] = 1;
                DirectionalActor_TryBeginState(p, 0);
            }
            break;
    }
}
