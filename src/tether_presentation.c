#include "tether_presentation.h"

/* Tentative: caller derives quadrant from x/y, then selects a sprite animation. */

s32 DirectionMarker_GetAnimation(s32 base, s32 group)
{
    if (group <= 1)
        base += 0x27;
    else
        base += 0x23;
    return base;
}

s32 DirectionMarker_Update(struct TetherRequestIndicator *indicator)
{
    s32 quadrant;
    s32 newIndex;
    s16 *position = &indicator->projected.x;

    if (position[0] > 0x77) {
        if (position[1] > 0x4f) {
            quadrant = 1;
        } else {
            quadrant = 0;
        }
    } else {
        if (position[1] > 0x4f) {
            quadrant = 2;
        } else {
            quadrant = 3;
        }
    }

    newIndex = DirectionMarker_GetAnimation(quadrant, indicator->request);
    if (newIndex != indicator->animationIndex) {
        indicator->animationIndex = newIndex;
        Anim_SetKeyframes(&indicator->animation, &indicator->resource,
                          (u16)indicator->animationIndex, 1, 0);
    }
    Anim_Step(&indicator->animation, &indicator->resource);
    return quadrant;
}

u8 *TerrainModifiers_FindCellOverride(s32, s32);

static inline s32 scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}

static inline u8 *cellProperties(s32 cell)
{
    u8 *properties = TerrainModifiers_FindCellOverride(cell, 1);
    if (properties)
        properties += 4;
    else {
        struct EngineMap *map = gTerrainMap;
        u32 offset = cell * 4 + 12;
        properties = map->cells + offset;
    }
    return properties;
}

static inline u16 cellFlags(s32 cell)
{
    return *(u16 *)(cellProperties(cell) + 2);
}

static inline s32 cellIndex(s32 x, s32 z)
{
    s32 index;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        index = 0;
    else
        index = gTerrainMap->rows[z] + x;
    return index;
}

// confidence=low: true when the tether position, or a point 1 or 2 cells out in any of the four

// directions, lies on a cell with flag 0x80 set.

s32 TetherActor_IsNearRequestTerrain(void *p)
{
    struct ContactPosition *pos = TetherActor_GetPosition();
    struct ContactPosition v;
    s32 i, j;
    if (cellFlags(cellIndex(pos->x >> 8, pos->z >> 8)) & 0x80)
        return 1;
    for (i = 0; i < 4; i++) {
        s32 angle;
        j = 1;
        angle = i << 6;
        for (; j <= 2; j++) {
            s32 r = j << 8;
            struct ContactPosition *pv = &v;
            pv->x = scale(gUnk_086149C4[(angle + 64) & 255] * r);
            pv->y = 0;
            pv->z = scale(gUnk_086149C4[angle & 255] * r);
            pv->x += pos->x;
            pv->y += pos->y;
            pv->z += pos->z;
            if (cellFlags(cellIndex(pv->x >> 8, pv->z >> 8)) & 0x80)
                return 1;
        }
    }
    return 0;
}

static inline s32 actorBlocked(struct StateActor *actor)
{
    u32 mask = 2;
    return actor->actionFlags & mask;
}

static inline void projectPosition(struct ContactPosition *projected, struct ContactPosition *world)
{
    s32 x = world->x;
    s32 z = world->z;
    s32 scaled;
    s32 sum, vertical, y, depth;
    projected->x = projectScale(3 * (x - z));
    scaled = projectScale(3 * (x + z));
    sum = scaled;
    vertical = projectScale(3 * world->y);
    y = sum - vertical;
    depth = sum + vertical;
    projected->x = (u16)projected->x - gCameraOrigin[0] + 120;
    projected->y = y - gCameraOrigin[1] + 80;
    projected->z = depth - gCameraOrigin[2];
}

