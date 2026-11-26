#define Anim_FindGlyphEntryById     sub_082196FC
#define Anim_RemoveAllInCurrentList sub_082195F8
#define Blend_Disable               sub_082195AC
#include "engine/video/anim_sprite_glyph.h"
#include "engine/video/render.h"
#include "engine/video/display_resource.h"

extern const u16 gUnk_0861410C[];

void Blend_SetAlpha(u32 mode, u32 destinationWeight, u32 sourceWeight)
{
    REG_BLDCNT = gUnk_0861410C[mode] | 0x40;
    REG_BLDALPHA = (destinationWeight << 8) | sourceWeight;
    REG_BLDY = 0;
}

/* Blend_Disable: Disables blending and restores default alpha and brightness register values */

void Blend_Disable(void)
{
    REG_BLDCNT = 0;
    REG_BLDALPHA = 0x1000;
    REG_BLDY = 0;
}

u32 Anim_ResetDefaultFlags(void)
{
    gDefaultAnimFlags = 0;
    return 0;
}

void Anim_UnlinkFromList(struct AnimSprite *, u32);

void Anim_Remove(struct AnimSprite *animSprite)
{
    if (animSprite->active != 0)
        Anim_UnlinkFromList(animSprite, animSprite->list);
}

/* Anim_RemoveAllInCurrentList: Walks the current animation list and removes every entry while preserving the next pointer */

void Anim_RemoveAllInCurrentList(void)
{
    struct AnimSprite *widget = gMainSpriteListHeads[gRenderListIndex];
    while (widget) {
        struct AnimSprite *next = widget->next;
        Anim_Remove(widget);
        widget = next;
    }
}

/* Protected IWRAM call veneers listed in symbols/progress_asm.csv.
 * Preserve their original Thumb/ARM fragments and embedded ARM bodies. */
INCLUDE_ASM("asm/nonmatching", sub_08219624);
INCLUDE_ASM("asm/nonmatching", sub_0821962C);
INCLUDE_ASM("asm/nonmatching", sub_08219644);
INCLUDE_ASM("asm/nonmatching", sub_0821964C);
INCLUDE_ASM("asm/nonmatching", sub_08219654);
INCLUDE_ASM("asm/nonmatching", sub_0821966C);
INCLUDE_ASM("asm/nonmatching", sub_08219674);
INCLUDE_ASM("asm/nonmatching", sub_0821967C);
INCLUDE_ASM("asm/nonmatching", sub_08219694);
INCLUDE_ASM("asm/nonmatching", sub_0821969C);
INCLUDE_ASM("asm/nonmatching", sub_082196A4);
INCLUDE_ASM("asm/nonmatching", sub_082196BC);

s32 ResourceHeader_Relocate(void *destination, void *source)
{
    struct DisplayResourceHeader *dst = destination;
    struct DisplayResourceHeader *src = source;
    *dst = *src;
    dst->words[3] += (u32)src;
    dst->words[4] += (u32)src;
    dst->words[5] += (u32)src;
    dst->words[6] += (u32)src;
    dst->words[7] += (u32)src;
    return 0;
}

struct AnimSpriteGlyph *Anim_FindGlyphEntryById(struct AnimSpriteResource *table, u16 id)
{
    struct AnimSpriteGlyph *entry = table->glyphs;
    s32 i;
    for (i = 0; i < table->glyphCount; i++) {
        if (entry->kind == id)
            return entry;
        entry++;
    }
    return NULL;
}

s32 AnimSprite_SelectGlyph(struct AnimSprite *animSprite, struct AnimSpriteResource *resource,
                           u16 index)
{
    struct AnimSpriteGlyph *entry = resource->glyphs + index;
    if (entry == 0)
        return -1;
    animSprite->glyphKind = entry->kind;
    animSprite->width = entry->width;
    animSprite->height = entry->height;
    animSprite->originX = entry->originX;
    animSprite->originY = entry->originY;
    animSprite->centerX = entry->centerX;
    animSprite->centerY = entry->centerY;
    animSprite->scaleX = 0x40;
    animSprite->scaleY = 0x40;
    animSprite->rotation = 0;
    if (animSprite->palette == 0) {
        animSprite->paletteBank = resource->paletteIndex;
        animSprite->palette = gPaletteBankData + (animSprite->paletteBank << 5);
    }
    animSprite->resourceIndex = entry->resourceIndex;
    animSprite->tileData = resource->tileBase + entry->tileOffset;
    animSprite->extraData = resource->extraData;
    return 0;
}

s32 AnimSprite_InitAtPosition(struct AnimSprite *animSprite, void *resource, u16 glyph, u32 flags,
                              u8 attributeA, u8 mode, u8 duration,
                              const struct ContactPosition *position)
{
    if (AnimSprite_SelectGlyph(animSprite, resource, glyph) < 0)
        return -1;
    animSprite->stamp = gDefaultAnimFlags;
    animSprite->flags |= flags;
    animSprite->elapsed = 0;
    animSprite->durationScale = 0x40;
    animSprite->duration = duration;
    animSprite->attribute1A = attributeA;
    animSprite->mode = mode;
    animSprite->position = *position;
    return 0;
}

