#include "global.h"
struct PhaseSprite {
    u32 flags;
    u8 pad4[2];
    u8 field6, pad7;
    s8 scaleX, scaleY;
};
struct PhaseCollision {
    u8 pad0[6];
    u16 flags;
};
struct PhaseActor {
    u8 pad0[4];
    struct PhaseSprite *sprite;
    u8 pad8[0xfc];
    u32 f104;
    u8 pad108[0xc];
    u32 flags114;
    u8 pad118[0x1c];
    struct PhaseCollision collision134;
    u8 pad13c[0x4c];
    struct PhaseCollision collision188;
    u8 pad190[0x8c];
    void (*phase)(void);
    u8 pad220[0x90];
    u8 phaseState;
    u8 pad2b1;
    u8 entered;
    u8 pad2b3[2];
    u8 phaseId;
    u8 pad2b6[0xa];
    u16 flags2c0;
    u8 pad2c2[6];
    u16 timer;
    u8 pad2ca[0x106];
    u8 *data;
};
static inline u8 consumeEntry(struct PhaseActor *s)
{
    if (s->entered) {
        s->entered = 0;
        return 1;
    }
    return 0;
}
static inline void clearFlags(u32 *p, u32 mask)
{
    *p &= ~mask;
}

void Sound_PlaySong(u32);
void ActorShadow_Enable(void *);
static inline void clearCollision(struct PhaseCollision *p, u32 mask)
{
    p->flags &= ~mask;
}
static inline void clearFlags16(u16 *p, u32 mask)
{
    *p &= ~mask;
}
void ActorPhase_UpdateSpriteRestore(struct PhaseActor *s)
{
    u8 *data = s->data;
    if (consumeEntry(s)) {
        s->sprite->flags = (s->sprite->flags | 2) & ~1;
        s->sprite->field6 = 0;
        clearCollision(&s->collision188, 4);
        clearCollision(&s->collision134, 4);
        Sound_PlaySong(0x16c);
    }
    if (s->sprite->scaleY > 112) {
        s->sprite->scaleY -= 8;
    } else if (s->sprite->scaleY > 64) {
        s->sprite->scaleX += 2;
        s->sprite->scaleY -= 4;
        ActorShadow_Enable((u8 *)s + 0x320);
    } else if (s->sprite->scaleX <= 63) {
        s->sprite->scaleX += 4;
    } else {
        s->sprite->scaleX = 64;
        s->sprite->scaleY = 64;
        clearFlags((u32 *)(data + 0xe4c), 0x44);
        s->phaseState = 1;
        clearFlags16(&s->flags2c0, 0x1800);
    }
}
