#include "character_model.h"

void ActorStats_ApplyPendingIncrement(struct ActorStats *actor)
{
    if (actor->pending) {
        if (actor->mode == 3) {
            actor->value += actor->increment;
            actor->value = actor->value > actor->limit ? actor->limit : actor->value;
        } else {
            actor->amount += actor->increment;
            actor->amount = actor->amount > actor->amountLimit ? actor->amountLimit : actor->amount;
            Character_SetLowResourceColorEffect(actor->state44, actor->state48);
        }
        actor->pending = 0;
    }
}

static inline u8 ValidIndex(s32 index)
{
    if (index < 0)
        goto invalid;
    if (index <= 1)
        goto valid;
invalid:
    return 0;
valid:
    return 1;
}

s32 ActorResource_SelectElementResource(s32 index)
{
    u32 kind;
    s32 count, selected;
    u32 mask = 0x800;
    if ((gEngineFlags | gScenePauseFlags) & mask) {
        struct AreaEffectLinkState *link = gLinkSession;
        struct AreaEffectLinkResourceSlot *slot;
        if (!link || link->unavailable != 1 || !ValidIndex(index))
            return 0x378;
        slot = (void *)(link->sessionData + (index * sizeof(*slot) + 0x40));
        if (!slot)
            return 0x378;
        kind = slot->element;
    } else {
        count = 0;
        if (GameState_HasEquippedConsumable(10)) {
            selected = 2;
            count = 1;
        }
        if (GameState_HasEquippedConsumable(26)) {
            selected = 3;
            count++;
        }
        if (GameState_HasEquippedConsumable(37)) {
            selected = 4;
            count++;
        }
        if (GameState_HasEquippedConsumable(47)) {
            selected = 5;
            count++;
        }
        kind = 1;
        if (count <= 3) {
            kind = 0;
            if (count == 1)
                kind = selected;
        }
    }
    switch (kind) {
        case 1:
            return 0x10e;
        case 2:
            return 0x10b;
        case 3:
            return 0x10d;
        case 4:
            return 0x10c;
        case 5:
            return 0x10a;
        case 0:
        default:
            return 0x378;
    }
}

s32 CharacterModel_GetPrimaryPaletteIndex(struct CharacterPreviewModel *model)
{
    s32 paletteIndex;
    switch (model->view.metadata.kind) {
        case 0:
            paletteIndex = ActorResource_SelectElementResource(model->playerIndex);
            break;
        case 1:
            paletteIndex = 0x106;
            break;
        case 2:
            paletteIndex = 0x107;
            break;
        case 3:
            paletteIndex = 0x108;
            break;
        case 4:
        case 5:
            paletteIndex = 0x109;
            break;
        default:
            paletteIndex = 0x378;
            break;
    }
    return paletteIndex;
}

void CharacterModel_LoadPrimaryPalette(struct CharacterPreviewModel *model)
{
    s32 paletteIndex = CharacterModel_GetPrimaryPaletteIndex(model);
    CpuSet(gPaletteBankData + (paletteIndex << 5), model->activePalette, 0x10);
}

void CharacterModel_LoadSecondaryPalette(struct CharacterPreviewModel *model, s32 appearance)
{
    u8 **paletteBank;
    u32 paletteIndex = appearance + 0x379;
    paletteBank = &gPaletteBankData;
    CpuSet(*paletteBank + (paletteIndex << 5), model->normalPalette, 0x10);
}

void CharacterModel_InitPalettes(struct CharacterPreviewModel *model, s32 appearance)
{
    u8 *primary, *secondary;
    u16 *palette;
    struct ModifierDisplayObject *part;
    s32 partIndex;
    u32 tile;
    primary = gPaletteBankData + (CharacterModel_GetPrimaryPaletteIndex(model) << 5);
    secondary = gPaletteBankData + ((appearance + 0x379) << 5);
    palette = model->activePalette;
    CpuSet(primary, palette, 0x10);
    CpuSet(secondary, model->normalPalette, 0x10);
    part = model->view.parts;
    tile = 0xa000;
    partIndex = 4;
    do {
        part->palette = palette;
        part->tile = tile;
        part++;
        partIndex--;
    } while (partIndex >= 0);
}

