// MATCHING_CONCESSION: register-pinning
#include "scene_panel_rig.h"
static inline s32 scale12(s32 v)
{
    return v >= 0 ? v >> 12 : -((-v) >> 12);
}
static inline s32 scale3(s32 v)
{
    return v >= 0 ? v >> 3 : -((-v) >> 3);
}
static inline s32 scale5(s32 v)
{
    return v >= 0 ? v >> 5 : -((-v) >> 5);
}
#define ScenePanelRig_TickSecondarySprites sub_0805A7E4
void ScenePanelRig_TickSecondarySprites(struct ScenePanelRig *rig, u32 palette)
{
    s32 i = 0;
    register u32 byteMask asm("r10") = 255;
    s32 amplitude = 24;
    for (;;) {
        u32 product = sizeof(struct SceneRigSprite);
        register s32 index asm("r4");
        struct SceneRigSprite *sprite;
        struct SceneRigSprite *output;
        struct ContactPackedVector base;
        s32 near;
        s32 far;
        s32 lift;
        s32 radius;
        index = i;
        product *= index;
        {
            register u32 arrayOffset asm("r5") = (u32) & ((struct ScenePanelRig *)0)->secondary;
            product += arrayOffset;
        }
        sprite = (void *)((u8 *)rig + product);
        output = sprite;
        near = index;
        asm("" : "+r"(near));
        near += 2;
        {
            s32 total = 8;
            far = total - near;
        }
        {
            s32 difference = rig->position.y;
            difference -= rig->origin.y;
            difference *= far;
            lift = scale5(3 * difference);
        }
        base.x = scale3(rig->position.x * near + rig->origin.x * far) + lift;
        base.y = scale3(rig->position.y * near + rig->origin.y * far);
        base.z = scale3(rig->position.z * near + rig->origin.z * far) + lift;
        {
            register const s16 *wave asm("r2") = gUnk_086149C4;
            register u16 *randomPhase asm("r3") = &sprite->randomPhase;
            u16 *phase;
            {
                register u32 sample asm("r1") = *randomPhase;
                u32 angle = byteMask;
                angle &= sample;
                angle += 64;
                {
                    u32 mask = byteMask;
                    angle &= mask;
                }
                radius = scale12(wave[angle] * amplitude);
            }
            {
                s32 horizontal;
                {
                    register u16 *anglePtr asm("r0") = &sprite->phase;
                    s32 product = *anglePtr + 64;
                    {
                        u32 mask = byteMask;
                        product &= mask;
                    }
                    product = wave[product] * radius;
                    phase = anglePtr;
                    horizontal = product;
                }
                {
                    u16 x = base.x + scale12(horizontal);
                    struct SceneRigSprite *store = output;
                    store->position.x = x;
                }
            }
            output->position.y = base.y;
            {
                register u32 sample asm("r1") = *phase;
                u32 angle = byteMask;
                s32 offset;
                angle &= sample;
                offset = scale12(wave[angle] * radius);
                {
                    u16 z = base.z + offset;
                    struct SceneRigSprite *store = output;
                    store->position.z = z;
                }
            }
            {
                u32 angle = *phase + 3;
                u32 mask;
                mask = byteMask;
                *phase = angle & mask;
            }
            *randomPhase += 5;
        }
        {
            struct DisplaySequenceState *state = &sprite->sequence;
            const volatile u16 *command;
            u32 duration, flags, bit;
            {
                u32 offset = state->index << 1;
                struct SceneRigSprite *frameSprite = sprite;
                register const volatile u16 *table asm("r1");
                table = frameSprite->sequence.commands;
                command = (const volatile u16 *)((u32)table + offset);
                frameSprite->frame = *command >> 6;
            }
            {
                u32 input = state->flags;
                u32 first = 1;
                u32 encoded;
                u32 value;
                first &= input;
                value = *command;
                encoded = (value & 0x30) >> 4;
                {
                    u32 one = 1;
                    encoded &= one;
                }
                {
                    register u32 flags asm("r0");
                    struct SceneRigSprite *out;
                    if (first != encoded) {
                        out = sprite;
                        asm("" : "+r"(out));
                        flags = out->flags | 4;
                    } else {
                        register struct SceneRigSprite *read asm("r1") = sprite;
                        flags = read->flags & ~4;
                        out = sprite;
                    }
                    out->flags = flags;
                }
            }
            {
                u32 input = state->flags;
                register u32 bit asm("r3");
                register u32 first asm("r1");
                bit = 2;
                first = bit;
                first &= input;
                first = (u8)first;
                if (first != (((*command & 0x30) >> 4) & bit)) {
                    register struct SceneRigSprite *out asm("r5") = sprite;
                    out->flags |= 8;
                } else {
                    register struct SceneRigSprite *read asm("r1") = sprite;
                    register u32 flags asm("r0");
                    register struct SceneRigSprite *out asm("r2");
                    asm("" : "+r"(read));
                    flags = read->flags & ~8;
                    out = sprite;
                    out->flags = flags;
                }
            }
            {
                u32 elapsed = state->elapsed + 1;
                u32 mask;
                register u32 temp asm("r2");
                register u32 savedDuration asm("r2");
                state->elapsed = elapsed;
                temp = 0xFFFF;
                asm("" : "+r"(temp));
                mask = temp;
                duration = state->duration;
                elapsed = (u16)elapsed;
                savedDuration = duration;
                if (elapsed >= duration) {
                    state->elapsed = 0;
                    flags = state->flags;
                    if (flags & 4) {
                        u32 index = state->index;
                        if (index == 0) {
                            u32 hold = 8;
                            hold &= flags;
                            if (hold) {
                                state->elapsed = savedDuration;
                            } else {
                                state->index = state->frameCount - 1;
                            }
                        } else {
                            state->index = index - 1;
                        }
                    } else {
                        u32 index = state->index + 1;
                        u32 frameCount;
                        u32 endFlag;
                        state->index = index;
                        frameCount = state->frameCount;
                        if ((index & mask) >= frameCount) {
                            endFlag = flags & 8;
                            asm("" : "+r"(endFlag));
                            endFlag = (u8)endFlag;
                            if (endFlag) {
                                state->elapsed = duration;
                                {
                                    u32 lastIndex = frameCount - 1;
                                    state->index = lastIndex;
                                }
                            } else {
                                state->index = 0;
                            }
                        } else {
                        }
                    }
                    {
                        u32 offset = state->index << 1;
                        const volatile u16 *table;
                        table = state->commands;
                        command = (const volatile u16 *)((u32)table + offset);
                    }
                    state->frameWeight = *command & 15;
                    {
                        s32 weight = state->frameWeight;
                        s32 speed;
                        speed = state->speed;
                        speed *= weight;
                        speed >>= 6;
                        state->duration = speed;
                        {
                            register u32 mask asm("r5") = byteMask;
                            speed &= mask;
                            if (!speed)
                                state->duration = 1;
                        }
                    }
                }
            }
        }
        AnimData_SelectPaletteBank(sprite->graphics, palette);
        {
            register s32 next asm("r1") = amplitude;
            next += 4;
            amplitude = next;
            asm("" : : "r"(next));
        }
        {
            register s32 next asm("r2") = i;
            next++;
            i = next;
            if (next > 5)
                break;
        }
    }
}
