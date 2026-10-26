#include "global.h"
#include "engine.h"
#include "ram.h"
// MATCHING_CONCESSION: register-pinning
struct Vec16 {
    s16 x, y, z;
};
struct SceneObject {
    u32 next0;
    u32 flags;
    u8 pad8[4];
    s16 x, y, z;
    u8 pad12[0x32];
    struct SceneObject *next;
};
struct SceneObjectList {
    u8 pad0[0x18];
    struct SceneObject *head;
};
struct FloorSwitch {
    u32 flags;
    u8 pad4[8];
    void *anim;
    u16 field10;
    u8 pad12[0xA];
    s16 x, y, z;
    u8 pad22[0xA];
    u8 animData[0x1C];
    s32 name;
    s32 r[2];
    s32 region;
    u16 sprite, field5A;
    u8 pad5C[8];
    s32 s[4];
    s32 w[4];
    u8 pad84[2];
    u16 field86;
    u16 flags88;
    u8 field8A, field8B;
    u16 field8C, field8E;
};
void *FloorSwitch_GetOrCreateManager(void);
struct FloorSwitch *FloorSwitch_AllocateSlot(void *);
struct SceneObjectList *SceneBody_GetRegistry(void);
s32 Region_FindContaining(struct Vec16 *);
static inline s32 optionalValue(void)
{
    s32 value;
    if (Script_GetPc())
        value = Script_GetValue();
    else
        value = 0;
    return value;
}
static inline u8 FloorSwitch_InCurrentRegion(struct FloorSwitch *e)
{
    if (*(s32 *)((u8 *)gGameState + 0x5A0) == e->region)
        return TRUE;
    return FALSE;
}
/* Registry routine AB8E (GCL proc place_floor_switch): takes a slot of the
   floor switch manager (0x020001A8, created on demand) and fills it from the
   script; a switch placed under a scene object starts pressed. */
void FloorSwitch_Create(void)
{
    void *manager = gFloorSwitchManager;
    struct FloorSwitch *e;
    struct FloorSwitch *p;
    struct Vec16 delta;
    s32 i, f;
    if (!manager) {
        manager = FloorSwitch_GetOrCreateManager();
        if (!manager)
            return;
    }
    e = FloorSwitch_AllocateSlot(manager);
    if (!e)
        return;
    e->field86 = 1;
    p = e;
    if (Script_SeekToKeyword('p')) {
        p->x = Script_GetValue();
        p->y = Script_GetValue();
        p->z = Script_GetValue();
    }
    p->name = Script_GetKeywordValue('n', 0);
    if (Script_SeekToKeyword('R')) {
        e->r[0] = optionalValue();
        e->r[1] = optionalValue();
    }
    if (Script_SeekToKeyword('S')) {
        s32 *out = e->s;
        for (i = 3; i >= 0; i--)
            *out++ = optionalValue();
    }
    if (Script_SeekToKeyword('W')) {
        s32 *out = e->w;
        for (i = 3; i >= 0; i--)
            *out++ = optionalValue();
    }
    e->sprite = Script_GetKeywordValue('T', 0);
    AnimData_Load(e->animData, e->sprite);
    e->flags88 = 0;
    if (!Script_GetKeywordValue('s', 0))
        e->flags88 |= 1;
    else
        e->flags88 |= 2;
    f = Script_GetKeywordValue('f', 0);
    if (f)
        e->flags88 |= f;
    Sprite_Init(p, e->animData, 0);
    if (e->flags88 & 2) {
        p->field10 = 1;
        AnimData_SelectPaletteBank(e->anim, 0x16D);
    } else {
        p->field10 = 0;
        AnimData_SelectPaletteBank(e->anim, 0x16C);
    }
    {
        register struct SceneObject *o asm("r4");
        struct Vec16 *d;
        if (SceneBody_GetRegistry())
            goto scan;
        goto skip;
    found:
        e->field5A = o->next0;
        e->flags88 |= 2;
        e->flags88 &= ~1;
        e->field8C = 0;
        e->field10 = 1;
        AnimData_SelectPaletteBank(e->anim, 0x16D);
        goto skip;
    scan:
        for (o = SceneBody_GetRegistry()->head; o; o = o->next) {
            d = &delta;
            if (!(o->flags & 2)) {
                struct Vec16 *op = (struct Vec16 *)&o->x;
                struct Vec16 *ep = (struct Vec16 *)&e->x;
                d->x = o->x - ep->x;
                d->y = op->y - ep->y;
                d->z = op->z - ep->z;
                if ((u16)d->x <= 0x100 && d->y <= 0x20 && d->y >= 0 && d->z <= 0x100 && d->z >= 0)
                    goto found;
            }
        }
    }
skip:
    e->field8E = Script_GetKeywordValue('d', 0);
    e->field8C = 0;
    e->region = Region_FindContaining((struct Vec16 *)&p->x);
    if (!FloorSwitch_InCurrentRegion(e))
        p->flags |= 1;
    else
        p->flags &= ~1;
    e->field8B = 0;
    e->field8A = 0;
}
