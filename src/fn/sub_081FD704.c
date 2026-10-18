#include "global.h"
#define OrbitEffect_FireProjectileOnContact sub_081FD704
#define Projectile_InitializeParams         sub_081DAC34
#define Projectile_Spawn                    sub_081DAB00
#define Task4E01_ReleaseEntry               sub_081DAD28
struct OrbitEffectWords {
    u32 a, b;
};
struct OrbitEffectPosition {
    s16 x, y, z, pad;
};
struct OrbitEffectOwner {
    u8 unknown0[0x1a8];
    s32 entry;
};
struct OrbitEffectContact {
    u8 unknown0[0xa];
    u16 flags;
    u8 unknownC[0x34 - 0xc];
    u32 type, status;
    u8 unknown3C[0x50 - 0x3c];
    struct OrbitEffectOwner *owner;
};
s32 Math_Atan2(s32, s32);
struct OrbitEffectPosition *Player_GetEffectivePosition(s32);
struct OrbitEffectPosition *Task4E01_GetEntryPosition(s32);
void sub_081DAD28(s32);
void sub_081DAC34(u8 *);
void Projectile_SetFlightParams(u8 *, u16, u16, u8, u32);
void Projectile_SetTypeAndElement(u8 *, u32, u32);
s32 sub_081DAB00(u8 *, struct OrbitEffectPosition *);
void sub_081FD704(struct OrbitEffectContact *p)
{
    u8 params[0x58];
    struct OrbitEffectPosition origin, target;
    struct OrbitEffectPosition *originPtr, *targetPtr, *source;
    struct OrbitEffectOwner *owner = p->owner;
    u32 direction;
    if ((p->flags & 1) && (p->type & 0x210) == 0x10 && !(p->status & 0x100)) {
        s32 *entry = &owner->entry;
        source = Task4E01_GetEntryPosition(*entry);
        originPtr = &origin;
        *(struct OrbitEffectWords *)originPtr = *(struct OrbitEffectWords *)source;
        sub_081DAD28(*entry);
        source = Player_GetEffectivePosition(0);
        targetPtr = &target;
        *(struct OrbitEffectWords *)targetPtr = *(struct OrbitEffectWords *)source;
        targetPtr->y += 0x80;
        direction = (u8)Math_Atan2(targetPtr->x - originPtr->x, targetPtr->z - originPtr->z);
        sub_081DAC34(params);
        Projectile_SetFlightParams(params, 0x8000, 0x5a, direction, 0x18);
        Projectile_SetTypeAndElement(params, 1, 6);
        sub_081DAB00(params, originPtr);
    }
}
