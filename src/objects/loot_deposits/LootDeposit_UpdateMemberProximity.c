#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "objects/loot_deposits/callback_pool.h"
#include "player/player.h"

extern const s16 gUnk_086149C4[];
static inline s32 Scale(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -((-x) >> 12);
}
static inline void Clear(void *p)
{
    u32 zero = 0;
    CpuSet(&zero, p, 0x05000002);
}
static inline s32 LootDeposit_ProjectComponent(s32 x)
{
    s32 result;
    if (x >= 0)
        result = x >> 12;
    else
        result = -((-x) >> 12);
    return result;
}

static inline u32 LootDeposit_TestPlayerFlag(const struct Player *player, u8 mask)
{
    return mask & player->statusFlags;
}
// confidence=low: starts a sway and advances the group member when the nearby player moves.
void LootDeposit_UpdateMemberProximity(void *unused, struct CallbackPoolEntry *entry)
{
    if (entry->defeated) {
        s32 x;
        s32 z;
        entry->frame = 0;
        entry->sprite.flags = (entry->sprite.flags & ~1) | 2;
        entry->defeated = 0;
        Clear(&entry->swayRadius);
        entry->swayRadius.x = 41;
        entry->swayRadius.z = 148;
        x = LootDeposit_ProjectComponent(entry->swayRadius.x *
                                         gUnk_086149C4[(entry->sprite.attribute + 64) & 255]) +
            Scale(entry->swayRadius.z * gUnk_086149C4[entry->sprite.attribute]);
        z = LootDeposit_ProjectComponent(
                -(entry->swayRadius.x * gUnk_086149C4[entry->sprite.attribute])) +
            Scale(entry->swayRadius.z * gUnk_086149C4[(entry->sprite.attribute + 64) & 255]);
        entry->sprite.position.x = entry->savedPosition.x + x;
        entry->sprite.position.z = z + entry->savedPosition.z;
    }
    if (entry->frame & 1) {
        if (LootDepositFeedback_IsPlayerInsideOriginRange(entry)) {
            if (!LootDeposit_TestPlayerFlag(gPlayers[0], 8)) {
                s32 active;
                if (gPlayerPassageState && !gPlayerInteractionMode)
                    active = 1;
                else
                    active = 0;
                if (!active && gPlayers[0]->actionMode != 3) {
                    if (entry->previousDistanceSquared != entry->distanceSquared &&
                        entry->motion.x == 0) {
                        entry->motion.x = 4;
                        entry->health--;
                        if (entry->health <= 0) {
                            entry->callbackIndex = 3;
                            entry->defeated = 1;
                            return;
                        }
                        Sound_PlaySong(0x3C9);
                    }
                }
            }
        }
    }
    LootDeposit_UpdateMemberSway(entry);
    entry->frame++;
}
