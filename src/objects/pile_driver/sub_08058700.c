#include "objects/pile_driver/pile_driver.h"
#include "objects/pile_driver/pile_driver_effects.h"
#include "gba/io_reg.h"
#include "gba/syscall.h"

void PileDriver_UpdateBackgroundEffect(struct PileDriver *panel, s32 tileX, s32 tileY, s32 screenX,
                                       s32 screenY)
{
    s32 effectAlpha;
    if (gGameState->unk618 > 0) {
        u32 frame = panel->mode;
        u32 shifted = (u32)frame << 16;
        u32 value;
        s32 alpha;
        value = shifted >> 16;
        if (value == 0)
            goto hide;
        Bg_EnableDisplay(DISPCNT_BG1_ON);
        if (value > 31) {
            alpha = 16;
            if (value > 35) {
                gRandomIndex = (gRandomIndex + 1) & 1023;
                {
                    u32 offset = gRandomIndex * 2;
                    s32 sample = *(u16 *)(offset + RANDOM_TABLE_ADDR);
                    panel->shakeOffset = (sample >> 3) & 7;
                }
            } else
                panel->shakeOffset = 5;
        } else {
            u32 mask;
            alpha = shifted >> 17;
            mask = 3;
            mask &= frame;
            if (!mask && panel->shakeOffset > 0)
                panel->shakeOffset--;
        }
        {
            u32 blend = ((16 - alpha) >> 1) + 8;
            REG_BLDALPHA = (blend << 8) | alpha;
        }
        BgGfx_LoadTilemapSimple(1, tileX, tileY, screenX, screenY + panel->shakeOffset);
        panel->mode--;
    } else {
        CpuCopy32((u16 *)panel->palette + 16, gBgPaletteBank12, 32);
        if (panel->effectPhase == 0) {
            s32 count;

            Bg_EnableDisplay(DISPCNT_BG1_ON);
            if (++panel->mode > 63)
                panel->mode = 0;
            if (panel->mode > 32)
                count = (64 - panel->mode) >> 3;
            else
                count = panel->mode >> 3;
            effectAlpha = count + 12;
            {
                u32 blend = ((16 - effectAlpha) >> 1) + 8;
                REG_BLDALPHA = (blend << 8) | effectAlpha;
            }
            gRandomIndex = (gRandomIndex + 1) & 1023;
            {
                u32 offset = gRandomIndex * 2;
                s32 sample = *(u16 *)(offset + RANDOM_TABLE_ADDR);
                panel->shakeOffset = (sample >> 3) & 7;
            }
            BgGfx_LoadTilemapSimple(1, tileX, tileY, screenX, screenY + panel->shakeOffset);
            {
                s32 zero = panel->mode & 3;
                if (zero == 0) {
                    struct ContactPackedVector position =
                                                   (*(struct ContactPackedVector *)&panel->center),
                                               velocity;
                    position.x += 64;
                    position.z += 64;
                    velocity.x = 300;
                    velocity.y = 300;
                    velocity.z = 300;
                    SparkParticle_Spawn(&position, 1, &velocity, 0, zero, 48, 2, 1);
                }
            }
        } else {
            u16 *frame = &panel->mode;
            if (*frame == 0)
                goto hide;
            {
                union PileDriverPosition position, velocity;
                const u16 *table;
                u32 highMask;
                union PileDriverPosition *positionWords;
                u32 next;
                position.world = *(struct SceneVec *)&panel->center;
                table = gRandomTable;
                gRandomIndex = (gRandomIndex + 1) & 1023;
                {
                    s32 offset = Mod(table[gRandomIndex], 500);
                    s32 coordinate;
                    {
                        union PileDriverPosition *lowPosition = &position;
                        positionWords = lowPosition;
                        coordinate = (u16)lowPosition->world.x - 250;
                    }
                    coordinate += offset;
                    {
                        u16 narrowed = coordinate;
                        coordinate = narrowed;
                    }
                    highMask = 0xFFFF0000;
                    position.words[0] = (position.words[0] & highMask) | coordinate;
                }
                gRandomIndex = (gRandomIndex + 1) & 1023;
                {
                    s32 offset = Mod(table[gRandomIndex], 500);
                    s32 coordinate = ((s32)position.words[0] >> 16) - 250;
                    coordinate += offset;
                    coordinate = (u32)coordinate << 16;
                    {
                        u32 previous = position.words[0];
                        u32 lowMask = 0xFFFF;
                        lowMask &= previous;
                        position.words[0] = lowMask | coordinate;
                    }
                }
                gRandomIndex = (gRandomIndex + 1) & 1023;
                {
                    s32 offset = Mod(table[gRandomIndex], 500);
                    union PileDriverPosition *lowPosition = positionWords;
                    s32 coordinate;
                    coordinate = (u16)lowPosition->world.z - 250;
                    coordinate += offset;
                    {
                        u16 narrowed = coordinate;
                        coordinate = narrowed;
                    }
                    lowPosition->words[1] = (lowPosition->words[1] & highMask) | coordinate;
                }
                velocity.words[0] &= highMask;
                {
                    next = gRandomIndex + 1;
                    next &= 1023;
                    {
                        u32 sample = table[next];
                        sample &= 31;
                        sample += 16;
                        velocity.words[0] = sample << 16;
                    }
                    velocity.world.z = 0;
                    gRandomIndex = (next + 1) & 1023;
                }
                {
                    u32 life = table[gRandomIndex];
                    life &= 7;
                    life += 8;
                    PileDriver_AddParticle(panel, positionWords, &velocity, life, 1);
                }
            }
            Bg_EnableDisplay(DISPCNT_BG1_ON);
            {
                u16 *lowFrame = frame;
                u32 frameValue;
                frameValue = *lowFrame;
                effectAlpha = frameValue >> 2;
                {
                    u32 blend = ((16 - effectAlpha) >> 1) + 8;
                    REG_BLDALPHA = (blend << 8) | effectAlpha;
                }
            }
            {
                u16 *conditionFrame = frame;
                if (!(*conditionFrame & 7)) {
                    s16 *shake = &panel->shakeOffset;
                    if (*shake <= 6)
                        (*shake)++;
                }
            }
            BgGfx_LoadTilemapSimple(1, tileX, tileY, screenX, screenY + panel->shakeOffset);
            if (--panel->mode == 0)
                PileDriver_CallPendingScript(gPileDriver, 9);
        }
    }
    return;
hide: {
    u16 *shadow = &gDispCntShadow;
    u32 mask = ~DISPCNT_BG1_ON;
    mask &= *shadow;
    *shadow = mask;
}
}
