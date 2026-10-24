#include "global.h"
// MATCHING_CONCESSION: register-pinning

struct Vec {
    s16 x, y, z, pad;
};
struct PackedVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
struct MessageEntry {
    u16 id;
    u8 pad2[4];
    u8 count[2];
    s16 *messages[2][4];
    u8 pad28[8];
    void *next;
};
struct Actor {
    u8 pad0[0x54];
    struct Vec position;
    u8 pad5c[0x50];
    u8 state, phase, active, finished, commandPending;
    u8 padB1[0x13];
    u32 stateCounter;
    u8 padC8[0x4c];
    u8 savedAngle;
    u8 pad115[3];
    struct MessageEntry entry;
};
#define B(p, o) (*(u8 *)((u8 *)(p) + (o)))
#define H(p, o) (*(u16 *)((u8 *)(p) + (o)))
#define W(p, o) (*(u32 *)((u8 *)(p) + (o)))
extern u8 *gUnk_0200047C;
extern u8 *gUnk_030042E4;
void SceneMessages_SetPendingTrigger(void *, void *);
u32 Trigger_ConsumePending(void *, s32);
u32 ActorResource_GetFixedId(void *);
void CpuSet(const void *, void *, u32);
void PaletteEffect_RefreshPalette(void *);
void ScaledShadow_Enable(void *);
void Ratatosk_PlayAttachmentEffect(void *, s32, s32, s32, s32, s32);
void Ratatosk_FireTrackingShot(void *, void *, s32);
void SceneActors_FireTrackingProjectileSpread(void *, void *, s32);