void TetherActor_PositionRequestIndicator(struct TetherRequestIndicator *indicator)
{
    struct ContactPosition *screen = &indicator->animation.position;
    struct ContactPosition *world = TetherActor_GetPosition();
    struct ContactPosition *projected = &indicator->projected;
    projectPosition(projected, *(&world));
    switch (DirectionMarker_Update(indicator)) {
        case 0:
            screen->x = (u16)indicator->projected.x - 32;
            screen->y = (u16)indicator->projected.y + 32;
            break;
        case 1:
            screen->x = (u16)indicator->projected.x - 32;
            screen->y = (u16)indicator->projected.y - 32;
            break;
        case 2:
            screen->x = (u16)indicator->projected.x + 32;
            screen->y = (u16)indicator->projected.y - 32;
            break;
        case 3:
            screen->x = (u16)indicator->projected.x + 32;
            screen->y = (u16)indicator->projected.y + 32;
            break;
    }

    if (screen->x < 16) {
        screen->x = 16;
    } else if (screen->x > 208) {
        screen->x = 208;
    }
    if (screen->y < 16) {
        screen->y = 16;
    } else if (screen->y > 128) {
        screen->y = 128;
    }
    if (!TetherActor_IsNearRequestTerrain(indicator) || actorBlocked(indicator->actor) ||
        indicator->request <= 0) {
        indicator->visible = 0;
        indicator->animation.flags |= 1;
    } else {
        if (!indicator->visible) {
            indicator->visible = 1;
            Sound_PlaySong(0x4d9);
        }
        indicator->animation.flags &= ~1;
    }
}

void TetherActor_UpdateRequestIndicator(void *arg)
{
    struct TetherRequestIndicator *indicator = arg;
    if (gEnginePauseFlags != 0) {
        indicator->animation.flags |= 1;
    } else {
        indicator->request = TetherActor_GetRequestMode();
        TetherActor_PositionRequestIndicator(indicator);
    }
}

void TetherActor_DestroyRequestIndicator(void *arg)
{
    struct TetherRequestIndicator *indicator = arg;
    if (indicator->animation.registered != 0)
        Anim_Remove(&indicator->animation);
}

s32 ResourceHeader_Relocate(void *dst, void *src);

void Widget_InitLinked(void *, void *, int, int, int, int, int, void *);

s32 TetherActor_InitRequestIndicator(struct TetherRequestIndicator *indicator)
{
    struct TetherIndicatorResource *resource;

    resource = ScriptProc_Start(0xCB05, 0xF771);
    indicator->resource = *resource;
    ResourceHeader_Relocate(&indicator->resource, resource);
    Widget_InitLinked(&indicator->animation, &indicator->resource, 0, 0x10, 0, 1, 0x3c, 0);
    Anim_SetKeyframes(&indicator->animation, &indicator->resource,
                      (u16)DirectionMarker_GetAnimation(0, 0), 0, 0);
    indicator->animation.flags |= 1;
    indicator->actor = SceneActors_GetBySlot(1);
    indicator->visible = 0;
    indicator->animationIndex = 0xFFFF;
    return 0;
}

void *TetherActor_CreateRequestIndicator(void)
{
    void *indicator = Task_Create(9, 0xac);
    if (indicator != 0) {
        Task_SetCallbacks(indicator, TetherActor_UpdateRequestIndicator,
                          TetherActor_DestroyRequestIndicator);
        if (TetherActor_InitRequestIndicator(indicator) < 0) {
            Task_Kill(indicator);
            return 0;
        }
    }
    return indicator;
}

void ScenePaletteFade_SetState(struct ScenePaletteFade *fade, void (*v)(struct ScenePaletteFade *))
{
    fade->update = v;
    fade->timer = 0;
}

void Palette_InterpolateBanks(void *, void *, void *, int, int);

void ScenePaletteFade_BlendPaletteBanks(u8 *destination, u8 *first, u8 *second, int frame,
                                        int shift, int banks)
{
    if (banks > 0) {
        u8 *secondBank = second;
        u8 *firstBank = first;
        u8 *destinationBank = destination;
        int count = banks;
        /* do-while(0) only keeps agbcc's block layout */
        do {
            Palette_InterpolateBanks(destinationBank, firstBank, secondBank, frame, shift);
            secondBank += 0x20;
            firstBank += 0x20;
            /* do-while(0) only keeps agbcc's block layout */
            do {
                destinationBank += 0x20;
            } while (0);
        } while (--count);
    }
}

/* Tentative screen family: keeps two palettes and copies the first to shadow RAM. */

