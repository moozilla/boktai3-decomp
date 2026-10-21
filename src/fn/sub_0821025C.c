#include "contact_actor.h"

// MATCHING_CONCESSION: register-pinning

#define gRandomTable gUnk_0203B400
#define gRandomIndex gUnk_03005308
struct ProximityProjectileEmitter {
    u8 pad0[0x18];
    s32 frame;
    u8 pad1C[0x24];
    struct ContactPosition position;
    u8 pad48[0xF2];
    s16 disabled;
};
extern const u16 gUnk_0203B400[];
extern u32 gUnk_03005308;
void Combat_SpawnDirectionalProjectile(struct ProximityProjectileEmitter *, u8);
void sub_0820FD54(struct ProximityProjectileEmitter *);
struct SpriteEffectController *SpriteEffectController_Create(const struct ContactPosition *,
                                                             const struct ContactPosition *, s32,
                                                             u32, u32, u32, u32, u32, u32, u32, u32,
                                                             u32, s32, s32, s32, s32, s32, s32);
static inline s32 randomValue(u32 table)
{
    u32 offset;
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    offset = gUnk_03005308 * 2;
    return *(const u16 *)(offset + (u32)table);
}
static inline void copy(struct ContactPosition *d, const struct ContactPosition *s)
{
    *d = *s;
}
static inline void raisePosition(struct ContactPosition *p)
{
    p->y += 0x80;
}
u32 ProjectileEmitter_EmitAttackFrame(struct ProximityProjectileEmitter *p)
{
    struct ContactPosition position;
    struct ContactPosition offset;
    if (!p->disabled) {
        register s32 f asm("r12") = p->frame;
        if (f <= 8) {
            copy(&position, &p->position);
            raisePosition(&position);
            {
                offset.x = (randomValue((u32)gUnk_0203B400) & 15) - 8;
                offset.y = 0;
                offset.z = (randomValue((u32)gUnk_0203B400) & 15) - 8;
            }
            if (f & 1)
                SpriteEffectController_Create(&position, &offset, 0x61F9, 0x31F4, 8, 0x200, 9,
                                              0x1C6, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            else
                SpriteEffectController_Create(&position, &offset, 0x61F9, 0x31F4, 8, 0x400, 9,
                                              0x1C6, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            if (p->frame <= 8)
                Combat_SpawnDirectionalProjectile(p, p->frame - 1);
        }
        if (p->frame == 9)
            sub_0820FD54(p);
    }
    return 0;
}