void CharacterPreview_InitializePalette(struct CharacterPreviewPalette *paletteState,
                                        struct CharacterPreviewModel *model, s32 index)
{
    u16 *palette;
    s32 resource;
    s32 offset, partIndex;
    struct CharacterPreviewPalette *flash = paletteState;
    paletteState->target = model;
    flash->flashPalette = gPaletteBankData + 0x3240;
    switch (model->view.metadata.kind) {
        case 0:
            resource = CharacterModel_GetPrimaryPaletteIndex(model);
            break;
        case 1:
            resource = 0x106;
            break;
        case 2:
            resource = 0x107;
            break;
        case 3:
            resource = 0x108;
            break;
        case 4:
        case 5:
            resource = 0x109;
            break;
        default:
            resource = 0x378;
            break;
    }
    paletteState->paletteId = index + 0x379;
    {
        const u8 *source = gPaletteBankData + (resource << 5);
        paletteState->normalPalette = gPaletteBankData + (paletteState->paletteId << 5);
        palette = model->activePalette;
        CpuSet(source, palette, 16);
    }
    CpuSet(paletteState->normalPalette, model->normalPalette, 16);
    {
        struct TilemapPanel **allocator = &gSceneCamera;
        struct TilemapPanel **tileAllocator;
        tileAllocator = allocator;
        offset = 0;
        partIndex = 4;
        do {
            struct ModifierDisplayObject *part =
                (void *)((u8 *)paletteState->target->view.parts + offset);
            part->palette = model->activePalette;
            part->tile = (*tileAllocator)->value38;
            offset += sizeof(*part);
        } while (--partIndex >= 0);
        (*allocator)->value38 = (*allocator)->value38 + (offset = 2);
        if ((*allocator)->value38 > 0xafff) {
            (*allocator)->value38 = 0xa000;
        } else if ((*allocator)->value38 <= 0x9fff) {
            (*allocator)->value38 = 0xa000;
        }
    }
}

void PaletteFlash_SelectPalette(struct CharacterPreviewPalette *flash, s32 mode)
{
    struct CharacterPreviewModel *target = flash->target;
    switch (mode) {
        case 0:
            CpuSet(flash->normalPalette, target->normalPalette, 16);
            flash->framesRemaining = mode;
            break;
        case 1:
            CpuSet(flash->flashPalette, target->normalPalette, 16);
            flash->framesRemaining = 8;
            break;
    }
}

void Character_UpdateTimedEffect(struct CharacterPreviewPalette *effect)
{
    if (effect->framesRemaining != 0) {
        effect->framesRemaining--;
        if (effect->framesRemaining == 0)
            PaletteFlash_SelectPalette(effect, 0);
    }
}

void ActorSprite_InitPartDrawings(struct ScenePoolActor *actor, struct CharacterPreviewModel *model,
                                  u32 id)
{
    struct {
        s16 x, y, z;
    } projected, *position;
    s32 dx, dy;
    s32 frames;
    s32 partIndex;
    s32 anim;
    u8 animId;
    s32 z;
    struct ModifierDisplayObject *part;
    void *resource;

    model->data = ScriptProc_Start(0xCB05, (u16)id);
    ResourceHeader_Relocate(&model->resource, model->data);
    frames = 0;
    for (partIndex = 0; partIndex < 5; partIndex++) {
        dx = 0;
        dy = 0;
        switch (partIndex) {
            case 0:
                anim = actor->bodyAnimation;
                frames = 16;
                break;
            case 3:
                anim = actor->accessoryAnimation;
                dx = 15;
                dy = -2;
                frames = 16;
                break;
            case 4:
                anim = actor->accessoryAnimation;
                dx = -19;
                dy = -2;
                frames = 16;
                break;
            case 1:
                anim = actor->faceAnimation;
                frames = 16;
                break;
            case 2:
                ScenePalette_RefreshOnFlags(actor, model);
                anim = actor->paletteMode;
                frames = 16;
                break;
            default:
                anim = 0;
                break;
        }
        part = &model->view.parts[partIndex];
        resource = &model->resource;
        animId = anim;
        if (Widget_InitLinked(part, resource, animId, frames, 0, 1, 0, 0) != 0)
            return;
        Anim_SetKeyframes(part, resource, animId, 1, 0);
        Anim_Step(part, resource);
        position = &projected;
        position->x = (actor->position.x >> 12) - ((struct TilemapPanel *)gSceneCamera)->scroll.x;
        position->y =
            ((actor->position.z + (actor->position.y + actor->restrictionMetric)) >> 12) + 0x5A;
        position->z = z = actor->position.z >> 12;
        position->x += dx;
        position->y += dy;
        part->x = position->x;
        part->y = position->y;
        part->z = position->z;
        part->attribute1A = 2;
    }
    Actor_SetSpriteDepthOffsets(model, z);
}

