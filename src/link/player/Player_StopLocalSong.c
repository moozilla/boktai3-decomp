#include "global.h"
#include "scene/collision/scene_actor_damage_visibility.h"

void Sound_StopSong(u32);

void Player_StopLocalSong(s32 player, u32 song)
{
    if (player == SceneActorDamage_LocalPlayer())
        Sound_StopSong(song);
}
