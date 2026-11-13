#define Anim_FindGlyphEntryById     sub_082196FC
#define Anim_RemoveAllInCurrentList sub_082195F8
#define Blend_Disable               sub_082195AC
#include "animation_widget.h"
#include "render.h"
#include "display_resource.h"

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

void Anim_UnlinkFromList(struct SceneInputWidget *, u32);

void Anim_Remove(struct SceneInputWidget *widget)
{
    if (widget->unknown4[0] != 0)
        Anim_UnlinkFromList(widget, widget->list);
}

/* Anim_RemoveAllInCurrentList: Walks the current animation list and removes every entry while preserving the next pointer */

void Anim_RemoveAllInCurrentList(void)
{
    struct SceneInputWidget *widget = gMainSpriteListHeads[gRenderListIndex];
    while (widget) {
        struct SceneInputWidget *next = widget->next;
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

struct WidgetGlyphEntry *Anim_FindGlyphEntryById(struct WidgetGlyphResource *table, u16 id)
{
    struct WidgetGlyphEntry *entry = table->glyphs;
    s32 i;
    for (i = 0; i < table->glyphCount; i++) {
        if (entry->kind == id)
            return entry;
        entry++;
    }
    return NULL;
}

s32 Anim_SelectWidgetGlyph(struct SceneInputWidget *widget, struct WidgetGlyphResource *resource,
                           u16 index)
{
    struct WidgetGlyphEntry *entry = resource->glyphs + index;
    u32 zero;
    if (entry == 0)
        return -1;
    widget->glyphKind = entry->kind;
    zero = 0;
    widget->width = entry->width;
    widget->height = entry->height;
    widget->originX = entry->originX;
    widget->originY = entry->originY;
    widget->centerX = entry->centerX;
    widget->centerY = entry->centerY;
    widget->scaleX = 0x40;
    widget->scaleY = 0x40;
    widget->rotation = zero;
    if (widget->palette == 0) {
        widget->paletteBank = resource->paletteIndex;
        widget->palette = gPaletteBankData + (widget->paletteBank << 5);
    }
    widget->resourceIndex = entry->resourceIndex;
    widget->tileData = resource->tileBase + entry->tileOffset;
    widget->extraData = resource->extraData;
    return 0;
}

s32 Anim_InitPositionedWidget(struct SceneInputWidget *widget, void *resource, u16 glyph, u32 flags,
                              u8 attributeA, u8 attributeB, u8 attributeC,
                              const struct ContactPosition *position)
{
    if (Anim_SelectWidgetGlyph(widget, resource, glyph) < 0)
        return -1;
    widget->stamp = gDefaultAnimFlags;
    widget->flags |= flags;
    widget->fieldC = 0;
    widget->scale = 0x40;
    widget->attributeE = attributeC;
    widget->attribute1A = attributeA;
    widget->attribute1B = attributeB;
    widget->position = *position;
    return 0;
}

s32 Anim_LoadKeyframe(struct SceneInputWidget *widget, struct WidgetGlyphResource *resource,
                      u16 glyph, u8 attribute)
{
    if (Anim_SelectWidgetGlyph(widget, resource, glyph) < 0)
        return -1;
    widget->stamp = gDefaultAnimFlags;
    widget->attribute1B = attribute;
    return 0;
}

void Anim_LinkToList(void *, u32);

s32 Widget_InitLinked(struct SceneInputWidget *widget, void *resource, u16 glyph, u32 flags,
                      u8 attributeA, u8 attributeB, u8 attributeC,
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
    zero = widget->unknown4[0];
    if (zero == 0) {
        widget->glyphKind = zero;
        widget->value6 = zero;
        widget->frame = zero;
        widget->frameTimer = zero;
        widget->frameDuration = zero;
        widget->value18 = zero;
        widget->status = 0;
        widget->width = zero;
        widget->height = zero;
        widget->originX = zero;
        widget->originY = zero;
        widget->centerX = zero;
        widget->centerY = zero;
        widget->scaleX = 0;
        widget->scaleY = 0;
        widget->rotation = zero;
        widget->value3C = zero;
        widget->renderData = zero;
        widget->palette = zero;
        widget->tileData = zero;
        widget->extraData = zero;
        Anim_InitPositionedWidget(widget, resource, glyph, flags, attributeA, attributeB,
                                  attributeC, &initialPosition);
        gDefaultAnimFlags++;
        if (gDefaultAnimFlags == 0xFFFF)
            gDefaultAnimFlags = zero;
        widget->cachedFrame = -1;
        widget->cachedPalette = -1;
        widget->previous = zero;
        widget->next = zero;
        Anim_LinkToList(widget, (u32) - (flags & 0x80) >> 31);
        return 0;
    }
    return -1;
}

s32 Widget_InitCleared(struct SceneInputWidget *widget, void *resource, u16 glyph, u32 flags,
                       u8 attributeA, u8 attributeB, u8 attributeC, void *position)
{
    u32 zero = widget->unknown4[0];
    if (zero != 0)
        return -1;
    else {
        widget->glyphKind = zero;
        widget->value6 = zero;
        widget->frame = zero;
        widget->frameTimer = zero;
        widget->frameDuration = zero;
        widget->value18 = zero;
        widget->status = 0;
        widget->width = zero;
        widget->height = zero;
        widget->originX = zero;
        widget->originY = zero;
        widget->centerX = zero;
        widget->centerY = zero;
        widget->scaleX = 0;
        widget->scaleY = 0;
        widget->rotation = zero;
        widget->value3C = zero;
        widget->renderData = zero;
        widget->palette = zero;
        widget->tileData = zero;
        widget->extraData = zero;
        Anim_InitPositionedWidget(widget, resource, glyph, flags, attributeA, attributeB,
                                  attributeC, position);
        gDefaultAnimFlags++;
        if (gDefaultAnimFlags == 0xFFFF)
            gDefaultAnimFlags = zero;
        widget->cachedFrame = -1;
        widget->cachedPalette = -1;
        widget->previous = 0;
        widget->next = 0;
        return 0;
    }
}

void Sprite_OnRenderPhase(void)
{
}

void Widget_CopyMasked(struct SceneInputWidget *dst, struct SceneInputWidget *src, u32 mask)
{
    dst->glyphKind = src->glyphKind;
    if (mask & 1)
        dst->flags = src->flags;
    dst->fieldC = src->fieldC;
    dst->attributeE = src->attributeE;
    dst->scale = src->scale;
    dst->frame = src->frame;
    dst->value6 = src->value6;
    dst->frameTimer = src->frameTimer;
    dst->frameDuration = src->frameDuration;
    dst->value18 = src->value18;
    if (mask & 2)
        dst->attribute1A = src->attribute1A;
    if (mask & 4)
        dst->attribute1B = src->attribute1B;
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
    dst->renderData = src->renderData;
}

void Anim_SelectPaletteBank(struct SceneInputWidget *animation, u32 index)
{
    s32 selected;
    animation->paletteBank = index;
    selected = animation->paletteBank;
    animation->palette = gPaletteBankData + (selected << 5);
}