void CharacterPreview_PositionParts(void *resource, struct ModifierDisplayObject *parts,
                                    struct ModifierPreviewOrigin *origin, u32 flags)
{
    s32 i = 0;
    u32 flip = flags & 4;
    for (; i <= 4; i++) {
        s32 x = 0;
        s32 y = 0;
        switch (i) {
            case 3:
                x = 15;
                y = -2;
                break;
            case 4:
                x = -19;
                y = -2;
                break;
        }
        if (flip)
            x = -x;
        parts[i].x = origin->x + x;
        parts[i].y = origin->y + y;
    }
}

static inline u8 HasPaletteRefreshFlag(void)
{
    u32 mask = 0x100;
    if (gGameState->flags868 & mask)
        return TRUE;
    return FALSE;
}

static inline u32 TestPaletteModeFlags(u32 mask)
{
    return (gEngineFlags | gScenePauseFlags) & mask;
}

void ScenePalette_RefreshOnFlags(struct ScenePoolActor *actor, struct CharacterPreviewModel *model)
{
    if (HasPaletteRefreshFlag() || TestPaletteModeFlags(0x800)) {
        if (actor->paletteState == 11)
            actor->paletteMode = 3;
        else
            actor->paletteMode = 0;
        CharacterModel_LoadPrimaryPalette(model);
    }
}

void CharacterModel_SetComponentKeyframes(struct ScenePoolActor *actor,
                                          struct CharacterPreviewModel *model)
{
    s32 partIndex = 0;
    do {
        u32 animation;
        struct ModifierDisplayObject *part;
        struct ModifierGraphicsResource *resource;
        switch (partIndex) {
            case 0:
                animation = actor->bodyAnimation;
                break;
            case 3:
            case 4:
                animation = actor->accessoryAnimation;
                break;
            case 1:
                animation = actor->faceAnimation;
                break;
            case 2:
                animation = actor->paletteMode;
                break;
            default:
                animation = 0;
                break;
        }
        part = &model->view.parts[partIndex];
        resource = &model->resource;
        Anim_SetKeyframes(part, resource, animation, 1, 0);
        Anim_Step(part, resource);
        partIndex++;
    } while (partIndex <= 4);
}

void CharacterModel_UpdateAccessoryAnimations(struct CharacterPreviewModel *model)
{
    struct ModifierGraphicsResource *resource;
    Anim_Step(&model->view.parts[2], resource = &model->resource);
    Anim_Step(&model->view.parts[3], resource);
    Anim_Step(&model->view.parts[4], resource);
}

void CharacterModel_UpdatePairedAnimations(struct CharacterPreviewModel *model)
{
    struct ModifierGraphicsResource *resource;
    Anim_Step(&model->view.parts[3], resource = &model->resource);
    Anim_Step(&model->view.parts[4], resource);
}

void CharacterModel_RemoveAnimations(struct ModifierDisplayObject *part)
{
    s32 partIndex = 4;
    do {
        Anim_Remove(part);
        part++;
    } while (--partIndex >= 0);
}

void CharacterModel_SetAnimationFlags(struct CharacterPreviewModel *model, u32 mask)
{
    u32 *flags = &model->view.parts[0].flags;
    s32 partIndex = 4;
    do {
        *flags |= mask;
        flags += sizeof(struct ModifierDisplayObject) / sizeof(*flags);
    } while (--partIndex >= 0);
}

void CharacterModel_ClearAnimationFlags(struct CharacterPreviewModel *model, u32 mask)
{
    u32 *flags;
    s32 partIndex;
    mask = ~mask;
    flags = &model->view.parts[0].flags;
    partIndex = 4;
    do {
        *flags &= mask;
        flags += sizeof(struct ModifierDisplayObject) / sizeof(*flags);
    } while (--partIndex >= 0);
}

