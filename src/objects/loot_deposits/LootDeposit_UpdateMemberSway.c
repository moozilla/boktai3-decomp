#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "objects/loot_deposits/callback_pool.h"

extern const s16 gUnk_086149C4[];
static inline s32 Scale(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -((-x) >> 12);
}
static inline s32 Half(s32 x)
{
    if (x >= 0)
        return x >> 1;
    return -((-x) >> 1);
}
static inline s32 ScaleR(s32 x)
{
    s32 result;
    if (x >= 0)
        result = x >> 12;
    else
        result = -((-x) >> 12);
    return result;
}
static inline s32 WrapAngle(s32 x)
{
    if (x > 128)
        x -= 256;
    else if (x < -128)
        x += 256;
    return x;
}
// confidence=low: rotates the member's offset about its origin and damps its turn step.
void LootDeposit_UpdateMemberSway(struct CallbackPoolEntry *entry)
{
    s32 angle = WrapAngle(entry->sprite.attribute);
    s32 x;
    s32 z;

    if (angle == 0 && entry->motion.x == 0)
        return;
    x = ScaleR(entry->swayRadius.x * gUnk_086149C4[(entry->sprite.attribute + 64) & 255]) +
        Scale(entry->swayRadius.z * gUnk_086149C4[entry->sprite.attribute]);
    z = ScaleR(-(entry->swayRadius.x * gUnk_086149C4[entry->sprite.attribute])) +
        Scale(entry->swayRadius.z * gUnk_086149C4[(entry->sprite.attribute + 64) & 255]);
    entry->sprite.position.x = entry->savedPosition.x + x;
    entry->sprite.position.z = z + entry->savedPosition.z;
    if (entry->motion.x == 0) {
        if (angle > 0)
            entry->sprite.attribute--;
        else
            entry->sprite.attribute++;
    } else {
        entry->sprite.attribute += entry->motion.x;
        if (entry->motion.x > 0) {
            if (angle > entry->motion.x)
                entry->motion.x *= -1;
        } else {
            if (angle < entry->motion.x) {
                entry->motion.x *= -1;
                entry->motion.x = Half(entry->motion.x);
            }
        }
    }
}
