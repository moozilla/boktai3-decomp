#define EnemyD459_UpdateAttackTiming     sub_08204D84
#define EnemyD459_UpdateProjectileAttack sub_08204E5C
#include "global.h"

struct EnemyD459Vector {
    s16 x, y, z, pad;
};
struct EnemyD459 {
    u8 pad0[0x18];
    s32 timer;
    u8 pad1c[0x40 - 0x1c];
    struct EnemyD459Vector position;
    u8 pad48[0x108 - 0x48];
    u32 (*callback)(struct EnemyD459 *);
    u8 animation[0x20];
    u16 attackDuration;
    u8 pad12e[4];
    u8 facing;
    u8 pad133;
    s16 attackPeriod;
    s16 movement;
    u8 pad138[0x14c - 0x138];
    u32 useAttackSchedule;
    u8 pad150[8];
    void *animationResource;
    u8 pad15c[0x184 - 0x15c];
    s16 attackCount;
    s16 attackIndex;
    s16 attackTimes[1];
};
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void Anim_SetDurationScale(void *, u32);
s32 Mod(s32, s32);
u32 sub_08204E5C(struct EnemyD459 *);

u32 sub_08204D84(struct EnemyD459 *enemy)
{
    if (enemy->useAttackSchedule) {
        s16 *index = &enemy->attackIndex;
        s32 selected = *index * 2;
        s16 *table = enemy->attackTimes;
        if (enemy->timer > *(s16 *)((u8 *)table + selected)) {
            void *anim = enemy->animation;
            void *resource = enemy->animationResource;
            u32 facing = enemy->facing;
            u32 zero = 0;
            Anim_RestartSequence(anim, resource, 3, facing, zero);
            Anim_SetDurationScale(anim, 64);
            enemy->callback = sub_08204E5C;
            enemy->attackDuration = 80;
            enemy->timer = zero;
            (*index)++;
            if (*index > enemy->attackCount - 1)
                *index = zero;
        }
    } else {
        s32 value = Mod(enemy->timer, enemy->attackPeriod);
        if (!value) {
            Anim_RestartSequence(enemy->animation, enemy->animationResource, 3, enemy->facing,
                                 value);
            enemy->callback = sub_08204E5C;
        }
    }
    return 0;
}
