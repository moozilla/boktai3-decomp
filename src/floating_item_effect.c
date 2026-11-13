#include "floating_item_effect.h"

void FloatingEffect_Init(struct FloatingEffect *effect)
{
    u32 zero = 0;
    u32 one;
    effect->key = zero;
    one = 1;
    effect->state = one;
    effect->entered = zero;
    effect->kind = zero;
    effect->timer = zero;
    effect->sprite.flags = (effect->sprite.flags | one) & ~2;
}

static inline int FloatingEffect_ProjectScalar(int input)
{
    int v = input * 3;
    if (v >= 0)
        v = v >> 5;
    else
        v = -((-v) >> 5);
    return v;
}

static inline void FloatingEffect_ProjectVec(struct ContactPosition *out, struct ContactPosition *v)
{
    int depth, height;
    int vx = v->x, vz = v->z;
    out->x = FloatingEffect_ProjectScalar(vx - vz);
    depth = FloatingEffect_ProjectScalar(vx + vz);
    height = FloatingEffect_ProjectScalar(v->y);
    out->y = depth - height;
    out->z = depth + height;
    out->x = out->x - (*(struct ContactPosition *)gCameraOrigin).x + 120;
    out->y = out->y - (*(struct ContactPosition *)gCameraOrigin).y + 80;
    out->z = out->z - (*(struct ContactPosition *)gCameraOrigin).z;
}

void FloatingEffect_Project(struct FloatingEffect *effect)
{
    struct SceneInputWidget *sprite = &effect->sprite;
    FloatingEffect_ProjectVec(&sprite->position, &effect->position);
    sprite->position.x -= 8;
    sprite->position.y -= 8;
}

void FloatingEffect_Bob(struct LinkedWidgetSet *manager, struct FloatingEffect *effect)
{
    if (effect->entered) {
        effect->entered = 0;
        effect->sprite.flags &= ~1;
    }
    if (effect->timer <= 2)
        effect->position.y += 16;
    else
        effect->position.y -= 16;
    FloatingEffect_Project(effect);
    if (effect->timer > 3) {
        int state = 1;
        effect->state = state;
        effect->entered = state;
        effect->timer = 0;
    } else
        effect->timer++;
}

void FloatingEffect_Wait(struct LinkedWidgetSet *manager, struct FloatingEffect *effect)
{
    if (effect->entered) {
        effect->entered = 0;
        effect->sprite.flags &= ~1;
    }
    FloatingEffect_Project(effect);
    if (effect->timer >= effect->duration) {
        u32 mode = effect->mode;
        if (mode == 0) {
            u32 index = effect->index;
            if (gPlayers[index])
                effect->state = 6;
            else
                effect->state = 5;
            effect->entered = 1;
            effect->timer = mode;
        } else if (mode == 1) {
            int state = 2;
            effect->state = state;
            effect->entered = mode;
            effect->timer = 0;
        } else if (mode == 3) {
            int state = 6;
            effect->state = state;
            effect->entered = 1;
            effect->timer = 0;
        }
    } else
        effect->timer++;
}

void FloatingEffect_ShrinkAway(struct LinkedWidgetSet *manager, struct FloatingEffect *effect,
                               int index)
{
    struct SceneInputWidget *sprite = &effect->sprite;
    u8 *scale;
    if (effect->entered) {
        effect->entered = 0;
        sprite->flags |= 2;
        sprite->rotation = 0;
        effect->sprite.scaleX = 64;
        effect->sprite.scaleY = 64;
    }
    scale = &effect->sprite.scaleX;
    if (*scale > 8)
        *scale -= 8;
    effect->sprite.scaleY = *scale;
    sprite->rotation += 16;
    effect->position.x += 8;
    effect->position.z += 8;
    FloatingEffect_Project(effect);
    sprite->position.x += (64 - *scale) >> 3;
    if (effect->timer > 7) {
        FloatingEffect_Init(effect);
        manager->active &= ~(1 << index);
    } else
        effect->timer++;
}

static inline u32 FloatingEffect_RandomAt(const u16 *table, u16 index)
{
    return table[index];
}

// confidence=low: rises for the first few frames, then sinks (shaking in mode 3) and moves to

// state 4 after 8 (mode 3: 9) frames.

