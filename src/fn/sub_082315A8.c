// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "ram.h"
#include "render.h"
/* EffectGroup_Spawn: sets a 4-part effect group kind (installing its update callback), spawns one part at the source position projected to screen space plus an offset, and toggles flag 0x200 on all parts for kind 2 (provisional name) */

struct Unk082315A8Vec {
    s16 x, y, z, pad;
};
struct Unk082315A8Part {
    u8 state, unknown1;
    u16 timer;
    u16 parameter;
    u8 unknown6[2];
    struct Unk082315A8Vec position;
    u32 flags;
    u8 unknown14[8];
    u32 flags1C;
    u8 unknown20[8];
    s16 x, y, z;
    u8 unknown2E[0xA];
    u8 anim[0x10];
};
struct Unk082315A8Group {
    u8 unknown0;
    u8 kind, active, spawned;
    u8 unknown4[4];
    struct Unk082315A8Vec *source;
    struct Unk082315A8Part parts[4];
    u8 unknown12C_pad[0x12C - 0x12C];
    void *callback;
};
struct Unk082315A8Graphics {
    u8 unknown0[0x1C];
    void *animation;
};
extern const u16 gUnk_08E879A0[];
void sub_08231444(void);
void EffectGroup_Reset(void *);
void Anim_SetSequence(void *, void *, u32, u32, u32);
void sub_08231EF8(void);
void sub_08231FDC(void);
void sub_082320C0(void);

static inline s32 Scale(s32 v)
{
    s32 t = v * 3;
    s32 r;
    if (t >= 0)
        r = t >> 5;
    else
        r = -(-t >> 5);
    return r;
}

s32 EffectGroup_Spawn(struct Unk082315A8Group *group, u32 kind, u32 parameter,
                      struct Unk082315A8Vec *position)
{
    struct Unk082315A8Graphics *graphics = gSceneActorEffectsManager;
    u32 spawned;
    struct Unk082315A8Vec delta;
    struct Unk082315A8Part *part;
    s32 i, sum, y;

    if (!graphics)
        return -1;
    if (kind == 0) {
        EffectGroup_Reset(group);
        return 0;
    }
    if ((group->active && group->kind != kind) || group->active > 3)
        return -1;
    if (group->kind != kind) {
        group->kind = kind;
        switch (group->kind) {
            case 0:
                group->callback = sub_08231444;
                break;
            case 1:
                group->callback = sub_08231EF8;
                break;
            case 2:
                group->callback = sub_08231FDC;
                break;
            case 3:
                group->callback = sub_082320C0;
                break;
        }
    }
    spawned = 0;
    {
        struct Unk082315A8Vec *d = &delta;
        struct Unk082315A8Vec *s = group->source;
        s32 x = s->x, z = s->z;
        d->x = Scale(x - z);
        sum = Scale(x + z);
        y = Scale(s->y);
        {
            s32 a = sum - y, b = sum + y;
            d->x = d->x - gCameraOrigin[0] + 0x78;
            d->y = a - gCameraOrigin[1] + 0x50;
            d->z = b - gCameraOrigin[2];
        }
    }
    for (i = 0; i <= 3; i++) {
        part = &group->parts[i];
        if (!part->state) {
            part->state = 1;
            part->timer = 0;
            part->parameter = parameter;
            part->position = *position;
            part->x = delta.x + part->position.x;
            part->y = delta.y + part->position.y;
            part->z = delta.z + part->position.z;
            part->flags &= ~1;
            Anim_SetSequence(part->anim, graphics->animation, gUnk_08E879A0[group->kind], 0, 0);
            spawned = 1;
            break;
        }
    }
    if (group->kind == 2) {
        asm("" : "+r"(group));
        for (i = 0; i <= 3; i++)
            group->parts[i].flags |= 0x200;
    } else {
        for (i = 0; i <= 3; i++)
            group->parts[i].flags &= ~0x200;
    }
    if (spawned) {
        group->active++;
        group->spawned++;
    }
    return 0;
}