void CharacterModel_SetComponentRenderMode(struct ModifierDisplayObject *parts, u32 mode)
{
    struct ModifierDisplayObject *part = &parts[4];
    do {
        part->attribute1A = mode;
        part--;
    } while ((s32)part >= (s32)parts);
}

/* ScenePool_ApplyMotionInput: keep the address symbol for the external motion caller. */

s32 sub_0811CD70(struct ScenePoolActor *actor, u32 input)
{
    s32 accelerating = 0;
    s32 braking;

    actor->requestedSound = 0x88;
    if (actor->motionLock > 0)
        return 0;
    if (actor->verticalSpeed >= 0 && (actor->restrictionMetric >> 12) == 0)
        actor->airborne = 0;
    else
        actor->airborne = 1;
    if (actor->airborne)
        return accelerating;
    if (input & 0x200)
        Modifier_StartDrainIfAboveThreshold(actor);
    if ((actor->boundsFlags & 0x200) && actor->biasClampMode != 2) {
        if (!(actor->inputFlags & 0x200))
            Sound_PlaySong(0x8E);
        return accelerating;
    }
    if ((actor->boundsFlags & 0x400) && actor->biasClampMode != 1) {
        if (!(actor->inputFlags & 0x400))
            Sound_PlaySong(0x8F);
        braking = 1;
        actor->forwardSpeed = (s32)(actor->lateralSpeed << 3) >> 4;
        actor->lateralSpeed = (s32)(actor->lateralSpeed << 3) >> 4;
    } else {
        braking = 0;
    }
    if (input & 0x100) {
        if (!braking) {
            actor->forwardSpeed += ModifierStats_ApplyKind0(actor, actor->acceleration);
            accelerating = 1;
        }
        actor->requestedSound = 0x89;
    }
    if (input & 0x40)
        actor->lateralSpeed -= actor->turnAcceleration >> braking;
    else if (input & 0x80)
        actor->lateralSpeed += actor->turnAcceleration >> braking;
    if (braking) {
        if (input & 0x10)
            actor->steering += 0x400;
        else if (input & 0x20)
            actor->steering -= 0x400;
    } else {
        if (input & 0x10)
            actor->steering += 0x1000;
        else if (input & 0x20)
            actor->steering -= 0x1000;
    }
    return accelerating;
}

void CharacterModel_SetPrimaryAppearance(struct CharacterPreviewModel *model, u32 appearance)
{
    model->primaryAppearance = SceneSlot_RegisterObject(appearance);
}

void CharacterModel_SetSecondaryAppearance(struct CharacterPreviewModel *model, u32 appearance,
                                           u32 variant)
{
    model->secondaryAppearance = DisplaySlots_Reserve(appearance, variant);
}

void ScenePool_ClampForwardBias(struct ScenePoolActor *s)
{
    if (s->forwardBias > s->maximumForwardBias)
        s->forwardBias = s->maximumForwardBias;
    if (s->forwardBias < s->minimumForwardBias)
        s->forwardBias = s->minimumForwardBias;
    if ((s->boundsFlags & 0x200) && s->biasClampMode != 2)
        s->forwardBias = s->minimumForwardBias;
}

void ScenePool_ClampMotionSpeeds(struct ScenePoolActor *s)
{
    s32 maximum = ModifierStats_ApplyKind1(s, s->maximumForwardSpeed);
    if (s->forwardSpeed > maximum)
        s->forwardSpeed = maximum;
    if (s->forwardSpeed < s->minimumForwardSpeed)
        s->forwardSpeed = s->minimumForwardSpeed;
    if (s->lateralSpeed > s->maximumLateralSpeed)
        s->lateralSpeed = s->maximumLateralSpeed;
    if (s->lateralSpeed < s->minimumLateralSpeed)
        s->lateralSpeed = s->minimumLateralSpeed;
    ScenePool_ClampForwardBias(s);
}

void Character_GetHeightAdjustedPosition(struct ScenePoolActor *actor, s16 *position)
{
    position[0] = actor->position.x >> 12;
    position[1] = (actor->position.y + actor->restrictionMetric) >> 12;
    position[2] = actor->position.z >> 12;
}

void Position_GetHeightAdjustedCoordinates(s32 *coordinates, s32 height, s16 *position)
{
    position[0] = coordinates[0] >> 12;
    position[1] = (coordinates[1] + height) >> 12;
    position[2] = coordinates[2] >> 12;
}

