#include "global.h"
struct EnemyF426 {
    u8 unk0[0x18];
    s32 timer;
    u8 unk1C[0x134 - 0x1C];
    s16 period;
    u8 unk136[0x148 - 0x136];
    void *pattern;
    u8 unk14C[0x184 - 0x14C];
    s16 patternCount;
    s16 patternIndex;
    s16 intervals[1];
};
void EnemyF426_SpawnProjectile(struct EnemyF426 *);
s32 Mod(s32, s32);
// confidence=low: fires a projectile either on the pattern's next interval (then advances and
// wraps the pattern) or every period frames.
s32 sub_08203CA0(struct EnemyF426 *p)
{
    if (p->pattern) {
        s16 *index = &p->patternIndex;
        if (p->timer > p->intervals[*index]) {
            EnemyF426_SpawnProjectile(p);
            p->timer = 0;
            (*index)++;
            if (*index > p->patternCount - 1)
                *index = 0;
        }
    } else if (Mod(p->timer, p->period) == 0)
        EnemyF426_SpawnProjectile(p);
    return 0;
}
