#ifndef GUARD_INCLUDE_PLAYER_EFFECTS_RECOIL_PUFF_SPRITE_H
#define GUARD_INCLUDE_PLAYER_EFFECTS_RECOIL_PUFF_SPRITE_H

#include "global.h"

struct RecoilPuffSprite {
    u32 flags;
    u8 pad4[0xb];
    u8 f0F; /* confidence=low */
    u8 pad10[0xa];
    u16 height;
    u8 pad1C[0xc];
    void *resource;
    u8 state; /* confidence=low */
    u8 phase; /* confidence=low */
    u16 timer; /* confidence=low */
};

typedef char RecoilPuffSpriteSizeCheck[sizeof(struct RecoilPuffSprite) == 0x30 ? 1 : -1];

#endif
