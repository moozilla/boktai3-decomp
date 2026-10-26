// MATCHING_CONCESSION: register-pinning
#include "sprite_effect_controller.h"
#include "engine.h"
struct SequenceDrawable {
    u32 flags;
    u8 active, unknown5[2], priority;
    u8 unknown8[4];
    void *graphics;
    u16 frame;
    u8 unknown12[0xa];
    struct ContactPosition position;
    u8 unknown24[8];
    u8 resource[0x1c];
};
struct TileDrawable {
    u32 flags;
    u8 active;
    u8 unknown5[3];
    u16 tile;
    u8 width, height;
    u8 biasX, biasY, unknownE, priority;
    u8 unknown10[8];
    struct ContactPosition position;
    u8 unknown20[8];
    void *graphics;
};
struct SpriteEffectControllerPool *SpriteEffectController_EnsurePool(void);
void *SpriteEffectController_LoadSpriteResources(struct SpriteEffectController *, s32, u32);
void ParticleSprite_Initialize(void *, void *, s32);
void Effect_SetOrigin(void *, s32, s32);
void Effect_SetPalette(void *, s32);
extern u32 gUnk_030053F0, gUnk_02000560;
static inline void Step(struct SpriteEffectController *owner, struct SequenceDrawable *inputSprite,
                        struct DisplaySequenceState *inputState, u32 inputOne)
{
    register struct SequenceDrawable *slot asm("r5") = inputSprite;
    register u32 one asm("r10") = inputOne;
    struct DisplaySequenceState *state;
    const volatile u16 *command;
    u32 ended, duration, flags, bit;
    state = inputState;
    command = (const volatile u16 *)((u32)owner->sequence.commands + (state->index << 1));
    slot->frame = *command >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        first = one;
        first &= input;
        {
            u32 bits = *command;
            u32 flip = (bits & 0x30) >> 4;
            u32 mask;
            mask = one;
            flip &= mask;
            if (first != flip) {
                u32 value = slot->flags;
                register u32 mask asm("r3");
                mask = 4;
                value |= mask;
                slot->flags = value;
            } else
                slot->flags &= ~4;
        }
    }
    {
        u32 input = state->flags;
        u32 bit;
        register u32 first asm("r1");
        bit = 2;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        asm("" : "+r"(first));
        first = (u8)first;
        if (first != (((*command & 0x30) >> 4) & bit))
            slot->flags |= 8;
        else
            slot->flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 mask;
        register u32 temp asm("r1");
        register u32 savedDuration asm("r3");
        state->elapsed = elapsed;
        temp = 0xFFFF;
        asm("" : "+r"(temp));
        mask = temp;
        asm("" : "+&r"(mask) : "r"(temp));
        /* Keep the live mask distinct from its source without binding r7. */
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
                    ended = 1;
                } else {
                    state->index = index - 1;
                    ended = 0;
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
                    ended = 1;
                } else {
                    ended = 0;
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
                register s32 weight asm("r1") = state->frameWeight;
                s32 speed;
                speed = state->speed;
                speed = (speed * weight) >> 6;
                state->duration = speed;
                if (!(u8)speed)
                    state->duration = 1;
            }
        } else {
            ended = 0;
        }
    }
    return;
}
struct SpriteEffectController *SpriteEffectController_Create(
    const struct ContactPosition *positionInput, const struct ContactPosition *velocityInput,
    s32 graphicsInput, u32 contextInput, u32 sequence, u32 drawableFlags, u32 flags, u32 palette,
    u32 priority, u32 variant, u32 animationFlags, u32 lifetime, s32 contactA, s32 contactB,
    s32 contactC, s32 contactD, s32 biasX, s32 biasY)
{
    const struct ContactPosition *position = positionInput;
    const struct ContactPosition *velocity = velocityInput;
    struct SpriteEffectControllerPool *p;
    struct SpriteEffectController *slot;
    /* Keep the two callback/resource argument homes while register hints
     * remain; removing either volatile currently changes low-register reuse. */
    volatile s32 graphicsId;
    volatile u32 context;
    register u16 *slotFlags asm("r9");
    u8 *state;
    u32 one;
    graphicsId = graphicsInput;
    context = contextInput;
    p = gSpriteEffectControllerPool;
    if (!p) {
        p = SpriteEffectController_EnsurePool();
        if (!p)
            return 0;
    }
    {
        u32 mask = 0x400;
        if ((gEngineFlags | gUnk_030053F0) & mask)
            return 0;
    }
    if (gUnk_02000560 & 4)
        return 0;
    slot = SpriteEffectController_AllocateSlot(p);
    if (!slot)
        return 0;
    {
        u16 *earlyFlags = &slot->flags;
        *earlyFlags = flags;
        {
            u8 *enter = &slot->enter;
            register u32 source asm("r3");
            register u32 lowOne asm("r1");
            source = 1;
            asm("" : "+r"(source));
            one = source;
            lowOne = one;
            *enter = lowOne;
            {
                u32 offset = 0x72;
                register u8 *lowState asm("r3");
                asm("" : "+r"(offset));
                lowState = (void *)(offset + (u32)slot);
                state = lowState;
                *lowState = lowOne;
            }
        }
        slot->velocity = *velocity;
        {
            register s32 value asm("r0") = velocity->x;
            u32 sum;
            register u32 product asm("r3");
            sum = value;
            sum *= value;
            value = velocity->y;
            product = value;
            product *= value;
            value = product;
            sum += value;
            value = velocity->z;
            product = value;
            product *= value;
            asm("" : "+r"(product));
            value = product;
            asm("" : "+r"(value));
            slotFlags = earlyFlags;
            if (sum + (u32)value)
                *earlyFlags |= 0x80;
        }
    }
    slot->lifetime = lifetime;
    {
        register u32 mode asm("r1") = *slotFlags;
        if (mode & 8) {
            register struct SequenceDrawable *sprite asm("r5");
            void *resource;
            void **cursor;
            slot->drawable = Heap_Alloc(0x48);
            Memory_Clear(slot->drawable, 0x48);
            sprite = slot->drawable;
            sprite->position = *position;
            {
                struct SpriteEffectController *arg = slot;
                s32 graphics;
                u32 resourceContext;
                asm("" : "+r"(arg));
                graphics = graphicsId;
                resourceContext = context;
                resource =
                    SpriteEffectController_LoadSpriteResources(arg, graphics, resourceContext);
            }
            cursor = (void *)slot;
            {
                void *drawable = *cursor++;
                Sprite_Init(sprite, (u8 *)drawable + 0x2c, 0);
            }
            {
                register u32 full asm("r1") = sequence;
                u32 narrowSequence;
                u32 row;
                u32 flagsByte;
                narrowSequence = (u16)full;
                asm("" : "+r"(narrowSequence) : "r"(full));
                {
                    u32 source = variant;
                    row = (u8)source;
                }
                full = animationFlags;
                flagsByte = (u8)full;
                Anim_SetSequence((void *)cursor, resource, narrowSequence, row, flagsByte);
            }
            AnimData_SelectPaletteBank(sprite->graphics, palette);
            Step(slot, sprite, (void *)cursor, one);
            {
                u32 value = sprite->flags;
                register u32 extra asm("r2");
                value |= 1;
                extra = drawableFlags;
                asm("" : "+r"(extra));
                value |= extra;
                sprite->flags = value;
            }
            {
                register const u8 *field asm("r3") = (void *)&priority;
                register u8 byte asm("r3");
                byte = *field;
                asm("" : "+r"(byte));
                sprite->priority = byte;
            }
        } else if (mode & 0x10) {
            struct TileDrawable *sprite;
            void *resource;
            slot->drawable = Heap_Alloc(0x2c);
            Memory_Clear(slot->drawable, 0x2c);
            sprite = slot->drawable;
            sprite->position = *position;
            {
                struct TileDrawable *graphicsOwner = slot->drawable;
                register u32 graphics asm("r1");
                u32 arg;
                graphics = graphicsId;
                arg = (u16)graphics;
                graphicsOwner->graphics = AnimData_FindEntry(arg);
            }
            {
                u32 key = 0x922e;
                register u32 resourceContext asm("r2");
                resourceContext = context;
                resource = ScriptProc_Start(key, resourceContext);
            }
            ParticleSprite_Initialize(sprite, ((struct TileDrawable *)slot->drawable)->graphics, 0);
            Effect_SetOrigin(sprite, 0U - (u32)biasX, 0U - (u32)biasY);
            {
                u32 full = sequence;
                u32 narrowSequence;
                u32 row;
                u32 flagsByte;
                narrowSequence = (u16)full;
                {
                    u32 source = variant;
                    row = (u8)source;
                }
                {
                    u32 source = animationFlags;
                    flagsByte = (u8)source;
                }
                Anim_SetSequence(&slot->sequence, resource, narrowSequence, row, flagsByte);
            }
            Effect_SetPalette(sprite, palette);
            {
                u32 value = sprite->flags;
                u32 lowOne;
                u32 extra;
                lowOne = one;
                value |= lowOne;
                extra = drawableFlags;
                value |= extra;
                sprite->flags = value;
            }
            {
                const u8 *field = (void *)&priority;
                u8 byte;
                byte = *field;
                sprite->priority = byte;
            }
            {
                register u16 *read asm("r2") = slotFlags;
                register u32 bits asm("r1");
                bits = *read;
                if (bits & 0x20) {
                    u32 two = 2;
                    register u8 *write asm("r3");
                    write = state;
                    asm("" : "+r"(write));
                    *write = two;
                } else if (bits & 0x200) {
                    u32 three = 3;
                    register u8 *write asm("r1");
                    write = state;
                    asm("" : "+r"(write));
                    *write = three;
                }
            }
        }
    }
    {
        register s32 a asm("r2") = contactA;
        if (a > 0) {
            SpriteEffectController_InitCollider(slot, a, contactB, contactC, contactD);
            {
                register u16 *write asm("r3") = slotFlags;
                u32 bits;
                bits = *write;
                {
                    u32 mask = 4;
                    mask |= bits;
                    *write = mask;
                }
            }
        }
    }
    return slot;
}