s32 Anim_LoadKeyframe(struct AnimSprite *animSprite, struct AnimSpriteResource *resource,
                      u16 glyph, u8 mode)
{
    if (AnimSprite_SelectGlyph(animSprite, resource, glyph) < 0)
        return -1;
    animSprite->stamp = gDefaultAnimFlags;
    animSprite->mode = mode;
    return 0;
}

void Anim_LinkToList(void *, u32);

s32 AnimSprite_InitLinked(struct AnimSprite *animSprite, void *resource, u16 glyph, u32 flags,
                      u8 attributeA, u8 mode, u8 duration,
                      const struct ContactPosition *position)
{
    struct ContactPosition initialPosition;
    u32 zero;

    if (position == 0) {
        initialPosition.x = 0;
        initialPosition.y = 0;
        initialPosition.z = 0;
    } else
        initialPosition = *position;
    zero = animSprite->active;
    if (zero == 0) {
        animSprite->glyphKind = zero;
        animSprite->value6 = zero;
        animSprite->sequenceId = zero;
        animSprite->frameIndex = zero;
        animSprite->frameCount = zero;
        animSprite->playbackFlags = zero;
        animSprite->status = 0;
        animSprite->width = zero;
        animSprite->height = zero;
        animSprite->originX = zero;
        animSprite->originY = zero;
        animSprite->centerX = zero;
        animSprite->centerY = zero;
        animSprite->scaleX = 0;
        animSprite->scaleY = 0;
        animSprite->rotation = zero;
        animSprite->value3C = zero;
        animSprite->frames = NULL;
        animSprite->palette = zero;
        animSprite->tileData = zero;
        animSprite->extraData = zero;
        AnimSprite_InitAtPosition(animSprite, resource, glyph, flags, attributeA, mode,
                                  duration, &initialPosition);
        gDefaultAnimFlags++;
        if (gDefaultAnimFlags == 0xFFFF)
            gDefaultAnimFlags = zero;
        animSprite->cachedFrame = -1;
        animSprite->cachedPalette = -1;
        animSprite->previous = zero;
        animSprite->next = zero;
        Anim_LinkToList(animSprite, (u32) - (flags & 0x80) >> 31);
        return 0;
    }
    return -1;
}

s32 AnimSprite_InitCleared(struct AnimSprite *animSprite, void *resource, u16 glyph, u32 flags,
                       u8 attributeA, u8 mode, u8 duration, void *position)
{
    u32 zero = animSprite->active;
    if (zero != 0)
        return -1;
    else {
        animSprite->glyphKind = zero;
        animSprite->value6 = zero;
        animSprite->sequenceId = zero;
        animSprite->frameIndex = zero;
        animSprite->frameCount = zero;
        animSprite->playbackFlags = zero;
        animSprite->status = 0;
        animSprite->width = zero;
        animSprite->height = zero;
        animSprite->originX = zero;
        animSprite->originY = zero;
        animSprite->centerX = zero;
        animSprite->centerY = zero;
        animSprite->scaleX = 0;
        animSprite->scaleY = 0;
        animSprite->rotation = zero;
        animSprite->value3C = zero;
        animSprite->frames = NULL;
        animSprite->palette = zero;
        animSprite->tileData = zero;
        animSprite->extraData = zero;
        AnimSprite_InitAtPosition(animSprite, resource, glyph, flags, attributeA, mode,
                                  duration, position);
        gDefaultAnimFlags++;
        if (gDefaultAnimFlags == 0xFFFF)
            gDefaultAnimFlags = zero;
        animSprite->cachedFrame = -1;
        animSprite->cachedPalette = -1;
        animSprite->previous = 0;
        animSprite->next = 0;
        return 0;
    }
}

void Sprite_OnRenderPhase(void)
{
}

void Widget_CopyMasked(struct AnimSprite *dst, struct AnimSprite *src, u32 mask)
{
    dst->glyphKind = src->glyphKind;
    if (mask & 1)
        dst->flags = src->flags;
    dst->elapsed = src->elapsed;
    dst->duration = src->duration;
    dst->durationScale = src->durationScale;
    dst->sequenceId = src->sequenceId;
    dst->value6 = src->value6;
    dst->frameIndex = src->frameIndex;
    dst->frameCount = src->frameCount;
    dst->playbackFlags = src->playbackFlags;
    if (mask & 2)
        dst->attribute1A = src->attribute1A;
    if (mask & 4)
        dst->mode = src->mode;
    dst->status = src->status;
    if (mask & 8) {
        dst->position = src->position;
    }
    dst->width = src->width;
    dst->height = src->height;
    dst->scaleX = src->scaleX;
    dst->scaleY = src->scaleY;
    dst->rotation = src->rotation;
    dst->resourceIndex = src->resourceIndex;
    if (mask & 0x10)
        dst->paletteBank = src->paletteBank;
    dst->tileData = src->tileData;
    dst->extraData = src->extraData;
    dst->frames = src->frames;
}

void Anim_SelectPaletteBank(struct AnimSprite *animation, u32 index)
{
    s32 selected;
    animation->paletteBank = index;
    selected = animation->paletteBank;
    animation->palette = gPaletteBankData + (selected << 5);
}
