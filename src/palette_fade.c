#include "palette_fade.h"
#include "scene_pool.h"
#include "background_controller.h"
#include "render.h"
#include "ram.h"
#include "engine.h"

static inline u8 IsPlayerScrollBlocked(void)
{
    if (gParticipantActors[0]) {
        u32 flags = gParticipantActors[0]->flags;
        if (flags & 5)
            return 1;
    }
    return 0;
}

/* Advance scanline phases at the selected speed above or below the split. */

void ScanlineScroll_AdvancePhases(struct PaletteFade *fade)
{
    s32 i;
    s32 speed;
    u16 topPhase;
    u16 bottomPhase;
    if (IsPlayerScrollBlocked())
        return;
    for (i = 0; i < fade->scanline.wave.split; i++) {
        speed = fade->scanline.wave.speedTop;
        topPhase = (fade->phases[i] + speed) & 0xFFF;
        fade->phases[i] = topPhase;
        fade->offsets[i] = fade->phases[i] >> 7;
    }

    for (; i < 160; i++) {
        speed = fade->scanline.wave.speedBottom;
        bottomPhase = (fade->phases[i] + speed) & 0xFFF;
        fade->phases[i] = bottomPhase;
        fade->offsets[i] = fade->phases[i] >> 7;
    }
}

void ScanlineEffect_SetSource(s32, s32, void *);

void ScanlineScroll_Update(struct PaletteFade *fade)
{
    if (fade->lastMapX != gBgRecords[3].x)
        BgGfx_LoadTilemapSimple(3, gBgRecords[3].x >> 3, 0, 0, 0);
    fade->lastMapX = gBgRecords[3].x;
    ScanlineScroll_AdvancePhases(fade);
    ScanlineEffect_SetSource(3, 0, fade->offsets);
    if (fade->moving && !IsPlayerScrollBlocked()) {
        fade->scroll += fade->speed;
        if ((fade->scroll >> 7) > 256)
            fade->scroll = 0x8000;
        fade->remaining--;
        if (fade->remaining <= 0)
            fade->moving = 0;
    }
    gBgRecords[fade->layer].x = fade->scroll >> 7;
}

void BackgroundScroll_UpdateHorizontal(struct PaletteFade *fade)
{
    fade->scroll += fade->horizontalSpeed;
    gBgRecords[fade->layer].x = fade->scroll >> 7;
}

u32 PaletteFade_OnTaskUpdate(struct PaletteFade *fade)
{
    if (fade->updateScroll)
        fade->updateScroll(fade);
    PaletteFade_UpdateTransition(fade);
    return 0;
}

void sub_082194D4(void);

u32 PaletteFade_OnTaskDestroy(struct PaletteFade *fade)
{
    if (fade->mode & 1)
        sub_082194D4();
    return gPaletteFade = 0;
}

void PaletteFade_AdvanceColors(struct PaletteFade *fade)
{
    s32 i;
    for (i = 0; i < 16; i++) {
        fade->red[fade->slot][i] += fade->deltaR[fade->slot][i];
        fade->green[fade->slot][i] += fade->deltaG[fade->slot][i];
        fade->blue[fade->slot][i] += fade->deltaB[fade->slot][i];
        {
            u32 destination = i * 2;
            destination += fade->slot * 32;
            destination += (u32)gBgPaletteBuffer;
            *(u16 *)destination = (fade->red[fade->slot][i] >> 24) |
                                  ((fade->green[fade->slot][i] >> 24) << 5) |
                                  ((fade->blue[fade->slot][i] >> 24) << 10);
        }
    }
}

void PaletteFade_UpdateTransition(struct PaletteFade *fade)
{
    if (fade->flags & 1) {
        if (fade->remainingFrames <= 0) {
            fade->flags &= ~1;
            if (fade->remainingTargets > 0)
                PaletteFade_BeginNextTarget(fade);
        } else {
            PaletteFade_AdvanceColors(fade);
            fade->remainingFrames--;
        }
    }
}

void PaletteFade_SetTransitionParameters(struct PaletteFade *fade, u32 slot, u32 target,
                                         u32 targetCount, u32 duration)
{
    fade->slot = slot;
    fade->target = target;
    fade->remainingTargets = targetCount;
    fade->duration = duration;
    fade->remainingFrames = 0;
    fade->flags = 0;
}

static inline struct PaletteFade *PaletteFade_GetOwner(struct PaletteFade **owner)
{
    return *owner;
}

union PaletteChannelValue {
    u32 full;
    u16 value;
};

#define PALETTE_FADE_CHANNEL(color, shift, result, channelMask)                                    \
    ((result).full = 0, (result).value = ((color) << 16) >> (shift) & (channelMask), (result).full)

