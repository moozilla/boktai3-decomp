#include "global.h"
#include "render.h"
struct Vec {
    s16 x, y, z, pad;
};
struct Sprite {
    u8 pad[8];
    u32 flags;
    u8 padc[20];
    struct Vec position;
    u8 pad28[12];
    u8 scaleX, scaleY;
    u16 angle;
};
struct Effect {
    u32 key;
    u8 state, entered, kind, mode;
    u32 item, timer;
    u16 duration;
    u8 index, pad13[5];
    struct Vec position;
    struct Vec *target;
    struct Sprite sprite;
    u8 tail[0x84 - 0x5C];
};
struct EffectManager {
    u8 pad0[0x18];
    u32 active;
    u8 frames[0x20];
    struct Effect effects[8];
};
extern u8 *gUnknown_02000580[];
struct EffectManager *FloatingEffectManager_GetOrCreate(void);
void FloatingEffect_Init(struct Effect *);
u32 Selection_GetDisplayFrame(u32);
void Anim_LoadKeyframe(void *, void *, u16, u32);
static inline int FloatingEffect_Scale3(int v)
{
    v *= 3;
    return v >= 0 ? v >> 5 : -((-v) >> 5);
}
#ifdef NONMATCHING
// fuzzy: 59.1%
// scope shape 14 (differences once low registers are set aside)
// scope shape 64 (differences once low registers are set aside)
s32 FloatingItemEffect_Spawn(int kind, int item, int mode, int index, struct Vec *position, struct Vec *target,
                 int duration, u32 id)
{
    struct EffectManager *m = FloatingEffectManager_GetOrCreate();
    u32 key = (id & 0xFFFFFF) | (kind << 28);
    struct Effect *e = NULL;
    s32 slot = -1;
    s32 i;
    u32 frame;
    int idx;
    struct Vec *s;
    int depth, height;
    i = 0;
    do {
        if (!(m->active & (1 << i))) {
            e = &m->effects[i];
            slot = i;
            if (id != 0 && e->key == key) {
                FloatingEffect_Init(e);
                m->active &= ~(1 << item);
                break;
            }
        }
        i++;
    } while (i < 8);
    if (e == NULL)
        return -1;
    e->key = key;
    e->index = index;
    idx = e->index;
    if (gUnknown_02000580[idx] == NULL)
        e->index = 0;
    e->duration = duration;
    e->mode = mode;
    switch (e->mode) {
        case 0:
            e->state = 3;
            e->entered = 1;
            e->timer = 0;
            break;
        case 1:
            e->state = 0;
            e->entered = 1;
            e->timer = 0;
            break;
        case 2:
            e->state = 3;
            e->entered = 1;
            e->timer = 0;
            break;
        case 3:
            e->state = 3;
            e->entered = 1;
            e->timer = 0;
            break;
    }
    e->kind = kind;
    e->item = item;
    switch (e->kind) {
        case 0:
            frame = Selection_GetDisplayFrame(item);
            break;
        case 1:
            frame = item + 0xF0;
            break;
        case 2:
            frame = item + 0x121;
            break;
        case 3:
            frame = item + 0x184;
            break;
        case 4:
            frame = item + 0xE7;
            break;
        case 5:
            frame = item + 0xD9;
            break;
        default:
            frame = 0x68;
            break;
    }
    Anim_LoadKeyframe(&e->sprite, m->frames, frame, 0);
    e->sprite.flags &= ~1;
    e->position = *position;
    e->target = target;
    s = &e->sprite.position;
    s->x = FloatingEffect_Scale3(position->x - position->z);
    depth = FloatingEffect_Scale3(position->x + position->z);
    height = FloatingEffect_Scale3(position->y);
    s->y = depth - height;
    s->z = depth + height;
    s->x = s->x - (*(struct Vec *)gCameraOrigin).x + 120;
    s->y = s->y - (*(struct Vec *)gCameraOrigin).y + 80;
    s->z = s->z - (*(struct Vec *)gCameraOrigin).z;
    s->x -= 8;
    e->sprite.position.y -= 8;
    if (slot >= 0)
        m->active |= 1 << slot;
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", FloatingItemEffect_Spawn);
#endif
