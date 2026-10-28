#include "scene_indicator.h"

struct SceneIndicators *SceneIndicators_GetInstance(void)
{
    return gSceneIndicators;
}

struct PackedVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};

void SceneIndicator_Initialize(struct SceneIndicators *m, struct SceneIndicator *p)
{
    struct PackedVec size, offset;
    int zero;
    u8 *sprite = (u8 *)&p->sprite;
    u8 *effect;
    Sprite_Init(sprite, m->animations, 1);
    zero = 0;
    sprite[7] = 3;
    effect = p->effect;
    size.x = 120;
    size.y = 32;
    size.z = 120;
    offset.x = 0;
    offset.y = 32;
    offset.z = 0;
    Collider_Init(effect, 0, 0x2001, 0, 16, &size, &offset);
    Collider_SetCallback(effect, 0, p);
    Collider_SetAttackProperties(effect, 10, 1, 0x400000, 0x60000, 30);
    p->active = zero;
}

void SceneIndicator_Update(struct SceneIndicators *owner, struct SceneIndicator *p)
{
    u8 old;
    if (p->disabled) {
        if (p->visible) {
            p->visible = 0;
            IndicatorSprite_SetFrame(&p->sprite, p->frame);
        }
        return;
    }
    old = p->visible;
    switch (p->mode) {
        case 0:
            if (p->timer >= (u32)(s16)p->times[p->index]) {
                p->index++;
                if (p->index >= p->count)
                    p->index = 0;
                p->visible = 1 - (p->index & 1);
                if (p->visible)
                    Sound_PlaySongChecked(0x5AA);
                p->timer = 0;
            }
            break;
        case 1:
            if (!old) {
                if (!p->delay) {
                    if (p->value == owner->tile)
                        p->delay = 1;
                } else {
                    p->delay++;
                    if (p->delay >= p->period) {
                        p->visible = 1;
                        p->delay = 60;
                        Sound_PlaySongChecked(0x5AA);
                    }
                }
            } else {
                if (!p->delay)
                    p->visible = 0;
                else
                    p->delay--;
            }
            break;
        case 2:
            break;
    }
    {
        int visible = p->visible;
        if (old != visible)
            IndicatorSprite_SetFrame(&p->sprite, p->frame + visible);
    }
    if (p->visible == 1)
        Collider_Refresh(p->effect);
    p->timer++;
}

void SceneIndicator_Destroy(struct SceneIndicators *owner, struct SceneIndicator *p)
{
    Sprite_Remove(&p->sprite);
}

int SceneIndicators_Update(struct SceneIndicators *m)
{
    int i;
    struct SceneIndicator *p = m->indicators;
    for (i = 15; i >= 0; i--, p++)
        if (p->active)
            SceneIndicator_Update(m, p);
    {
        int x = gPlayers->x >> 8, y = gPlayers->y >> 8, tile;
        if (x < 0 || y < 0 || x >= gTerrainWidth || y >= gTerrainHeight)
            tile = 0;
        else
            tile = gTerrainMap->rows[y] + x;
        m->tile = tile;
    }
    return 0;
}

u32 SceneIndicators_Destroy(struct SceneIndicators *s)
{
    struct SceneIndicator *e = s->indicators;
    s32 i;
    for (i = 15; i >= 0; i--, e++) {
        SceneIndicator_Destroy(s, e);
    }
    return (u32)(gSceneIndicators = 0);
}

#include "scene_indicator.h"
int SceneIndicators_Initialize(struct SceneIndicators *m)
{
    int i;
    struct IndicatorAnimation *a;
    struct SceneIndicator *first;
    gSceneIndicators = m;
    m->count = 0;
    if (Script_SeekToKeyword('t')) {
        for (i = 0, a = m->animations; i < 4; a++, i++) {
            int value;
            if (Script_GetPc()) {
                value = Script_GetValue();
                m->count++;
            } else
                value = 0x4211;
            if (!AnimData_Load(a, value))
                return -1;
        }
    } else {
        for (i = 0, a = m->animations; i < 4; a++, i++)
            if (!AnimData_Load(a, 0x4211))
                return -1;
        m->count = 4;
    }
    asm("" : "+r"(m));
    {
        int has = Script_SeekToKeyword('p');
        first = m->indicators;
        if (has) {
            for (a = m->animations, i = 3; i >= 0; a++, i--) {
                int value;
                if (Script_GetPc()) {
                    value = Script_GetValue();
                    m->count++;
                } else
                    value = 0;
                AnimData_SelectPaletteBank(a, a->frame + value);
            }
        }
    }
    {
        struct SceneIndicator *p = first;
        for (i = 15; i >= 0; i--, p++)
            SceneIndicator_Initialize(m, p);
    }
    if (gPlayers) {
        int x = gPlayers->x >> 8, y = gPlayers->y >> 8, tile;
        if (x < 0 || y < 0 || x >= gTerrainWidth || y >= gTerrainHeight)
            tile = 0;
        else
            tile = gTerrainMap->rows[y] + x;
        m->tile = tile;
    }
    return 0;
}