static inline void setState(struct Actor *q, u32 state, u32 zero, u32 kind)
{
    q->state = state;
    q->phase = zero;
    q->active = kind;
    q->stateCounter = zero;
    q->commandPending = 1;
}
struct Extension {
    u8 pad0[0xe50];
    u16 anchorLift;
    u8 padE52[10];
    struct Vec destination;
    u8 padE64[0x30];
    u16 stateParam;
    u8 padE96[0x4e];
    u16 paletteToggle;
    u8 padEe6[0x2a];
    struct Vec shotTarget;
};
struct Anchor {
    u8 pad0[0x36c];
    struct Vec position;
};
struct Palette {
    u8 pad0[0x394];
    void *data;
    u8 pad398[0x114];
    u8 value, pad4ad, mode, rate;
    u32 timer;
    u8 pad4b4[4];
    u8 buffer[32];
};
struct Anim {
    u32 pad[2];
    u32 flags;
};
static inline void clearAnim(struct Anim *a)
{
    a->flags &= ~1;
}
static inline void setupPalette(struct Palette *p, u8 mode, u32 duration, u8 rate)
{
    p->timer = duration;
    p->mode = mode;
    p->rate = rate;
    if (p->mode == 2) {
        u32 value = 32;
        p->value = value;
    } else if (p->mode == 1)
        p->value = 0;
}
static inline void Palette_SelectResource(u8 *p, u32 id)
{
    struct Palette *palette = (void *)p;
    const u32 originalId = id;
    if (originalId == 0)
        palette->data = p + 0x8b8;
    else if (originalId == 501)
        palette->data = p + 0xab8;
    else {
        CpuSet(gUnk_030042E4 + (id *= 32), palette->buffer, 0x04000008);
        palette->data = palette->buffer;
    }
}
// confidence=low: dispatches queued scene-message commands to Ratatosk states and effects.
#define Ratatosk_DispatchSceneCommands sub_081F6B90
void Ratatosk_DispatchSceneCommands(u8 *p)
{
    struct Actor *q = (void *)p;
    struct MessageEntry *entry = (void *)(p + 0x118);
    union {
        struct Vec vec;
        struct PackedVec packed;
    } position;
    s32 i = 0;
    if (i < entry->count[gUnk_0200047C[0x24]])
        do {
            u8 *controller = gUnk_0200047C;
            s16 *message = entry->messages[controller[0x24]][i];
            SceneMessages_SetPendingTrigger(entry, message);
            switch (((u8 *)message)[6]) {
                case 0:
                    setState(q, 2, 0, 0);
                    break;
                case 1:
                    q->savedAngle = (((u8 *)message)[8] + 5) << 5;
                    setState(q, 2, 0, 1);
                    break;
                case 3:
                    q->savedAngle = (((u8 *)message)[8] + 5) << 5;
                    setState(q, 2, 0, 2);
                    break;
                case 4:
                    q->savedAngle = (((u8 *)message)[8] + 5) << 5;
                    setState(q, 2, 0, 3);
                    break;
                case 10:
                    setState(q, 2, 0, 5);
                    break;
                case 11:
                    setState(q, 2, 0, 6);
                    break;
                case 12:
                    ((struct Extension *)p)->destination.x = message[4];
                    ((struct Extension *)p)->destination.y = message[5];
                    ((struct Extension *)p)->destination.z = message[6];
                    setState(q, 2, 0, 4);
                    break;
                case 19:
                    setState(q, 2, 0, 7);
                    break;
                case 37:
                    ((struct Extension *)p)->paletteToggle = 0;
                    setState(q, 2, 0, 9);
                    break;
                case 38:
                    setState(q, 2, 0, 8);
                    break;
                case 39:
                    ((struct Extension *)p)->paletteToggle = 1;
                    setState(q, 2, 0, 9);
                    break;
                case 40:
                    setState(q, 2, 0, 10);
                    break;
                case 41:
                    ((struct Extension *)p)->stateParam = message[4];
                    setState(q, 2, 0, 11);
                    break;
                case 42:
                    setState(q, 2, 0, 12);
                    break;
                case 43:
                    if (message[4] == 0)
                        setState(q, 2, 0, 13);
                    else
                        setState(q, 2, 0, 14);
                    break;
                case 15: {
                    s32 paletteValue;
                    if (message[4] == 0) {
                        u32 id;
                        ((struct Palette *)p)->value = 0;
                        ((struct Palette *)p)->mode = 0;
                        ((struct Palette *)p)->timer = ActorResource_GetFixedId(p);
                        id = ActorResource_GetFixedId(p);
                        Palette_SelectResource(p, id);
                    } else {
                        paletteValue = message[5];
                        if (paletteValue == 0)
                            setupPalette((void *)p, 3, 501, 5);
                        else
                            setupPalette((void *)p, 3, 500, 5);
                    }
                    Trigger_ConsumePending(&q->entry, 1);
                    break;
                }
                case 16: {
                    s32 paletteValue = message[4];
                    if (paletteValue == 0)
                        setupPalette((void *)p, 1, 498, 5);
                    else {
                        paletteValue = message[5];
                        if (paletteValue == 0)
                            setupPalette((void *)p, 2, 501, 5);
                        else
                            setupPalette((void *)p, 2, 500, 5);
                    }
                    Trigger_ConsumePending(&q->entry, 1);
                    break;
                }
                case 9:
                    Ratatosk_PlayAttachmentEffect(q, message[4], 180, 180, 250, 100);
                    Trigger_ConsumePending(&q->entry, 1);
                    break;
                case 2:
                    q->position.x = message[4];
                    q->position.y = message[5];
                    q->position.z = message[6];
                    ((struct Anchor *)p)->position.x = q->position.x;
                    {
                        register s32 lift asm("r0") = ((struct Extension *)p)->anchorLift;
                        register u32 y asm("r3") = (u16)q->position.y;
                        ((struct Anchor *)p)->position.y = lift + y;
                    }
                    ((struct Anchor *)p)->position.z = q->position.z;
                    Trigger_ConsumePending(&q->entry, 1);
                    break;
                case 5:
                    q->savedAngle = (((u8 *)message)[8] + 5) << 5;
                    setState(q, 2, 0, 22);
                    break;
                case 7:
                    setState(q, 2, 0, 16);
                    break;
                case 8:
                    setState(q, 2, 0, 17);
                    break;
                case 44:
                    setState(q, 2, 0, 18);
                    break;
                case 45:
                    setState(q, 2, 0, 20);
                    break;
                case 46:
                    setState(q, 2, 0, 19);
                    break;
                case 47:
                    position.packed.x = q->position.x;
                    position.packed.y = q->position.y;
                    position.packed.z = q->position.z;
                    Ratatosk_FireTrackingShot(p, &position, message[4]);
                    Trigger_ConsumePending(&q->entry, 1);
                    break;
                case 48:
                    position.vec.x = q->position.x;
                    position.vec.y = q->position.y;
                    position.vec.z = q->position.z;
                    SceneActors_FireTrackingProjectileSpread(p, &position, message[4]);
                    Trigger_ConsumePending(&q->entry, 1);
                    break;
                case 49:
                    ((struct Extension *)p)->shotTarget.x = message[4];
                    ((struct Extension *)p)->shotTarget.y = message[5];
                    ((struct Extension *)p)->shotTarget.z = message[6];
                    Trigger_ConsumePending(&q->entry, 1);
                    break;
                case 50:
                    setState(q, 2, 0, 21);
                    break;
            }
            PaletteEffect_RefreshPalette(p);
            clearAnim((void *)(p + 0x34c));
            ScaledShadow_Enable(p + 0x40c);
        } while (++i < entry->count[gUnk_0200047C[0x24]]);
}