void PaletteFade_BeginNextTarget(struct PaletteFade *fade)
{
    struct PaletteFade *owner = fade;
    s32 fromOffset = fade->slot * 32;
    u8 *fromBase = (u8 *)gBgPaletteBuffer;
    u16 *from;
    u16 *to = fade->resources.banks.targets;
    s32 i;
    to += fade->target * 16;
    fade->remainingFrames = fade->duration;
    for (i = 0; i < 16; i++) {
        u32 targetColor;
        union PaletteChannelValue targetChannel, sourceChannel, currentChannel;
        u32 sourceColor;
        u32 current;
        u16 targetRed, sourceRed, currentRed;
        fade->deltaR[fade->slot][i] =
            (targetColor = to[i], targetRed = 31, targetRed &= targetColor,
             sourceColor = *(from = (u16 *)(fromBase + (fromOffset + i * 2))), sourceRed = 31,
             sourceRed &= sourceColor, (targetRed - sourceRed) << 24);
        {
            const u16 mask[1] = {31};
            PaletteFade_GetOwner(&owner)->deltaG[fade->slot][i] =
                (PALETTE_FADE_CHANNEL(targetColor, 21, targetChannel, mask[0]),
                 PALETTE_FADE_CHANNEL(sourceColor, 21, sourceChannel, mask[0]),
                 targetChannel.full - sourceChannel.full)
                << 24;
            fade->deltaB[fade->slot][i] =
                (PALETTE_FADE_CHANNEL(targetColor, 26, targetChannel, mask[0]),
                 PALETTE_FADE_CHANNEL(sourceColor, 26, sourceChannel, mask[0]),
                 targetChannel.full - sourceChannel.full)
                << 24;
            fade->deltaR[fade->slot][i] = Div(fade->deltaR[fade->slot][i], fade->duration);
            PaletteFade_GetOwner(&owner)->deltaG[fade->slot][i] =
                Div(PaletteFade_GetOwner(&owner)->deltaG[fade->slot][i], fade->duration);
            fade->deltaB[fade->slot][i] = Div(fade->deltaB[fade->slot][i], fade->duration);
            fade->red[fade->slot][i] =
                (current = *from, currentRed = 31, currentRed &= current, currentRed << 24);
            fade->green[fade->slot][i] = PALETTE_FADE_CHANNEL(current, 21, currentChannel, mask[0])
                                         << 24;
            fade->blue[fade->slot][i] = PALETTE_FADE_CHANNEL(current, 26, currentChannel, mask[0])
                                        << 24;
        }
    }
    if (fade->remainingTargets >= 0) {
        fade->remainingTargets--;
        fade->target++;
        fade->flags |= 1;
    }
}

void PaletteFade_LoadPaletteResources(struct PaletteFade *fade, u32 sourceId, u32 targetId)
{
    s32 i;
    for (i = 0; i <= 1; i++) {
        struct PaletteFadePaletteResource *resource = NULL;
        switch (i) {
            case 0:
                resource = ScriptProc_Start(0x92B3, sourceId);
                break;
            case 1:
                resource = ScriptProc_Start(0x92B3, targetId);
                break;
        }
        fade->resources.palettes[i] = resource->colors;
    }
    CpuSet(fade->resources.palettes[0], gBgPaletteBuffer, 256);
}

void PaletteFade_SetupBackground(struct PaletteFade *fade, u32 resourceId, u32 layer, s32 unusedC,
                                 s32 unusedD)
{
    u32 selectedLayer;
    u32 resource = ScriptProc_Start(0xC091, resourceId);
    fade->background = resource;
    selectedLayer = layer;
    Bg_SetupLayers(2, 0, resource, 0, 0, 1, &selectedLayer);
    Bg_SetPriority(3, 3);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
}

static inline s32 NextArgShifted(void)
{
    if (Script_GetPc())
        return Script_GetValue() << 7;
    return 0;
}

static inline s32 NextArg(void)
{
    if (Script_GetPc())
        return Script_GetValue();
    return 0;
}

s32 PaletteFade_InitFromScript(struct PaletteFade *fade)
{
    s32 backgroundId, backgroundLayer, sourceId, targetId;
    u32 mode;
    s32 result;
    if (!Script_SeekToKeyword('b'))
        goto fail;
    backgroundId = NextArg();
    backgroundLayer = NextArg();
    sourceId = NextArg();
    targetId = NextArg();
    PaletteFade_SetupBackground(fade, backgroundId, backgroundLayer, sourceId, targetId);
    fade->mode = Script_GetKeywordValue('f', 2);
    fade->layer = Script_GetKeywordValue('p', 0);
    mode = fade->mode;
    if (mode & 1) {
        if (!Script_SeekToKeyword('s'))
            goto fail;
        fade->scanline.wave.speedTop = NextArg();
        fade->scanline.wave.speedBottom = NextArg();
        if (!Script_SeekToKeyword('l'))
            goto fail;
        fade->scanline.wave.firstLine = NextArg();
        fade->scanline.wave.split = NextArg();
        fade->updateScroll = ScanlineScroll_Update;
    } else if (mode & 2) {
        if (!Script_SeekToKeyword('s'))
            goto fail;
        fade->horizontalSpeed = NextArgShifted();
        fade->verticalSpeed = NextArgShifted();
        fade->updateScroll = BackgroundScroll_UpdateHorizontal;
    }
    goto done;
fail:
    return -1;
done:
    Memory_Clear(fade->offsets, sizeof(fade->offsets));
    Memory_Clear(fade->phases, sizeof(fade->phases));
    result = ((s32(*)(struct PaletteFade *, u32, u32))PaletteFade_LoadPaletteResources)(
        fade, sourceId, targetId);
    gPaletteFade = fade;
    return result;
}

