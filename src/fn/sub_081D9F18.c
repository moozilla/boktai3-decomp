#include "global.h"

struct PConfig {
    u16 flags;
    u8 pad2[0x38 - 2];
    s16 variant;
    u8 pad3A[0x48 - 0x3A];
    u32 type;
    s32 graphics;
};

void Projectile_SetSpriteParams(void *, u32, u32, s32);
void Projectile_SetCollisionSize(void *, u32, u32, u32);
void Sound_PlaySong(u32);

void Projectile_InitFromType(struct PConfig *p)
{
    if (p->type != 11) {
        Projectile_SetSpriteParams(p, 0x2112, 0, p->graphics + 0x116);
        if (p->type > 11)
            return;
    }
    switch (p->type) {
        case 1:
            if (p->variant == 1)
                Sound_PlaySong(0x203);
            else
                Sound_PlaySong(0xCD);
            Projectile_SetCollisionSize(p, 0x10, 0x50, 0x10);
            break;
        case 8:
            Sound_PlaySong(0x3CE);
            Projectile_SetCollisionSize(p, 0x10, 0x50, 0x10);
            break;
        case 4:
            Sound_PlaySong(0x3CD);
            Projectile_SetCollisionSize(p, 8, 0x50, 8);
            break;
        case 10:
            Sound_PlaySong(0xCD);
            Projectile_SetCollisionSize(p, 8, 0x50, 8);
            break;
        case 2:
            if (p->variant == 1)
                Sound_PlaySong(0x203);
            else
                Sound_PlaySong(0xCD);
            Projectile_SetCollisionSize(p, 0x10, 0x50, 0x10);
            break;
        case 6:
            Sound_PlaySong(0xCD);
            Projectile_SetCollisionSize(p, 0x20, 0x50, 0x20);
            break;
        case 0:
        case 5:
            Sound_PlaySong(0xCD);
            Projectile_SetCollisionSize(p, 0x10, 0x50, 0x10);
            break;
        case 9:
            Sound_PlaySong(0x164);
            Projectile_SetCollisionSize(p, 8, 0x50, 8);
            break;
        case 3:
            Sound_PlaySong(0x3CF);
            Projectile_SetCollisionSize(p, 8, 0x50, 8);
            break;
        case 7:
            Projectile_SetCollisionSize(p, 8, 0x50, 8);
            break;
        case 11:
            if (!(p->flags & 0x2000)) {
                Sound_PlaySong(0xCD);
                Projectile_SetSpriteParams(p, 0x2112, 0, p->graphics + 0x116);
                Projectile_SetCollisionSize(p, 8, 0x50, 8);
            }
            break;
    }
}
