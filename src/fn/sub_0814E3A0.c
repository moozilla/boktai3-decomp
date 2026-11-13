#include "global.h"
#include "player.h"
#include "scene_vec.h"
void Projectile_SetEntryDirection(s32, s32);
void PlayerContact_GetOffsetPosition(s32, struct SceneVec *);
void SceneEntry_SetPosition(s32, struct SceneVec *);
void Player_UpdateAttachedEntryPosition(struct Player *p)
{
    struct SceneVec position;
    s32 *entry = &p->statModifierContext.attachedEntry;
    if (*entry >= 0) {
        Projectile_SetEntryDirection(p->unk2D, *entry);
        PlayerContact_GetOffsetPosition(0, &position);
        SceneEntry_SetPosition(*entry, &position);
    }
}
