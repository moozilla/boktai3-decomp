#include "scene_panel.h"

// MATCHING_CONCESSION: register-pinning

void ScenePanel_UpdateBackgroundEffect(struct ScenePanel *panel, s32 tileX, s32 tileY, s32 screenX,
                                       s32 screenY)
{
    if (gGameState->unk618 > 0) {
        register u16 frame asm("r5") = panel->mode;
        u32 shifted = (u32)frame << 16;
        u32 value;
        s32 alpha;
        value = shifted >> 16;
        if (value == 0)
            goto hide;
        {
            u32 mask = 0x200;
            u16 *shadow = &gDispCntShadow;
            *shadow = mask | *shadow;
        }
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
            alpha = shifted >> 17;
            if (!(frame & 3) && panel->shakeOffset > 0)
                panel->shakeOffset--;
        }
        {
            u32 blend = ((16 - alpha) >> 1) + 8;
            *(vu16 *)0x04000052 = (blend << 8) | alpha;
        }
        BgGfx_LoadTilemapSimple(1, tileX, tileY, screenX, screenY + panel->shakeOffset);
        panel->mode--;
    } else {
        CpuSet((u8 *)panel->palette + 32, (void *)0x03005140, 0x04000008);
        if (panel->effectPhase == 0) {
            s32 count;
            s32 alpha;
            {
                u32 mask = 0x200;
                u16 *shadow = &gDispCntShadow;
                *shadow = mask | *shadow;
            }
            if (++panel->mode > 63)
                panel->mode = 0;
            if (panel->mode > 32)
                count = (64 - panel->mode) >> 3;
            else
                count = panel->mode >> 3;
            alpha = count + 12;
            {
                u32 blend = ((16 - alpha) >> 1) + 8;
                *(vu16 *)0x04000052 = (blend << 8) | alpha;
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
                union ScenePanelPackedVector {
                    struct ContactPackedVector packed;
                    struct {
                        u32 xy, zpad;
                    } words;
                } position, velocity;
                const u16 *table;
                register u32 indexMask asm("r8");
                u32 highMask;
                register union ScenePanelPackedVector *positionWords asm("r9");
                u32 next;
                position.packed = (*(struct ContactPackedVector *)&panel->center);
                {
                    register const u16 *load asm("r2") = (const u16 *)RANDOM_TABLE_ADDR;
                    asm("" : "+r"(load));
                    table = load;
                }
                {
                    u32 nextIndex = gRandomIndex + 1;
                    register u32 lowMask asm("r3") = 1023;
                    indexMask = lowMask;
                    gRandomIndex = nextIndex & lowMask;
                }
                {
                    s32 offset = Mod(table[gRandomIndex], 500);
                    register s32 coordinate asm("r1");
                    {
                        union ScenePanelPackedVector *lowPosition = &position;
                        asm("" : "+r"(lowPosition));
                        positionWords = lowPosition;
                        coordinate = (u16)lowPosition->words.xy - 250;
                    }
                    coordinate += offset;
                    coordinate = (u16)coordinate;
                    highMask = 0xFFFF0000;
                    position.words.xy = (position.words.xy & highMask) | coordinate;
                }
                gRandomIndex = (gRandomIndex + 1) & indexMask;
                {
                    s32 offset = Mod(table[gRandomIndex], 500);
                    s32 coordinate = ((s32)position.words.xy >> 16) - 250;
                    coordinate += offset;
                    coordinate = (u32)coordinate << 16;
                    {
                        u32 previous = position.words.xy;
                        u32 lowMask = 0xFFFF;
                        lowMask &= previous;
                        position.words.xy = lowMask | coordinate;
                    }
                }
                gRandomIndex = (gRandomIndex + 1) & indexMask;
                {
                    s32 offset = Mod(table[gRandomIndex], 500);
                    register union ScenePanelPackedVector *lowPosition asm("r2") = positionWords;
                    register s32 coordinate asm("r1");
                    coordinate = (u16)lowPosition->words.zpad - 250;
                    coordinate += offset;
                    coordinate = (u16)coordinate;
                    lowPosition->words.zpad = (lowPosition->words.zpad & highMask) | coordinate;
                }
                velocity.words.xy &= highMask;
                {
                    u32 mask;
                    next = gRandomIndex + 1;
                    mask = indexMask;
                    next &= mask;
                    {
                        u32 sample = table[next];
                        sample &= 31;
                        sample += 16;
                        velocity.words.xy = sample << 16;
                    }
                    velocity.words.zpad &= highMask;
                    gRandomIndex = (next + 1) & mask;
                }
                {
                    u32 life = table[gRandomIndex];
                    life &= 7;
                    life += 8;
                    ScenePanel_AddParticle(panel, (union ScenePanelPosition *)positionWords,
                                           (union ScenePanelPosition *)&velocity, life, 1);
                }
            }
            {
                u32 mask = 0x200;
                u16 *shadow = &gDispCntShadow;
                *shadow = mask | *shadow;
            }
            {
                u16 *lowFrame = frame;
                register u32 frameValue asm("r0");
                register s32 alpha asm("r2");
                frameValue = *lowFrame;
                alpha = frameValue >> 2;
                {
                    u32 blend = ((16 - alpha) >> 1) + 8;
                    *(vu16 *)0x04000052 = (blend << 8) | alpha;
                }
            }
            {
                u16 *conditionFrame = frame;
                if (!(*conditionFrame & 7)) {
                    u32 shakeOffset = 0x132A;
                    s16 *shake;
                    shake = (void *)((u8 *)panel + shakeOffset);
                    if (*shake <= 6)
                        (*shake)++;
                }
            }
            BgGfx_LoadTilemapSimple(1, tileX, tileY, screenX, screenY + panel->shakeOffset);
            if (--panel->mode == 0)
                ScenePanel_CallPendingScript(gScenePanel, 9);
        }
    }
    return;
hide: {
    u16 *shadow = &gDispCntShadow;
    u32 mask = ~0x200;
    mask &= *shadow;
    *shadow = mask;
}
}
