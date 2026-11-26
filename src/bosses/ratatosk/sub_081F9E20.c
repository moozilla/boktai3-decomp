#include "engine/shared/global.h"
#include "effects/projectile/projectile_config.h"

struct PreviewProjectileOwner {
    u8 pad0[0x1a8];
    s32 projectileIndex;
};
struct PreviewProjectileContact {
    u8 pad0[0xa];
    u16 flags;
    u8 padC[0x34 - 0xc];
    u32 contactFlags, targetFlags;
    u8 pad3C[0x50 - 0x3c];
    struct PreviewProjectileOwner *owner;
};
struct PreviewActor {
    u8 pad0[0xefc];
    s16 damage, damage2, damage3;
};
struct ContactPosition *Player_GetEffectivePosition(u32);
s32 Math_Atan2(s32, s32);
void Sound_PlaySong(u32);
void PreviewActor_ReplaceProjectileTowardPlayer(struct PreviewProjectileContact *contact,
                                                void *unused, struct PreviewActor *actor)
{
    struct Task4E01Params params;
    struct ContactPosition origin, target;
    struct PreviewProjectileOwner *owner = contact->owner;
    struct ContactPosition *source;
    u8 angle;
    struct ContactPosition *originPtr, *targetPtr;
    if ((contact->flags & 1) && (contact->contactFlags & 0x210) == 0x10 &&
        !(contact->targetFlags & 0x100)) {
        source = Task4E01_GetEntryPosition(owner->projectileIndex);
        originPtr = &origin;
        *originPtr = *source;
        if (Task4E01_RemoveEntry(owner->projectileIndex) == 0) {
            source = Player_GetEffectivePosition(0);
            targetPtr = &target;
            *targetPtr = *source;
            targetPtr->y += 128;
            angle = Math_Atan2(targetPtr->x - originPtr->x, targetPtr->z - originPtr->z);
            Projectile_InitializeParams(&params);
            Projectile_SetFlightParams(&params, 0x8000, 90, angle, 36);
            Projectile_SetDamageParams(&params, 0, 0, actor->damage, actor->damage2,
                                       actor->damage3);
            Projectile_SetTypeAndElement(&params, 1, 6);
            if (Projectile_Spawn(&params, originPtr) >= 0)
                Sound_PlaySong(0x56e);
        }
    }
}
