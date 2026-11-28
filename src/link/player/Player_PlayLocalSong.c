#include "global.h"
#include "scene/collision/scene_actor_damage_visibility.h"

void Sound_PlaySong(u32);

void Player_PlayLocalSong(s32 player, u32 song)
{
    if (player == SceneActorDamage_LocalPlayer())
        Sound_PlaySong(song);
}
