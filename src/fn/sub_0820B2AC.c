#include "global.h"

struct DragonPuppyPosition {
    s16 x, y, z, unk_06;
};

struct DragonPuppy {
    u8 unk_00[0x18];
    s32 timer;
    struct DragonPuppyPosition position;
    u8 unk_24[0x1c];
    struct DragonPuppyPosition displayPosition;
    u8 unk_48[0xc0];
    void (*update)(void);
    u8 animation[0x1a];
    s16 recoveryDelay;
    u8 unk_128[6];
    s16 delay;
    u8 unk_130[2];
    u8 animationRow;
    u8 unk_133[5];
    s16 attackDelay;
    s16 attackTime;
    u8 unk_13c[0xe];
    s16 disabled;
    u8 unk_14c[0x24];
    void *animationData;
    u8 unk_174[0x12];
    s16 attackIndex;
};

extern const u16 gUnk_0203B400[];
extern u32 gUnk_03005308;
void DragonPuppy_HandleDefeat(struct DragonPuppy *);
void DragonPuppy_UpdateAttackTiming(void);
void Anim_SetSequence(void *, void *, u32, u32, u32);

static inline u32 NextRandom(void)
{
    const u16 *table = gUnk_0203B400;
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    return table[gUnk_03005308];
}

u32 DragonPuppy_UpdateAttackWindup(struct DragonPuppy *p)
{
    if (p->disabled == 0) {
        if (p->attackTime < 0 && p->timer == 1)
            DragonPuppy_HandleDefeat(p);
        if (p->timer > p->attackTime - 60 && p->attackTime > 0) {
            p->displayPosition.x = (NextRandom() & 7) + p->position.x - 4;
            p->displayPosition.y = (NextRandom() & 7) + p->position.y - 4;
            p->displayPosition.z = (NextRandom() & 7) + p->position.z - 4;
            if (p->timer > p->attackTime && p->attackTime > 0) {
                p->displayPosition = p->position;
                Anim_SetSequence(p->animation, p->animationData, 3, p->animationRow, 0);
                p->update = DragonPuppy_UpdateAttackTiming;
                p->timer = 0;
                p->recoveryDelay = p->attackDelay;
                p->attackIndex = 0;
                p->delay = 0;
            }
        }
    }
}
