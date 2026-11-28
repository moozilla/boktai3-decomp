#include "global.h"
#include "bosses/ratatosk/ratatosk.h"
#include "effects/projectiles/projectile_config.h"

struct PreviewProjectileOwner {
    u8 pad0[0x1a8];
    s32 projectileIndex;
};
struct ContactPosition *Player_GetEffectivePosition(u32);
s32 Math_Atan2(s32, s32);
void Sound_PlaySong(u32);
void PreviewActor_ReplaceProjectileTowardPlayer(struct ContactCollider *contact, void *unused,
                                                struct Ratatosk *actor)
{
    struct Task4E01Params params;
    struct ContactPosition origin, target;
    struct PreviewProjectileOwner *owner = contact->owner;
    struct ContactPosition *source;
    u8 angle;
    struct ContactPosition *originPtr, *targetPtr;
    if ((contact->contactFlags & 1) && (contact->statRewardFlags & 0x210) == 0x10 &&
        !(contact->reactionFlags & 0x100)) {
        source = ProjectileTask_GetEntryPosition(owner->projectileIndex);
        originPtr = &origin;
        *originPtr = *source;
        if (ProjectileTask_RemoveEntry(owner->projectileIndex) == 0) {
            source = Player_GetEffectivePosition(0);
            targetPtr = &target;
            *targetPtr = *source;
            targetPtr->y += 128;
            angle = Math_Atan2(targetPtr->x - originPtr->x, targetPtr->z - originPtr->z);
            Projectile_InitializeParams(&params);
            Projectile_SetFlightParams(&params, 0x8000, 90, angle, 36);
            Projectile_SetDamageParams(&params, 0, 0, (s16)actor->range[0], (s16)actor->range[1],
                                       (s16)actor->range[2]);
            Projectile_SetTypeAndElement(&params, 1, 6);
            if (Projectile_Spawn(&params, originPtr) >= 0)
                Sound_PlaySong(0x56e);
        }
    }
}
