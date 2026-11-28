#include "global.h"
#include "player/player.h"
#include "engine/math/scene_vec.h"
void Projectile_SetEntryDirection(s32, s32);
void PlayerContact_GetOffsetPosition(s32, struct SceneVec *);
void SceneEntry_SetPosition(s32, struct SceneVec *);
void Player_UpdateAttachedEntryPosition(struct Player *p)
{
    struct SceneVec position;

    if (p->statModifierContext.attachedEntry >= 0) {
        Projectile_SetEntryDirection(p->unk2D, p->statModifierContext.attachedEntry);
        PlayerContact_GetOffsetPosition(0, &position);
        SceneEntry_SetPosition(p->statModifierContext.attachedEntry, &position);
    }
}
