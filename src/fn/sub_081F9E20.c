#include "global.h"
#include "scene_vec.h"
// foundry draft (worker best 98.2%); what differs is mostly register allocation and instruction order
// Temporary compatibility aliases for the local ELF's pre-promotion symbols.

#define Projectile_InitParams       Projectile_InitializeParams
#define Projectile_RemoveByIndex    Task4E01_RemoveEntry
#define Projectile_CreateAtPosition Projectile_Spawn
struct PositionWords {
    u32 a, b;
};
union PreviewPosition {
    struct SceneVec vec;
    struct PositionWords words;
};
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
struct SceneVec *Task4E01_GetEntryPosition(s32);
s32 Task4E01_RemoveEntry(s32);
struct SceneVec *Player_GetEffectivePosition(s32);
s32 Math_Atan2(s32, s32);
void Projectile_InitializeParams(u8 *);
void Projectile_SetFlightParams(u8 *, u16, u16, u8, u32);
void Projectile_SetDamageParams(u8 *, u32, u32, u32, u32, u32);
void Projectile_SetTypeAndElement(u8 *, u32, u32);
s32 Projectile_Spawn(u8 *, struct SceneVec *);
void Sound_PlaySong(u32);
static inline struct PositionWords *CopyPosition(struct PositionWords *dest,
                                                 const struct PositionWords *src)
{
    *dest = *src;
    return dest;
}
void PreviewActor_ReplaceProjectileTowardPlayer(struct PreviewProjectileContact *contact,
                                                void *unused, struct PreviewActor *actor)
{
    u8 params[0x58];
    s16 origin[4], target[4];
    struct PreviewProjectileOwner *owner = contact->owner;
    struct PositionWords *new_var;
    struct PositionWords *new_var3;
    s16 *new_var2;
    u8 angle;
    s16 *originPtr, *targetPtr;
    if ((contact->flags & 1) && (contact->contactFlags & 0x210) == 0x10 &&
        !(contact->targetFlags & 0x100)) {
        new_var3 = (struct PositionWords *)Task4E01_GetEntryPosition(owner->projectileIndex);
        new_var2 = (s16 *)origin;
        new_var = (struct PositionWords *)new_var2;
        originPtr = (s16 *)CopyPosition(new_var, new_var3);
        if (Task4E01_RemoveEntry(owner->projectileIndex) == 0) {
            new_var2 = originPtr;
            new_var3 = (struct PositionWords *)Player_GetEffectivePosition(0);
            new_var = (struct PositionWords *)target;
            targetPtr = (s16 *)CopyPosition(new_var, new_var3);
            targetPtr[1] += 128;
            angle = Math_Atan2(targetPtr[0] - new_var2[0], targetPtr[2] - new_var2[2]);
            Projectile_InitializeParams(params);
            Projectile_SetFlightParams(params, 0x8000, 90, angle, 36);
            Projectile_SetDamageParams(params, 0, 0, actor->damage, actor->damage2, actor->damage3);
            Projectile_SetTypeAndElement(params, 1, 6);
            if (Projectile_Spawn(params, (struct SceneVec *)new_var2) >= 0)
                Sound_PlaySong(0x56e);
        }
    }
}
