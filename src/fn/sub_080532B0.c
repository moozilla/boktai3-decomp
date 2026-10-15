// MATCHING_CONCESSION: register-pinning
#include "stacked_actor.h"
#define StackedActor_Initialize sub_080532B0
struct ActorMapView {
    u8 unknown0[4];
    u8 *cells;
    u8 unknown8[0x1C];
    u16 rowOffsets[1];
};
extern struct ActorMapView *gUnk_030052F4;
extern u32 gUnk_030052F8, gUnk_030052FC;
extern u8 *gUnk_02000580;
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
u8 *sub_0821D64C(u32, u32);
void AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void AnimData_SelectTiles(void *, u32);
void sub_0805321C(struct StackedActor *);
s32 SceneMessages_RegisterReceiver(void *, s32, s32);
void Sound_PlaySong(u32);
void sub_08056C6C(u32);
void sub_081593A0(u8 *, struct StackedActorPosition *);
void sub_0815AEE0(u8 *);
void sub_08052640(struct StackedActor *);
void sub_08052938(struct StackedActor *);
void sub_08052E80(struct StackedActor *);
s32 sub_0804A0A4(void);
void sub_08049FFC(struct StackedActorPosition *);
s32 StackedActor_Initialize(struct StackedActor *actor, s32 tag, s32 unused)
{
    struct InitCache {
        s32 tag;
        void *graphics;
        struct StackedActorPosition *world;
        struct StackedActorSprite *segments;
        void *receiver;
        u16 *proximity;
    } cache;
    register u16 *baseFrame asm("r10");
    u32 present;
    u32 mode;

    cache.tag = tag;
    present = Script_SeekToKeyword('p');
    if (present) {
        s32 x, z;
        s32 index;
        register s16 *worldX asm("r4");
        register s16 *worldY asm("r5");
        u8 *cell;
        u32 type;
        s32 y;
        u32 value = Script_GetValue();
        worldX = &actor->world.x;
        *worldX = value;
        value = Script_GetValue();
        worldY = &actor->world.y;
        *worldY = value;
        actor->world.z = Script_GetValue();
        x = (s16) * (volatile u16 *)worldX >> 8;
        z = (s16) * (volatile u16 *)((u8 *)worldX + 4) >> 8;
        cache.world = (void *)worldX;
        if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
            index = 0;
        else {
            u32 row = gUnk_030052F4->rowOffsets[z];
            index = row + x;
        }
        cell = sub_0821D64C(index, 1);
        if (cell)
            cell += 4;
        else {
            struct ActorMapView *map = gUnk_030052F4;
            u32 offset = index * 4 + 12;
            cell = map->cells + offset;
        }
        type = *cell >> 4;
        y = (*cell & 15) << 8;
        switch (type) {
            case 1:
                y -= *((u8 *)worldX + 4);
                break;
            case 2:
                y -= *(u8 *)worldX;
                break;
        }
        *worldY = y;
    } else {
        actor->world.x = present;
        actor->world.y = present;
        actor->world.z = present;
        cache.world = &actor->world;
    }
    if (Script_SeekToKeyword('C'))
        actor->pendingScriptId = Script_GetValue();
    else
        actor->pendingScriptId = 0;
    if (Script_SeekToKeyword('c'))
        actor->acceptedScriptId = Script_GetValue();
    else
        actor->acceptedScriptId = 0;
    if (Script_SeekToKeyword('i'))
        actor->completedScriptId = Script_GetValue();
    else
        actor->completedScriptId = 0;
    if (Script_SeekToKeyword('o'))
        actor->particleScriptId = Script_GetValue();
    else
        actor->particleScriptId = 0;
    if (Script_SeekToKeyword('d'))
        actor->baseFrame = Script_GetValue();
    else
        actor->baseFrame = 6;
    baseFrame = &actor->baseFrame;
    if (Script_SeekToKeyword('a'))
        actor->interactionEnabled = Script_GetValue();
    else
        actor->interactionEnabled = 0;
    if (Script_SeekToKeyword('n'))
        actor->particleScriptValue = Script_GetValue();
    else
        actor->particleScriptValue = 0;
    if (Script_SeekToKeyword('P'))
        mode = Script_GetValue();
    else
        mode = 3;
    {
        register u16 *source asm("r1") = baseFrame;
        actor->tileBase = (*source >> 1) * 3 + 160;
    }
    {
        void *graphics = actor->graphics;
        u16 *tiles = &actor->tileBase;
        struct StackedActorSprite *sprite;
        void *shadowGraphics;
        struct StackedActorSprite *shadow;
        register struct StackedActorPosition *shadowPosition asm("r2");
        AnimData_Load(graphics, 0xBA8);
        sprite = &actor->sprite;
        Sprite_Init(sprite, graphics, 0);
        {
            register u16 *source asm("r1") = baseFrame;
            sprite->frame = *source;
        }
        AnimData_SelectTiles(graphics, *tiles);
        actor->sprite.flags |= 0x200;
        {
            u8 *destination = &actor->sprite.renderMode;
            register u32 renderMode asm("r2") = mode;
            *destination = renderMode;
        }
        {
            register struct StackedActorPosition *source asm("r3") = cache.world;
            actor->sprite.position = *source;
        }
        shadowGraphics = actor->shadowGraphics;
        AnimData_Load(shadowGraphics, 0xBA8);
        shadow = &actor->shadow;
        Sprite_Init(shadow, shadowGraphics, 0);
        {
            register u16 *source asm("r1") = baseFrame;
            shadow->frame = *source;
        }
        AnimData_SelectTiles(shadowGraphics, *tiles);
        shadow->flags |= 0x400;
        {
            u8 *destination = &actor->shadow.renderMode;
            register u32 renderMode asm("r2") = mode;
            *destination = renderMode;
        }
        shadowPosition = &actor->shadow.position;
        {
            register struct StackedActorPosition *source asm("r3") = cache.world;
            *shadowPosition = *source;
        }
        {
            s32 i = 0;
            register s32 count asm("r1");
            register u32 one asm("r5");
            struct StackedActorSprite *savedShadow;
            u8 *segmentGraphics;
            cache.graphics = graphics;
            savedShadow = shadow;
            cache.segments = (void *)((u8 *)shadowPosition + 0x2C);
            {
                register void *receiver asm("r3") = actor->trigger;
                u16 *state;
                cache.receiver = receiver;
                state = &actor->proximityState;
                cache.proximity = state;
            }
            {
                register u32 offset asm("r1") = 0x16C;
                asm("" : : "r"(offset));
                segmentGraphics = (void *)((u8 *)actor + offset);
            }
            do {
                register u32 offset asm("r4");
                struct StackedActorSprite *current;
                AnimData_Load(segmentGraphics, 0xBA8);
                {
                    u32 stride = sizeof(struct StackedActorSprite);
                    offset = i;
                    offset *= stride;
                }
                {
                    u32 currentOffset = offset;
                    currentOffset += 0xE8;
                    current = (void *)((u8 *)actor + currentOffset);
                }
                Sprite_Init(current, segmentGraphics, 0);
                {
                    register u16 *source asm("r2") = baseFrame;
                    u32 frame = *source;
                    register u32 selected asm("r1");
                    one = 1;
                    selected = one;
                    selected &= frame;
                    current = (void *)(offset + (u32)actor + 0xE8);
                    selected += 16;
                    current->frame = selected;
                }
                {
                    register u32 tileOffset asm("r3") = 0x1C4;
                    register u16 *tiles asm("r0");
                    asm("" : : "r"(tileOffset));
                    tiles = (void *)((u8 *)actor + tileOffset);
                    AnimData_SelectTiles(segmentGraphics, *tiles + 2);
                }
                {
                    register struct StackedActorSprite *base asm("r0") = cache.segments;
                    register u32 *flags asm("r2") = (void *)((u8 *)base + offset);
                    *flags |= 0x201;
                }
                {
                    register u8 *relative asm("r1") = (u8 *)actor + offset;
                    u8 *destination = relative + 0xEF;
                    register u32 renderMode asm("r2") = 1;
                    *destination = renderMode;
                    {
                        register u32 offset asm("r3") = 0x104;
                        register struct StackedActorPosition *destination asm("r2");
                        register struct StackedActorPosition *source asm("r3");
                        destination = (void *)(relative + offset);
                        source = cache.world;
                        *destination = *source;
                    }
                }
                segmentGraphics += 0x1C;
                {
                    u32 step = 1;
                    i += step;
                }
                count = i;
            } while (count <= 2);
            sub_0805321C(actor);
            SceneMessages_RegisterReceiver(cache.receiver, cache.tag, 10);
            if (Script_SeekToKeyword('D')) {
                register u32 zeroByte asm("r2") = 0;
                u32 zeroHalf = 0;
                register u16 *state asm("r3") = cache.proximity;
                *state = zeroHalf;
                actor->animationPhase = zeroHalf;
                actor->interactionDelay = zeroHalf;
                actor->lift = zeroByte;
                sub_08052628(actor, sub_08052640);
            } else {
                s32 active = 0;
                s32 appear;
                if (Script_SeekToKeyword('f'))
                    active = Script_GetValue();
                appear = 0;
                if (Script_SeekToKeyword('s'))
                    appear = Script_GetValue();
                if (active) {
                    s32 tether;
                    Sound_PlaySong(0x160);
                    actor->unknown380 = one;
                    sub_08056C6C(0);
                    sub_081593A0(gUnk_02000580, cache.world);
                    sub_0815AEE0(gUnk_02000580);
                    actor->sprite.flags &= ~1;
                    {
                        u32 tile = actor->tileBase + 2;
                        AnimData_SelectTiles(cache.graphics, tile);
                    }
                    {
                        register struct StackedActorSprite *target asm("r1") = savedShadow;
                        target->flags |= one;
                    }
                    {
                        register u32 offset asm("r2") = 0x1C0;
                        register u16 *destination asm("r1");
                        asm("" : : "r"(offset));
                        destination = (void *)((u8 *)actor + offset);
                        *destination = 7;
                    }
                    sub_08052418(actor);
                    {
                        register u32 offset asm("r3") = 0x1C6;
                        register u16 *destination asm("r1");
                        asm("" : : "r"(offset));
                        destination = (void *)((u8 *)actor + offset);
                        *destination = 8;
                    }
                    *cache.proximity = one;
                    {
                        register u32 offset asm("r2") = 0x36A;
                        register u16 *destination asm("r1");
                        asm("" : : "r"(offset));
                        destination = (void *)((u8 *)actor + offset);
                        *destination = 10;
                    }
                    {
                        register u32 offset asm("r3") = 0x368;
                        register u8 *destination asm("r1");
                        asm("" : : "r"(offset));
                        destination = (void *)((u8 *)actor + offset);
                        *destination = 24;
                    }
                    actor->removeAfterUse = appear;
                    sub_08052628(actor, sub_08052E80);
                    tether = 0;
                    if (Script_SeekToKeyword('w'))
                        tether = Script_GetValue();
                    if (tether) {
                        if (!sub_0804A0A4())
                            return 0;
                        sub_08049FFC(cache.world);
                    }
                } else {
                    {
                        register u32 zeroByte asm("r0") = 0;
                        register u16 *state asm("r2") = cache.proximity;
                        *state = active;
                        {
                            register u32 offset asm("r3") = 0x1C6;
                            register u16 *destination asm("r1");
                            asm("" : : "r"(offset));
                            destination = (void *)((u8 *)actor + offset);
                            *destination = active;
                        }
                        {
                            register u32 offset asm("r2") = 0x36A;
                            register u16 *destination asm("r1");
                            destination = (void *)((u8 *)actor + offset);
                            *destination = active;
                        }
                        {
                            register u32 offset asm("r3") = 0x368;
                            register u8 *destination asm("r1");
                            destination = (void *)((u8 *)actor + offset);
                            *destination = zeroByte;
                        }
                    }
                    if (appear) {
                        Sound_PlaySong(0x15E);
                        actor->sprite.flags |= 2;
                        {
                            struct StackedActorSprite *target = savedShadow;
                            target->flags |= 2;
                        }
                        actor->sprite.scaleX = one;
                        actor->sprite.scaleY = one;
                        actor->shadow.scaleX = one;
                        actor->shadow.scaleY = one;
                        sub_08052628(actor, sub_08052938);
                    } else
                        sub_08052628(actor, sub_08052A40);
                }
            }
        }
    }
    gUnk_02000118 = actor;
    return 0;
}