void Character_UpdateLoopingSound(struct ScenePoolActor *actor)
{
    u16 *playing = &actor->playingSound;
    u16 *requested = &actor->requestedSound;
    if (*playing != *requested) {
        Sound_StopSong(0x87);
        Sound_StopSong(0x88);
        Sound_StopSong(0x89);
        Sound_StopSong(0x8a);
        Sound_StopSong(0x8b);
        Sound_PlaySong(*requested);
        *playing = *requested;
    }
}

static inline u8 IsMotionKind(u32 kind, u32 expected)
{
    return kind == expected;
}

void SceneMotion_PlayKindSound(struct ScenePoolActor *actor)
{
    if (IsMotionKind(actor->kind, 4) || IsMotionKind(actor->kind, 3))
        Sound_PlaySong(0x95);
    else if (IsMotionKind(actor->kind, 5))
        Sound_PlaySong(0x13e);
    else
        Sound_PlaySong(0x91);
}

static inline u8 HasPendingDamage(struct ActorStats *actor)
{
    u32 mask = 1;
    if (actor->damageFlags & mask)
        return 1;
    return 0;
}

static inline u32 ClearDamageFlag(u8 *flags, u32 mask)
{
    u32 value = ~mask;
    value &= *flags;
    return value;
}

static inline u32 ReadDamageFlags(u8 *flags)
{
    return *flags;
}

static inline void SetDamageFlag(u8 *flags, u32 mask)
{
    u32 value = mask;
    value |= ReadDamageFlags(flags);
    *flags = value;
}

void ActorStats_ApplyPendingDamage(struct ActorStats *actor)
{
    if (HasPendingDamage(actor)) {
        u32 cleared = ClearDamageFlag(&actor->damageFlags, 1);
        u32 zero = 0;
        actor->damageFlags = cleared;
        PaletteFlash_SelectPalette((void *)&actor->pendingDamage, 1);
        actor->amount -= ModifierStats_ApplyKinds4And14(actor, actor->pendingDamage);
        actor->damageTimer = zero;
        actor->pendingDamage = zero;
        Character_SetLowResourceColorEffect(actor->state44, actor->state48);
        if (actor->amount <= 0)
            SetDamageFlag(&actor->damageFlags, 2);
    }
}

void Character_OnActionSoundCue(void)
{
    Sound_PlaySong(0x94);
}

void ModifierMenu_InitSelectionSnapshot(struct CharacterPaletteCycle *dest,
                                        struct CharacterPaletteSource *src)
{
    CpuSet(src->colors, dest, 0x04000008);
    dest->timer = 0;
}

void PaletteFade_CycleMenuColors(struct CharacterPaletteCycle *cycle)
{
    u16 *colors = cycle->colors;
    if (cycle->timer <= 1) {
        cycle->timer++;
    } else {
        u16 lastColor;
        u16 *color;
        s32 remaining;
        u16 *destination;
        cycle->timer = 0;
        lastColor = colors[7];
        remaining = 7;
        destination = gBgPaletteBank1;
        color = colors + 6;
        do {
            color[1] = color[0];
            color--;
            remaining--;
        } while (remaining > 1);
        colors[1] = lastColor;
        CpuSet(colors, destination, 0x04000008);
    }
}

u8 *GameState_GetBikeName(void)
{
    return &gGameState->playerName[16];
}

void GameState_SetNameFromScript(void)
{
    if (Script_SeekToKeyword('n')) {
        const u8 *text;
        Memory_Clear(&gGameState->playerName[16], 16);
        text = Script_ReadString(Script_GetPc());
        if (text) {
            s32 i = 0;
            u8 *name = &gGameState->playerName[16];
            do {
                u8 c = text[i];
                if (!c)
                    break;
                name[i] = c;
                i++;
            } while (i <= 10);
            name[i] = 0;
        }
    }
}

void GameState_EnableAlternateHud(void)
{
    u32 mask = 0x80;
    gGameState->flags868 |= mask;
}

void GameState_SetRuntimeFlag100(void)
{
    u32 mask = 0x100;
    gGameState->flags868 |= mask;
}