void FloatingEffect_RiseAndSettle(struct LinkedWidgetSet *manager, struct FloatingEffect *effect)
{
    if (effect->entered) {
        effect->entered = 0;
        effect->sprite.flags &= ~1;
    }
    if (effect->mode == 3) {
        if (effect->timer <= 3)
            effect->position.y += 0x40;
        else
            effect->position.y -= 4;
    } else if (effect->timer <= 4)
        effect->position.y += 0x40;
    else
        effect->position.y -= 8;
    FloatingEffect_Project(effect);
    if ((effect->mode == 3) && (effect->timer > 3)) {
        const u16 *table = gRandomTable;
        u32 *seed = &gRandomIndex;
        u32 mask;
        u16 index = *seed + 1;
        mask = 0x3FF;
        index &= mask;
        *seed = index;
        {
            u32 bits = FloatingEffect_RandomAt(table, index);
            s32 x = (u16)effect->sprite.position.x - 1;
            effect->sprite.position.x = x + (bits & 3);
        }
        index = (index + 1) & mask;
        *seed = index;
        {
            u32 bits = FloatingEffect_RandomAt(table, index);
            s32 y = (u16)effect->sprite.position.y - 1;
            effect->sprite.position.y = y + (bits & 3);
        }
    }
    if (effect->mode == 3) {
        if (effect->timer <= 8)
            goto increment;
    } else if (effect->timer <= 7)
        goto increment;
    {
        u32 nextState = 4;
        u32 zero = 0;
        effect->state = nextState;
        effect->entered = 1;
        effect->timer = zero;
    }
    goto done;
increment:
    effect->timer++;
done:;
}

void FloatingEffect_RiseAndShrink(struct LinkedWidgetSet *manager, struct FloatingEffect *effect,
                                  int index)
{
    struct SceneInputWidget *sprite = &effect->sprite;
    u8 *scale;
    if (effect->entered) {
        effect->entered = 0;
        effect->sprite.flags &= ~1;
        sprite->flags |= 2;
        sprite->rotation = 0;
        effect->sprite.scaleX = 64;
        effect->sprite.scaleY = 64;
    }
    scale = &effect->sprite.scaleX;
    if (*scale > 16)
        *scale -= 16;
    effect->position.y += 64;
    FloatingEffect_Project(effect);
    sprite->position.x += (64 - *scale) >> 3;
    if (effect->timer > 3) {
        FloatingEffect_Init(effect);
        manager->active &= ~(1 << index);
    } else
        effect->timer++;
}

static inline int FloatingEffect_Scale4(int v)
{
    return v >= 0 ? v >> 4 : -((-v) >> 4);
}

void FloatingItemEffect_UpdateFlight(struct LinkedWidgetSet *manager, struct FloatingEffect *effect,
                                     int index)
{
    struct SceneInputWidget *sprite = &effect->sprite;
    struct ContactPosition to, from;
    u32 t, inv;
    u8 *scale;
    int xl;
    if (effect->entered) {
        effect->entered = 0;
        effect->sprite.flags &= ~1;
        sprite->flags |= 2;
        sprite->rotation = 0;
        effect->sprite.scaleX = 64;
        effect->sprite.scaleY = 64;
    }
    t = effect->timer;
    inv = 16 - t;
    if (effect->mode == 0) {
        u32 targetIndex = effect->index;
        struct Player *target = gPlayers[targetIndex];
        FloatingEffect_ProjectVec(&to, (void *)&target->position);
    } else
        FloatingEffect_ProjectVec(&to, effect->target);
    FloatingEffect_ProjectVec(&from, &effect->position);
    to.y -= 4;
    sprite->position.x = FloatingEffect_Scale4(from.x * inv + to.x * t);
    sprite->position.y = FloatingEffect_Scale4(from.y * inv + to.y * t);
    scale = &sprite->scaleX;
    if (*scale > 32)
        *scale -= 2;
    sprite->scaleY = *scale;
    sprite->rotation += 8;
    xl = (u16)sprite->position.x - 8;
    sprite->position.x = xl + ((64 - *scale) >> 3);
    sprite->position.y -= 8;
    if (effect->timer > 15) {
        if (effect->kind == 0) {
            if (effect->item - 40 <= 6)
                Sound_PlaySongChecked(0x183);
            else if (Inventory_IsKeyItemId(effect->item) == 1)
                Sound_PlaySongChecked(0x123);
            else
                Sound_PlaySongChecked(0x111);
        } else if (effect->kind != 3) {
            Sound_PlaySongChecked(0x123);
        }
        FloatingEffect_Init(effect);
        manager->active &= ~(1 << index);
    } else
        effect->timer++;
}

