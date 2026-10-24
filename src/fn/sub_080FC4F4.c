// MATCHING_CONCESSION: register-pinning
#include "scene_actor_registry.h"
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// Remaining differences: actor allocation, collider argument scheduling, and stack spills.
struct Frames {
    u16 *data;
    u8 flags, count, multiplier, duration;
    u16 index, pad, rate, timer;
};
struct Render {
    u32 flags;
    u8 pad[12];
    u16 tile;
};
static inline void step(struct Frames *f, struct Render *r)
{
    u16 *word = f->data + f->index;
    r->tile = *word >> 6;
    {
        int settings = f->flags;
        register int mask asm("r3") = 1;
        register int mine asm("r1");
        int bits;
        mine = mask & settings;
        bits = (*word & 48) >> 4;
        bits &= mask;
        if (mine != bits)
            r->flags |= 4;
        else
            r->flags &= ~4;
    }
    {
        int settings = f->flags;
        register int mask asm("r3") = 2;
        register int mine asm("r1");
        int bits;
        mine = mask & settings;
        mine = (u8)mine;
        bits = (*word & 48) >> 4;
        bits &= mask;
        if (mine != bits)
            r->flags |= 8;
        else
            r->flags &= ~8;
    }
    {
        u32 mask;
        int duration;
        register int savedDuration asm("r3");
        u32 timer = f->timer + 1;
        f->timer = timer;
        {
            register u32 initialMask asm("r1") = 65535;
            asm("" : "+r"(initialMask));
            mask = initialMask;
        }
        duration = f->duration;
        timer = (u16)timer;
        savedDuration = duration;
        if (timer >= (u32)duration) {
            register int flags asm("r1");
            int index;
            f->timer = 0;
            flags = f->flags;
            if (flags & 4) {
                index = f->index;
                if (!index) {
                    if (flags & 8) {
                        f->timer = savedDuration;
                        goto update;
                    }
                    index = f->count;
                }
                index--;
                goto storeIndex;
            } else {
                int count;
                index = f->index + 1;
                f->index = index;
                count = f->count;
                if ((index & mask) < count)
                    goto update;
                index = (u8)(flags & 8);
                if (index) {
                    f->timer = duration;
                    index = count - 1;
                }
            }
        storeIndex:
            f->index = index;
        update:
            word = f->data + f->index;
            {
                u32 value = *word;
                u32 low = 15;
                low &= value;
                f->multiplier = low;
            }
            f->duration = (f->multiplier * f->rate) >> 6;
            if (!f->duration)
                f->duration = 1;
        }
    }
}

void Collider_SetBodyProperties(void *, u16, u32, u32);
void Collider_SetAttackProperties(void *, u16, u16, u32, u32, u16);
void Axe_LatchColliderContact(void);
void Axe_OnSecondaryColliderContact(void);
u32 Shadow_Initialize(void *, void *, s32);
void Axe_InitAttackTrail(struct SceneActorRegistryActor *s)
{
    struct ContactPackedVector bodySize;
    struct ContactPackedVector bodyOffset;
    struct ContactPackedVector attackSize;
    struct ContactPackedVector attackOffset;
    u8 *d = s->context;
    void *resource = d + 0xa64;
    struct Render *sprite;
    struct Frames *frames;
    void *sequence;
    s32 i;
    typeof(*((u16 *)(((u8 *)s) + 0xb4))) t1;
    typeof(*((u16 *)(((u8 *)s) + 0xbc))) t2;
    typeof(*((u32 *)(((u8 *)s) + 0xcc))) t3;
    typeof(*((u16 *)(((u8 *)s) + 0xc4))) t4;
    AnimData_Load(resource, 0x5291);
    sprite = (void *)(d + 0xa38);
    Sprite_Init(sprite, resource, 0);
    sequence = ScriptProc_Start(0x922e, 0x3f72);
    {
        struct {
            u8 pad[0xa];
            u16 index;
        } *drawable = (void *)(((u8 *)s->drawable) + 0x48);
        u32 index = drawable->index;
        frames = (void *)(d + 0xa80);
        Anim_SetSequence(frames, sequence, index, d[0xed7], 0);
    }
    step(frames, sprite);
    *((u32 *)(d + 0xa38)) |= 1;
    *((struct ContactPosition *)(d + 0xa54)) = s->position;
    d[0xa3f] = 1;
    bodySize.x = 24;
    bodySize.y = 70;
    bodySize.z = 24;
    bodyOffset.x = 0;
    bodyOffset.y = 0;
    bodyOffset.z = 0;
    for (i = 0; i < 3; i++) {
        void *c = (d + 0xb8c) + (i * 0x54);
        Collider_Init(c, 0, 0x5001, 0, 16, &bodySize, &bodyOffset);
        Collider_SetBodyProperties(c, 0, 0, 0);
        Collider_SetPosition(c, d + 0xc88, 0);
        Collider_SetCallback(c, Axe_LatchColliderContact, d);
        Collider_Refresh(c);
    }

    attackSize.x = 24;
    attackSize.y = 64;
    attackSize.z = 24;
    attackOffset.x = 0;
    attackOffset.y = 0;
    attackOffset.z = 0;
    for (i = 0; i < 3; i++) {
        void *c = (d + 0xa90) + (i * 0x54);
        do {
            Collider_Init(c, 0, 0x2401, 0, 16, &attackSize, &attackOffset);
        } while (0);
        t1 = *((u16 *)(((u8 *)s) + 0xb4));
        t2 = *((u16 *)(((u8 *)s) + 0xbc));
        t3 = *((u32 *)(((u8 *)s) + 0xcc));
        t4 = *((u16 *)(((u8 *)s) + 0xc4));
        Collider_SetAttackProperties(c, t1, t2, 0, t3, t4);
        Collider_SetPosition(c, d + 0xc88, 0);
        Collider_SetCallback(c, Axe_OnSecondaryColliderContact, d);
    }

    d[0xca7] = Shadow_Initialize(d + 0xcac, d + 0xa54, 0);
    ActorShadow_Disable(d + 0xcac);
}

#else
INCLUDE_ASM("asm/nonmatching", Axe_InitAttackTrail);
#endif