u8 *Video_GetWrappedBackgroundMapEntry(u32 layerIndex, u32 x, u32 y)
{
    struct BackgroundLayer *layer = &gBgRecords[layerIndex];
    u8 *base = (u8 *)layer->map;

    return base + ((x & 0x1F) << 1) + ((y & 0x1F) << 6);
}

void PanelText_DrawDigits(s32 value, s32 divisor, u32 x, u32 y, s32 style)
{
    u16 *cell;
    s32 base = 0x21;
    if (!style)
        base = 0x11;
    cell = (u16 *)Video_GetWrappedBackgroundMapEntry(3, x, y);
    if (divisor > 0) {
        do {
            s32 digit = Div(value, divisor);
            *cell++ = digit + base;
            do {
                value -= digit * divisor;
            } while (0);
            divisor = Div(divisor, 10);
        } while (divisor > 0);
    }
}

void Video_DrawBackgroundRowMarkers(u32 x, u32 y)
{
    u16 *cell = (u16 *)Video_GetWrappedBackgroundMapEntry(3, x, y);
    s32 i;
    for (i = 0; i <= 6; i++) {
        if (i != 3)
            *cell = 0x10;
        cell++;
    }
    *cell = 1;
}

void CharacterModel_LoadActorAppearance(struct ScenePoolActor *actor)
{
    struct GameState *state = gGameState;
    actor->bodyAnimation = *(u8 *)&state->unk7A0[2] + 12;
    actor->paletteMode = state->styleParameterE;
    actor->accessoryAnimation = *(u8 *)&state->unk7A0[1] + 18;
    actor->faceAnimation = *(u8 *)&state->unk7A0[0] + 6;
}

s32 ModifierMenu_CountAppearanceModifiers(struct SceneActorStyle *style)
{
    s32 count = 0;
    s32 entryIndex = 0;
    do {
        if ((s8)style->entries[entryIndex] < 0)
            break;
        count++;
        entryIndex++;
    } while (entryIndex <= 3);
    return count;
}

void GameState_ClearSlots7A8FromScript(void)
{
    s32 i;
    for (i = 0; i <= 3; i++)
        ((s16 *)gGameState->modifierIdsBytes)[i] = -1;
}

void SceneActorStyle_InitializeFromGameState(struct SceneActorStyle *style)
{
    s32 i;
    u32 zero;
    struct GameState *state = gGameState;
    style->adjustedA = *(u8 *)&state->unk7A0[2] + 12;
    style->adjustedB = *(u8 *)&state->unk7A0[0] + 6;
    style->adjustedC = *(u8 *)&state->unk7A0[1] + 18;
    style->parameterE = state->styleParameterE;
    for (i = 0; i <= 3; i++)
        style->entries[i] = ((u16 *)state->modifierIdsBytes)[i];
    zero = 0;
    style->count = ModifierMenu_CountAppearanceModifiers(style);
    state = gGameState;
    style->parameterD = (u16)state->unk81A;
    style->parameterB = state->unk7A0[0];
    style->parameterF = state->unk7A0[3];
    style->parameterC = state->unk7A0[1];
    style->parameterA = state->unk7A0[2];
    {
        u16 initial;
        state = (void *)&state->unk800[12];
        initial = *(u16 *)state;
        style->initialHalfword = initial;
        style->currentHalfword = initial;
    }
    style->initialWord = zero;
    style->currentWord = zero;
}

void SceneActorStyle_Initialize(struct SceneActorStyle *style, s32 body, s32 face, s32 accessory,
                                s32 paletteMode, s32 paletteState, s32 styleVariant,
                                s32 initialHalfword, u32 initialWord)
{
    s32 i;
    u8 *entry;
    style->adjustedA = body + 12;
    style->adjustedB = face + 6;
    style->adjustedC = accessory + 18;
    style->parameterE = paletteMode;
    {
        u32 zero = 0;
        i = 3;
        entry = &style->entries[3];
        do {
            *entry-- = zero;
        } while (--i >= 0);
    }
    style->count = 0;
    style->parameterD = paletteState;
    style->parameterB = face;
    style->parameterF = styleVariant;
    style->parameterC = accessory;
    style->parameterA = body;
    style->initialHalfword = initialHalfword;
    style->currentHalfword = initialHalfword;
    style->initialWord = initialWord;
    style->currentWord = initialWord;
}