s32 FloatingEffectManager_Update(struct LinkedWidgetSet *manager)
{
    s32 i = 0;
    void (**table)(struct LinkedWidgetSet *, struct FloatingEffect *, s32) = gUnk_08604C64;
    u8 *state;
    struct FloatingEffect *effect;
    state = &manager->effects[0].state;
    effect = manager->effects;
    for (; i <= 7; state += sizeof(*effect), effect++, i++) {
        if (manager->active & (1 << i)) {
            if (*state <= 6)
                table[*state](manager, effect, i);
        }
    }
}

u32 FloatingEffectManager_Destroy(struct LinkedWidgetSet *manager)
{
    s32 i;
    struct SceneInputWidget *sprite = &manager->effects[0].sprite;
    for (i = 7; i >= 0; i--) {
        Anim_Remove(sprite);
        sprite = (void *)((u8 *)sprite + sizeof(struct FloatingEffect));
    }
    {
        u32 zero = 0;
        gLinkedWidgetSet = (void *)zero;
    }
}

s32 LinkedWidgetSet_Init(struct LinkedWidgetSet *manager)
{
    struct ChoiceResource *resource;
    struct ContactPosition zero;
    s32 i;
    gLinkedWidgetSet = manager;
    resource = ScriptProc_Start(0xCB05, 0x5D04);
    if (!resource)
        return -1;
    manager->resource = *resource;
    ResourceHeader_Relocate(&manager->resource, resource);
    manager->active = 0;
    ZeroFill8(&zero);
    for (i = 0; i < 8; i++) {
        struct FloatingEffect *effect = &manager->effects[i];
        effect->key = 0;
        effect->state = 1;
        effect->kind = 0;
        effect->timer = 0;
        Widget_InitLinked(&effect->sprite, &manager->resource, 0, 0x11, 1, 0, 0x3C, &zero);
    }
}

struct LinkedWidgetSet *FloatingEffectManager_Create(void)
{
    struct LinkedWidgetSet *manager;
    if (gLinkedWidgetSet != 0)
        return gLinkedWidgetSet;
    manager = Task_Create(9, sizeof(*manager));
    if (manager) {
        Task_SetCallbacks(manager, (void *)FloatingEffectManager_Update,
                          (void *)FloatingEffectManager_Destroy);
        if (LinkedWidgetSet_Init(manager) < 0) {
            Task_Kill(manager);
            return 0;
        }
    }
    return manager;
}

struct LinkedWidgetSet *FloatingEffectManager_GetOrCreate(void)
{
    struct LinkedWidgetSet *manager = gLinkedWidgetSet;
    if (manager == 0)
        return FloatingEffectManager_Create();
    return manager;
}

s32 FloatingItemEffect_Spawn(int kind, int item, int mode, int index,
                             struct ContactPosition *position, struct ContactPosition *target,
                             int duration, u32 id)
{
    struct LinkedWidgetSet *manager = FloatingEffectManager_GetOrCreate();
    u32 key = (id & 0xFFFFFF) | (kind << 28);
    struct FloatingEffect *effect = NULL;
    s32 slot = -1;
    s32 i;
    u32 frame;
    int idx;
    i = 0;
    do {
        if (!(manager->active & (1 << i))) {
            effect = &manager->effects[i];
            slot = i;
            if (id != 0 && effect->key == key) {
                FloatingEffect_Init(effect);
                manager->active &= ~(1 << item);
                break;
            }
        }
        i++;
    } while (i < 8);
    if (effect == NULL)
        return -1;
    effect->key = key;
    effect->index = index;
    idx = effect->index;
    if (gPlayers[idx] == NULL)
        effect->index = 0;
    effect->duration = duration;
    effect->mode = mode;
    switch (effect->mode) {
        case 0:
            effect->state = 3;
            effect->entered = 1;
            effect->timer = 0;
            break;
        case 1:
            effect->state = 0;
            effect->entered = 1;
            effect->timer = 0;
            break;
        case 2:
            effect->state = 3;
            effect->entered = 1;
            effect->timer = 0;
            break;
        case 3:
            effect->state = 3;
            effect->entered = 1;
            effect->timer = 0;
            break;
    }
    effect->kind = kind;
    effect->item = item;
    switch (effect->kind) {
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
    Anim_LoadKeyframe(&effect->sprite, &manager->resource, (u16)frame, 0);
    effect->sprite.flags &= ~1;
    effect->position = *position;
    effect->target = target;
    FloatingEffect_ProjectVec(&effect->sprite.position, position);
    effect->sprite.position.x -= 8;
    effect->sprite.position.y -= 8;
    if (slot >= 0)
        manager->active |= 1 << slot;
    return 0;
}

void ScanlineWave_Reset(void)
{
    gScanlineWave = 0;
}
