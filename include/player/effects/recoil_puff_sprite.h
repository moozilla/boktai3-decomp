#ifndef GUARD_INCLUDE_PLAYER_EFFECTS_RECOIL_PUFF_SPRITE_H
#define GUARD_INCLUDE_PLAYER_EFFECTS_RECOIL_PUFF_SPRITE_H

#include "engine/task.h"
#include "scene/collision/contact_position.h"

struct RecoilPuffSprite {
    u32 flags;
    u8 pad4[0xb];
    u8 priority;
    u8 unknown10[8];
    struct ContactPosition position;
    u8 unknown20[8];
    void *resource;
    u8 active;
    u8 phase; /* confidence=low */
    u16 timer; /* confidence=low */
};

typedef char RecoilPuffSpriteSizeCheck[sizeof(struct RecoilPuffSprite) == 0x30 ? 1 : -1];

struct RecoilPuffPool {
    struct Task task;
    struct RecoilPuffSprite entries[8];
    u16 next;
};

typedef char RecoilPuffPoolSizeCheck[sizeof(struct RecoilPuffPool) == 0x19C ? 1 : -1];

struct RecoilPuffPool *RecoilPuffPool_GetOrCreate(void);
s32 RecoilPuffPool_Init(struct RecoilPuffPool *);
u32 RecoilPuffPool_UpdateTask(struct RecoilPuffPool *);
u32 RecoilPuffPool_DestroyTask(struct RecoilPuffPool *);
void RecoilPuff_Update(struct RecoilPuffSprite *);
void sub_08237280(struct RecoilPuffPool *);

#endif