struct PaletteFade *PaletteFade_CreateFromScript(void)
{
    struct PaletteFade *fade;
    fade = Task_Create(9, sizeof(*fade));
    if (fade != 0) {
        Task_SetCallbacks(fade, (void (*)(void *))PaletteFade_OnTaskUpdate,
                          (void (*)(void *))PaletteFade_OnTaskDestroy);
        if (PaletteFade_InitFromScript(fade) < 0) {
            Task_Kill(fade);
            return 0;
        }
    }
    return fade;
}

void PaletteFade_SetScrollPosition(s32 horizontal, s32 vertical)
{
    struct PaletteFade *fade = gPaletteFade;
    if (fade != 0) {
        fade->horizontalSpeed = horizontal;
        fade->verticalSpeed = vertical;
    }
}

void PaletteFade_SetWaveParameter(s32 index, u32 value)
{
    if (((struct PaletteFade *)gPaletteFade))
        ((struct PaletteFade *)gPaletteFade)->scanline.waveParameters[index] = value;
}

/* Registry routine 734C: fades palette slot -t (default 3) of the palette
   buffer towards palette -g of the fade task over -f frames (default 60; 0 =
   at once), -r (default 8). Per color it keeps the start channels and the
   channel deltas as 8.24 fixed point. */

#define PALETTE_FADE_READ_RED(color_, red_, sample_)                                               \
    ((color_ = (sample_)), (red_ = 31), (red_ &= color_), red_)

void PaletteFade_StartFromScript(void)
{
    struct PaletteFade *fade;
    s32 slot;
    s32 target;
    s32 rate;
    s32 frames;
    if (!gPaletteFade)
        return;
    slot = Script_GetKeywordValue('t', 3);
    target = Script_GetKeywordValue('g', 0);
    rate = Script_GetKeywordValue('r', 8);
    frames = Script_GetKeywordValue('f', 60);
    fade = gPaletteFade;
    PaletteFade_SetTransitionParameters(fade, slot, target, rate, frames);
    if (frames <= 0) {
        s32 sourceBankOffset = fade->slot * 32;
        u16 *sourcePalette = gBgPaletteBuffer;
        u16 *targetPalette = fade->resources.banks.targets;
        s32 targetBankOffset = fade->target * 32;
        s32 i;
        for (i = 0; i <= 15; i++) {
            u32 targetColor, sourceColor;
            u16 initialRed, initialGreen, initialBlue;
            u16 targetRed, sourceRed;
            fade->deltaR[fade->slot][i] =
                (PALETTE_FADE_READ_RED(targetColor, targetRed,
                                       *(u16 *)((u32)targetPalette + i * 2 + targetBankOffset)) -
                 (initialRed = PALETTE_FADE_READ_RED(
                      sourceColor, sourceRed,
                      *(u16 *)((u32)sourcePalette + i * 2 + sourceBankOffset))))
                << 24;
            fade->deltaG[fade->slot][i] = ((((targetColor <<= 16) >> 21) & 31) -
                                           (initialGreen = ((sourceColor <<= 16) >> 21) & 31))
                                          << 24;
            fade->deltaB[fade->slot][i] =
                (((targetColor >> 26) & 31) - (initialBlue = (sourceColor >> 26) & 31)) << 24;
            fade->red[fade->slot][i] = initialRed << 24;
            fade->green[fade->slot][i] = initialGreen << 24;
            fade->blue[fade->slot][i] = initialBlue << 24;
        }

        PaletteFade_AdvanceColors(fade);
    } else
        PaletteFade_BeginNextTarget(fade);
}

void PaletteFade_StartScrollFromScript(void)
{
    struct PaletteFade *g = gPaletteFade;
    if (g != 0) {
        struct PaletteFade *fade = g;
        fade->remaining = Script_GetKeywordValue(0x66, 0);
        fade->destination = Script_GetKeywordValue(0x70, 0) << 7;
        if (fade->remaining == 0)
            fade->speed = fade->destination;
        else
            fade->speed = Div(fade->destination - fade->scroll, fade->remaining);
        fade->moving = 1;
    }
}
