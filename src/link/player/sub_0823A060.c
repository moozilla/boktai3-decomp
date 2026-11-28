#include "global.h"
#include "player/player.h"
#include "player/player_status_flags.h"
#include "scene/collision/scene_actor_damage_visibility.h"

void Player_PlayLocalSong(s32, s32);

static inline s32 LinkPlayer_IsInSceneModeFour(struct Player *other)
{
    if (other->active != 0 && other->sceneMode == 4)
        return 1;
    return 0;
}

/* The aura fade level lives in visualSlowdownTimer; unk4E1[0] marks a pending end sound. */
static inline void LinkPlayer_FadeOutAura(struct Player *p)
{
    if (p->visualSlowdownTimer != 0) {
        if (p->unk4E1[0] != 0) {
            Player_PlayLocalSong(p->slot, 0x336);
            p->unk4E1[0] = 0;
        }
        p->visualSlowdownTimer -= 1;
        if (p->visualSlowdownTimer == 0) {
            p->animation.flags &= ~2;
            p->animation.scaleX = 0x40;
            p->animation.scaleY = 0x40;
        } else {
            s32 v;
            p->animation.flags |= 2;
            v = 0x40 - (p->visualSlowdownTimer >> 1);
            p->animation.scaleX = v;
            p->animation.scaleY = v;
        }
    }
}

void sub_0823A060(struct Player *p)
{
    if (p->active != 0) {
        if (p->sceneMode == 5) {
            s32 v;
            if (p->visualSlowdownTimer <= 0x3f) {
                if (p->visualSlowdownTimer == 0) {
                    Player_PlayLocalSong(p->slot, 0x1ee);
                    p->unk4E1[0] = 1;
                }
                p->visualSlowdownTimer += 1;
            }
            p->animation.flags |= 2;
            v = 0x40 - (p->visualSlowdownTimer >> 1);
            p->animation.scaleX = v;
            p->animation.scaleY = v;
            Player_AddStatusFlags(p, 0x50000);
        } else {
            LinkPlayer_FadeOutAura(p);
        }
        if (p->sceneMode == 6) {
            s32 side = SceneActorDamage_LocalPlayer();
            struct Player *other;
            if (p->slot == side) {
                p->secondaryDrawFlags |= 0x200;
                p->animation.flags |= 0x200;
                Player_AddStatusFlags(p, 0x20000);
            } else if ((other = gPlayers[side]) != 0 && LinkPlayer_IsInSceneModeFour(other)) {
                p->secondaryDrawFlags |= 0x200;
                p->animation.flags |= 0x200;
                Player_AddStatusFlags(p, 0x20000);
            } else {
                p->secondaryDrawFlags |= 1;
                p->animation.flags |= 1;
                Player_AddStatusFlags(p, 0x20000);
            }
        } else {
            p->secondaryDrawFlags &= ~0x200;
            p->animation.flags &= ~0x200;
        }
        if (p->kind & 1)
            p->active--;
    } else {
        LinkPlayer_FadeOutAura(p);
        if (p->animation.flags & 0x200)
            p->animation.flags &= ~0x200;
        if (p->secondaryDrawFlags & 0x200)
            p->secondaryDrawFlags &= ~0x200;
    }
}