struct SceneIndicators *SceneIndicators_Create(void)
{
    struct SceneIndicators *p;
    if (gSceneIndicators != 0)
        return gSceneIndicators;
    p = Task_Create(0x8, 0xbd0);
    if (p != 0) {
        Task_SetCallbacks(p, SceneIndicators_Update, SceneIndicators_Destroy);
        if (SceneIndicators_Initialize(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}

struct SceneIndicator *SceneIndicators_FindFreeSlot(struct SceneIndicators *p)
{
    struct SceneIndicator *q = p->indicators;
    s32 i = 0;
    while (i <= 15) {
        if (q->active == 0) {
            return q;
        }
        i++;
        q++;
    }
    return 0;
}

struct SceneIndicator *SceneIndicators_FindById(struct SceneIndicators *p, u32 key)
{
    struct SceneIndicator *q = p->indicators;
    s32 i = 0;
    while (i <= 15) {
        if (q->active != 0 && q->id == key) {
            return q;
        }
        i++;
        q++;
    }
    return 0;
}

#include "scene_indicator.h"
static inline int cellIndex(struct Vec *p)
{
    int x = p->x >> 8, z = p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}

static inline u8 *cell(int i)
{
    u8 *p = TerrainModifiers_FindCellOverride(i, 1);
    if (p)
        return p + 4;
    return gTerrainMap->cells + 12 + i * 4;
}

// MATCHING_CONCESSION: register-pinning
int SceneIndicator_CreateFromScript(void)
{
    struct SceneIndicators *m = SceneIndicators_GetInstance();
    struct SceneIndicator *p;
    struct Vec *position;
    struct IndicatorSprite *sprite;
    void *effect;
    if (!m)
        return -1;
    p = SceneIndicators_FindFreeSlot(m);
    if (!p)
        return -1;
    p->id = Script_GetKeywordValue('n', 0);
    if (Script_SeekToKeyword('p')) {
        p->sprite.position.x = Script_GetValue();
        p->sprite.position.y = Script_GetValue();
        p->sprite.position.z = Script_GetValue();
    }
    if (Script_GetKeywordValue('f', 1)) {
        u8 *c;
        int height, slope;
        p->sprite.position.x = (p->sprite.position.x & ~255) + 128;
        p->sprite.position.z = (p->sprite.position.z & ~255) + 128;
        position = &p->sprite.position;
        c = cell(cellIndex(position));
        slope = *c >> 4;
        height = (*c & 15) << 8;
        switch (slope) {
            case 1:
                height -= (u8)position->z;
                break;
            case 2:
                height -= (u8)position->x;
                break;
        }
        p->sprite.position.y = height;
    }
    p->frame = Script_GetKeywordValue('P', -1);
    if (p->frame < 0) {
        switch (Terrain_GetHigherNeighborMask(&p->sprite.position) & 14) {
            case 2:
                p->frame = 4;
                break;
            case 6:
                p->frame = 4;
                break;
            case 12:
                p->frame = 2;
                break;
            case 8:
                p->frame = 2;
                break;
            case 0:
            case 4:
            case 10:
            case 14:
            default:
                p->frame = 0;
                break;
        }
    }
    p->index = 0;
    p->count = 0;
    if (Script_SeekToKeyword('t')) {
        register u16 *base asm("r6") = p->times;
        u16 *t = base;
        do {
            if (Script_GetPc()) {
                *t = Script_GetValue();
                p->count++;
            } else {
                u32 all = 65535;
                *t = all;
            }
            t++;
        } while ((s32)t <= (s32)(base + 7));
        if (p->count & 1)
            return -1;
        p->mode = 0;
        position = &p->sprite.position;
        sprite = &p->sprite;
        effect = p->effect;
    } else {
        int i;
        register u32 mask asm("r3");
        position = &p->sprite.position;
        sprite = &p->sprite;
        effect = p->effect;
        {
            register u32 all asm("r0") = 65535;
            asm("" : : "r"(all));
            mask = all;
        }
        for (i = 0; i < 8; i++)
            p->times[i] |= mask;
    }
    p->delay = 0;
    p->period = Script_GetKeywordValue('T', -1);
    if (p->period >= 0)
        p->mode = 1;
    if (Script_SeekToKeyword('s')) {
        p->mode = 2;
        p->visible = Script_GetValue();
    }
    p->animation = Script_GetKeywordValue('E', 0);
    Sprite_ApplyAnimation(sprite, &m->animations[p->animation]);
    sprite->frame = p->frame + p->visible;
    Collider_SetPosition(effect, position, 0);
    if (Script_GetKeywordValue('v', 1))
        sprite->flags &= ~1;
    else
        sprite->flags |= 1;
    Collider_SetAttackPowerAndForce(effect, (u16)Script_GetKeywordValue('d', 10), 1);
    {
        register u32 raw asm("r0") = (u16)position->x;
        register int x asm("r2");
        int z;
        x = (s16)raw >> 8;
        z = position->z >> 8;
        if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
            p->value = 0;
        else
            p->value = gTerrainMap->rows[z] + x;
    }
    p->active = 1;
    return 0;
}

int SceneIndicator_Pause(void)
{
    struct SceneIndicators *m = SceneIndicators_GetInstance();
    int id;
    struct SceneIndicator *p;
    if (!m)
        return -1;
    id = Script_GetKeywordValue('n', 0);
    if (!id)
        return -1;
    p = SceneIndicators_FindById(m, id);
    if (!p)
        return -1;
    p->disabled = 1;
    p->visible = 0;
    IndicatorSprite_SetFrame(&p->sprite, p->frame);
    return 0;
}

int SceneIndicator_Resume(void)
{
    struct SceneIndicators *m = SceneIndicators_GetInstance();
    int id;
    struct SceneIndicator *p;
    if (!m)
        return -1;
    id = Script_GetKeywordValue('n', 0);
    if (!id)
        return -1;
    p = SceneIndicators_FindById(m, id);
    if (!p || !p->disabled)
        return -1;
    p->disabled = 0;
    switch (p->mode) {
        case 0:
            p->index = 0;
            break;
        case 1:
            p->delay = 0;
            break;
        case 2:
            p->visible = 1;
            IndicatorSprite_SetFrame(&p->sprite, p->frame + 1);
            break;
    }
    return 0;
}
