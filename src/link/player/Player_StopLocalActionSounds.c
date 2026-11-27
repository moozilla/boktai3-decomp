#include "global.h"
#include "player/player.h"
#include "link/session/link_session_state.h"
#include "scene/collision/scene_actor_damage_visibility.h"
extern struct AreaEffectLinkState *gLinkSession;
void Sound_StopSong(u32);
void Player_StopLocalActionSounds(struct Player *p)
{
    if (p->slot == SceneActorDamage_LocalPlayer()) {
        if (p->actionMode != 6) {
            Sound_StopSong(0xd8);
        } else {
            Sound_StopSong(0x239);
            Sound_StopSong(0x202);
            Sound_StopSong(0x366);
        }
    }
}
