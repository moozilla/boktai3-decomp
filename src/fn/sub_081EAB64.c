#include "global.h"
#include "render.h"
// confidence=low: menu ownership follows Params3D0_CreateDisplay.

// confidence=low: menu ownership follows Params3D0_CreateDisplay; option meanings remain tentative.
struct Display {
    u8 sprites[0x724];
    s32 selection, count;
    u32 state;
    s32 result, counter, fadeShift, idleTimer, unused740, unused744, stateTimer;
    u32 lastState;
    s32 velocity, timer, slide, paletteStep, unused760, skipped, musicTimer;
};
struct Parameters {
    void *g, *o, *l, *s, *d, *c;
    s16 timeout, silent;
    s32 fadeOut, confirmFade;
};
struct Message {
    u32 count : 16;
    u32 unused : 16;
    void *values;
};
extern struct Parameters gUnk_020003D0;
extern u16 gUnk_03005260[];
extern u8 *gUnk_02000710;
extern const s32 gUnk_08613D80[];
s32 TimeDisplay_GetCurrentMonth(void);
s32 TimeDisplay_GetLightPhaseGroup(void);
void Sound_PlaySong(s32);
s32 Sound_IsSongPlaying(s32);
void Sound_FadeOutChannel10(s32);
void Menu_StartFadeOut(s32);
void BgGfx_LoadTilemapAtPosition(s32, s32, s32);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void Palette_InterpolateBanks(void *, void *, void *, s32, s32);
s32 Mod(s32, s32);
void TimeDisplay_SetSelectionKeyframe(void *);
void Actor_HideAuxiliarySprites(void *);
void Actor_UpdatePairedSpriteAnimation(void *);
void Anim_LoadKeyframe(void *, void *, u16, s32);
void TimeDisplay_HideAnimatedObjects(void *);
void Script_Call(void *, void *);
void Task_Kill(void *);
#define FLAGS(off) (*(u32 *)((u8 *)p + (off)))
void Params3D0_UpdateDisplay(struct Display *p)
{
    u8 *source_3;
    u8 *source_2;
    s32 value;
    s16 position;
    struct Message message;
    message.count = 1;
    message.values = &value;
    if (p->timer == p->musicTimer + 170 && gUnk_020003D0.silent == 0) {
        switch (TimeDisplay_GetCurrentMonth() - 1) {
            case 2:
            case 3:
            case 4: {
                const s32 *table = gUnk_08613D80;
                s32 phase = TimeDisplay_GetLightPhaseGroup() * 4;
                table += 2;
                Sound_PlaySong(*(const s32 *)((const u8 *)table + phase));
            } break;
            case 5:
            case 6:
            case 7: {
                const s32 *table = gUnk_08613D80;
                s32 phase = TimeDisplay_GetLightPhaseGroup() * 4;
                table += 4;
                Sound_PlaySong(*(const s32 *)((const u8 *)table + phase));
            } break;
            case 8:
            case 9:
            case 10: {
                const s32 *table = gUnk_08613D80;
                s32 phase = TimeDisplay_GetLightPhaseGroup() * 4;
                table += 6;
                Sound_PlaySong(*(const s32 *)((const u8 *)table + phase));
            } break;
            case 0:
            case 1:
            case 11: {
                const s32 *table = gUnk_08613D80;
                s32 phase = TimeDisplay_GetLightPhaseGroup() * 4;
                table += 8;
                Sound_PlaySong(*(const s32 *)((const u8 *)table + phase));
            } break;
        }
    }
    if (p->idleTimer > gUnk_020003D0.timeout) {
        p->state = 13;
        p->result = 5;
        p->fadeShift = gUnk_020003D0.fadeOut;
        Menu_StartFadeOut(p->fadeShift);
        if (Sound_IsSongPlaying(0xA4)) {
            Sound_FadeOutChannel10(p->fadeShift);
        }
        p->idleTimer = 0;
        return;
    }
    switch (p->state) {
        case 0:
            if (p->timer > (1 << p->fadeShift)) {
                p->state = 1;
            }
            break;
        case 1:
            if ((gUnk_03005260[1] & 1) || (gUnk_03005260[1] & 10)) {
                if (p->stateTimer > 10) {
                    p->skipped = 1;
                    p->unused760 = 1;
                    BgGfx_LoadTilemapAtPosition(2, 0, 0);
                    p->state = 3;
                    break;
                }
            }
            BgGfx_LoadTilemapAtPosition(2, p->slide, 0);
            if (p->slide >= 0) {
                p->state = 2;
                p->slide = 0;
            } else {
                p->slide += 4;
            }
            break;
        case 2:
            if (p->stateTimer > 10) {
                p->state = 3;
            }
            break;
        case 3:
            if (p->stateTimer == 0) {
                ScreenTransition_SendCommand(1, 2, 31, 31, 31, 0xFFFF, 0);
            } else if (p->stateTimer == 4) {
                Sound_PlaySong(0x2BC);
                p->musicTimer = p->timer;
                ScreenTransition_SendCommand(0, 5, 31, 31, 31, 0xFFFF, 0);
                *((u32 *)(((u8 *)p) + 0x20)) &= ~1;
                *((u32 *)(((u8 *)p) + 0x350)) &= ~1;
                *((u32 *)(((u8 *)p) + 0x3D8)) &= ~1;
                *((u32 *)(((u8 *)p) + 0x460)) &= ~1;
            } else if (p->stateTimer > 35) {
                p->state = 4;
            }
            break;
        case 4:
            if (p->stateTimer > 10) {
                p->state = 5;
            }
            break;
        case 5:
            if ((gUnk_03005260[1] & 1) || (gUnk_03005260[1] & 10)) {
                if (!p->skipped) {
                    p->skipped = 1;
                    p->paletteStep = 1 << p->unused760;
                }
            }
            {
                u8 *source;
                u8 *input;
                u8 *output;
                if (p->paletteStep <= (1 << p->unused760)) {
                    *((u32 *)(((u8 *)p) + 0xA8)) &= ~1;
                    source_2 = gPaletteBankData + 0x6240;
                    input = (u8 *)p + 0x520;
                    output = (u8 *)p + 0x700;
                    Palette_InterpolateBanks(input, output, source_2, p->paletteStep, p->unused760);
                    source = gPaletteBankData + 0x6260;
                    Palette_InterpolateBanks((u8 *)p + 0x540, output, source, p->paletteStep,
                                             p->unused760);
                    *(void **)((u8 *)p + 0xE8) = (u8 *)p + 0x500;
                    p->paletteStep++;
                }
            }
            if ((p->paletteStep > ((1 << (p->unused760 - 1)) + 20)) || (p->skipped == 1)) {
                p->state = 6;
            }
            break;
        case 6:
            if ((gUnk_03005260[1] & 1) || (gUnk_03005260[1] & 8)) {
                if (p->timer > (1 << p->fadeShift)) {
                    FLAGS(0x130) &= ~1;
                    p->state = 7;
                    Sound_PlaySong(0xE0);
                    p->idleTimer = 0;
                    p->paletteStep = 1 << p->unused760;
                }
            }
            {
                u8 *source;
                u8 *input;
                u8 *output;
                if (p->paletteStep <= (1 << p->unused760)) {
                    *((u32 *)(((u8 *)p) + 0xA8)) &= ~1;
                    source_3 = gPaletteBankData + 0x6240;
                    input = (u8 *)p + 0x520;
                    output = (u8 *)p + 0x700;
                    Palette_InterpolateBanks(input, output, source_3, p->paletteStep, p->unused760);
                    source = gPaletteBankData + 0x6260;
                    Palette_InterpolateBanks((u8 *)p + 0x540, output, source, p->paletteStep,
                                             p->unused760);
                    *(void **)((u8 *)p + 0xE8) = (u8 *)p + 0x500;
                    p->paletteStep++;
                }
            }
            if (Mod(p->stateTimer, 30) == 0) {
                if (FLAGS(0x130) & 1)
                    FLAGS(0x130) &= ~1;
                else
                    FLAGS(0x130) |= 1;
            }
            p->idleTimer++;
            break;
        case 7:
            if (p->stateTimer > 124) {
                p->state = 8;
                FLAGS(0x130) &= ~1;
                TimeDisplay_SetSelectionKeyframe(p);
                Actor_HideAuxiliarySprites(p);
            } else if (Mod(p->stateTimer, 4) == 0) {
                if (FLAGS(0x130) & 1)
                    FLAGS(0x130) &= ~1;
                else
                    FLAGS(0x130) |= 1;
            }
            break;
        case 8:
            if (p->timer < (1 << p->fadeShift)) {
                break;
            }
            if ((gUnk_03005260[1] & 1) || (gUnk_03005260[1] & 0x38)) {
                p->idleTimer = 0;
                if ((gUnk_03005260[1] & 1) || (gUnk_03005260[1] & 8)) {
                    *(u16 *)(gUnk_02000710 + 0x870) = p->selection;
                    if (p->selection == 0) {
                        Sound_PlaySong(0xE0);
                        p->fadeShift = gUnk_020003D0.confirmFade;
                        if (p->fadeShift <= 5) {
                            p->fadeShift = 6;
                        }
                    } else {
                        Sound_PlaySong(0xDD);
                        p->fadeShift = gUnk_020003D0.confirmFade;
                    }
                    p->state = 13;
                    p->result = p->selection + 1;
                    Menu_StartFadeOut(p->fadeShift);
                    if (Sound_IsSongPlaying(0xA4)) {
                        if (p->fadeShift <= 1) {
                            Sound_FadeOutChannel10(p->fadeShift);
                        } else {
                            Sound_FadeOutChannel10(p->fadeShift - 1);
                        }
                    }
                    break;
                } else if (gUnk_03005260[1] & 0x20) {
                    Sound_PlaySong(0x12B);
                    if ((--p->selection) < 0) {
                        p->selection = p->count - 1;
                    }
                    p->state = 11;
                    break;
                } else if (gUnk_03005260[1] & 0x10) {
                    Sound_PlaySong(0x12B);
                    if ((++p->selection) >= p->count) {
                        p->selection = 0;
                    }
                    p->state = 9;
                    break;
                }
            }
            Actor_UpdatePairedSpriteAnimation(p);
            p->idleTimer++;
            break;
        case 9:
            Anim_LoadKeyframe((u8 *)p + 0x1B0, (u8 *)p + 0x4E0, (u16)(p->selection + 12), 0);
            FLAGS(0x1B8) &= ~1;
            {
                s16 *destination = (s16 *)((u8 *)p + 0x1D0);
                s32 zero = 0;
                *destination = 144;
                p->velocity = zero;
            }
            p->state = 10;
            break;
        case 10:
            Actor_UpdatePairedSpriteAnimation(p);
            if (p->stateTimer <= 4) {
                Anim_LoadKeyframe((u8 *)p + 0x2C0, (u8 *)p + 0x4E0, 6, 0);
            }
            p->velocity -= 2;
            position = *(u16 *)((u8 *)p + 0x1D0) + p->velocity;
            *(u16 *)((u8 *)p + 0x1D0) = position;
            {
                s32 y = (s16)position - 144;
                *(s16 *)((u8 *)p + 0x148) = y;
            }
            if ((s16)position <= 0) {
                Anim_LoadKeyframe((u8 *)p + 0x128, (u8 *)p + 0x4E0, (u16)(p->selection + 12), 0);
                *(s16 *)((u8 *)p + 0x148) = 0;
                FLAGS(0x1B8) |= 1;
                p->state = 8;
            }
            break;
        case 11:
            Anim_LoadKeyframe((u8 *)p + 0x1B0, (u8 *)p + 0x4E0, (u16)(p->selection + 12), 0);
            FLAGS(0x1B8) &= ~1;
            {
                u16 *destination = (u16 *)((u8 *)p + 0x1D0);
                s32 zero = 0;
                {
                    s32 initialY = 0xFF70;
                    *destination = initialY;
                }
                p->velocity = zero;
            }
            p->state = 12;
            break;
        case 12:
            Actor_UpdatePairedSpriteAnimation(p);
            if (p->stateTimer <= 4) {
                Anim_LoadKeyframe((u8 *)p + 0x238, (u8 *)p + 0x4E0, 10, 0);
            }
            p->velocity += 2;
            position = *(u16 *)((u8 *)p + 0x1D0) + p->velocity;
            *(u16 *)((u8 *)p + 0x1D0) = position;
            {
                s32 y = (s16)position + 144;
                *(s16 *)((u8 *)p + 0x148) = y;
            }
            if ((s16)position >= 0) {
                Anim_LoadKeyframe((u8 *)p + 0x128, (u8 *)p + 0x4E0, (u16)(p->selection + 12), 0);
                *(s16 *)((u8 *)p + 0x148) = 0;
                FLAGS(0x1B8) |= 1;
                p->state = 8;
            }
            break;
        case 14:
            break;
        case 13:
            if ((u32)(p->result - 1) <= 3 && Mod(p->stateTimer, 4) == 0) {
                if (FLAGS(0x130) & 1)
                    FLAGS(0x130) &= ~1;
                else
                    FLAGS(0x130) |= 1;
            }
            if (p->counter > (1 << p->fadeShift) || p->fadeShift == 0) {
                TimeDisplay_HideAnimatedObjects(p);
                if (p->result == 0) {
                    Script_Call(gUnk_020003D0.c, 0);
                } else if (p->result == 1) {
                    Script_Call(gUnk_020003D0.g, 0);
                } else if (p->result == 2) {
                    value = 0;
                    Script_Call(gUnk_020003D0.o, &message);
                } else if (p->result == 3) {
                    Script_Call(gUnk_020003D0.l, 0);
                } else if (p->result == 5) {
                    Script_Call(gUnk_020003D0.d, 0);
                } else if (p->result == 4) {
                    Script_Call(gUnk_020003D0.s, 0);
                }
                Task_Kill(p);
            }
            p->counter++;
            break;
    }

    if (p->state != p->lastState) {
        p->stateTimer = 0;
    }
    p->lastState = p->state;
    p->stateTimer++;
    p->timer++;
}