void Bg_SetPriority(u32, s32);

u8 *sub_08216E54(u16);

void PaletteScreen_LoadResources(struct ScenePaletteFade *fade)
{
    u8 *resource;
    u8 *palette;
    s32 layers[2];
    resource = sub_08216E54(0x4ff);
    fade->background = resource;
    layers[0] = 1;
    layers[1] = 0;
    Bg_SetupLayers(1, 0, resource, 0, 0, 2, layers);
    Bg_SetPriority(2, 2);
    Bg_SetPriority(3, 3);
    palette = (u8 *)((struct ScreenPaletteResource *)ScriptProc_Start(0x92b3, 0x663d))->colors;
    fade->firstPalette = palette;
    CpuSet(palette, gBgPaletteBuffer, 0x4000080);
    fade->secondPalette =
        (u8 *)((struct ScreenPaletteResource *)ScriptProc_Start(0x92b3, 0x673d))->colors;
}

void BgGfx_LoadTilemapAtPosition(u32, u32, u32);

void ScenePaletteFade_SetBackgroundOffsets(void *unused)
{
    BgGfx_LoadTilemapAtPosition(2, 8, 0x34);
    BgGfx_LoadTilemapAtPosition(3, 8, 0x34);
}

void ScenePaletteFade_BeginDelay(struct ScenePaletteFade *fade)
{
    ScenePaletteFade_SetState(fade, ScenePaletteFade_UpdateDelay);
}

void ScenePaletteFade_BeginBlend(struct ScenePaletteFade *fade)
{
    fade->frame = 0;
    ScenePaletteFade_SetState(fade, ScenePaletteFade_UpdateBlend);
}

void ScenePaletteFade_BeginHold(struct ScenePaletteFade *fade)
{
    ScenePaletteFade_SetState(fade, ScenePaletteFade_UpdateHold);
}

void ScenePaletteFade_UpdateDelay(struct ScenePaletteFade *fade)
{
    ScenePaletteFade_SetBackgroundOffsets(fade);
    fade->timer = fade->timer + 1;
    if (fade->timer >= fade->delay)
        ScenePaletteFade_BeginBlend(fade);
}

void ScenePaletteFade_UpdateBlend(struct ScenePaletteFade *fade)
{
    ScenePaletteFade_BlendPaletteBanks(gBgPaletteBuffer, fade->firstPalette, fade->secondPalette,
                                       fade->frame, fade->blendShift, 3);
    fade->frame = fade->frame + 1;
    ScenePaletteFade_SetBackgroundOffsets(fade);
    if ((fade->frame >> fade->blendShift) != 0)
        ScenePaletteFade_BeginHold(fade);
}

void ScenePaletteFade_UpdateHold(struct ScenePaletteFade *fade)
{
    ScenePaletteFade_SetBackgroundOffsets(fade);
}

void ScenePaletteFade_Update(void *arg)
{
    struct ScenePaletteFade *fade = arg;
    Callback_InvokeActor(fade, fade->update);
}

void ScenePaletteFade_Destroy(void *unused)
{
}

s32 ScenePaletteFade_InitFromScript(struct ScenePaletteFade *fade, void *unused)
{
    s32 v;
    {
        u16 *g = &gDispCntShadow;
        u32 m = 0xFFFFF0FF;
        *g = *g & m;
    }
    PaletteScreen_LoadResources(fade);
    if (Script_SeekToKeyword(0x66))
        v = Script_GetValue();
    else
        v = 6;
    fade->blendShift = v;
    v = Script_SeekToKeyword(0x73);
    if (v)
        v = Script_GetValue();
    fade->delay = v;
    fade->frame = 0;
    ScenePaletteFade_BeginDelay(fade);
    return 0;
}

void *ScenePaletteFade_Create(void *a)
{
    void *fade = Task_Create(0xb, 0x38);
    if (fade != 0) {
        Task_SetCallbacks(fade, ScenePaletteFade_Update, ScenePaletteFade_Destroy);
        if (ScenePaletteFade_InitFromScript(fade, a) < 0) {
            Task_Kill(fade);
            return 0;
        }
    }
    return fade;
}
