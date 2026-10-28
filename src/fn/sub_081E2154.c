#include "global.h"
#include "menu_parameters.h"
// Provisional name: SoundTest_Update
struct Sprite {
    u8 head[8];
    u32 flags;
    u8 gap[0x20 - 12];
    s16 x, y;
    u8 tail[0x60 - 0x24];
};
struct Resource {
    u32 words[8];
};
struct Viewer {
    u8 p0[0x20];
    struct Sprite background;
    struct Resource backgroundResource;
    u8 p1[8];
    struct Sprite panel;
    struct Resource panelResource;
    u8 p2[8];
    struct Sprite cursor;
    struct Resource cursorResource;
    u8 p3[8];
    struct Sprite label;
    struct Resource labelResource;
    u8 p4[8];
    struct Resource digitsResource;
    struct Sprite digitsA[3], digitsB[3], labels[3];
    s32 state, exitFlag, exitTimer, fadeShift, selection, countA, countB, countC, textIndex, valueA,
        valueB, previousA, previousB, previousC, repeatTimer;
};
void Anim_LoadKeyframe(void *, void *, u16, s32);
s32 Mod(s32, s32);
s32 Div(s32, s32);
extern const s16 gUnk_086136F0[], gUnk_086137AE[], gUnk_08613BC4[];
u32 Sound_IsSongPlaying(s32);
void Sound_StopSong(s32);
void Sound_PlaySong(s32);
void Anim_SetKeyframes(void *, void *, u16, s32, s32);
extern u16 gUnk_03005260[];
void Menu_StartFadeOut(s32);
s32 SoundTest_StopSelection(void *);
void SoundTest_UpdatePlaybackIndicators(void *);
void SoundTest_PlaySelection(void *);
void SoundTest_MoveCategoryCursor(void *);
void SoundTest_SelectPrevious(void *);
void SoundTest_SelectNext(void *);
void Palette_UpdateHighlightPulse(void *, void *);
void Script_Call(void *, void *);
void Task_Kill(void *);
// Handle sound-test controls, accelerating held left/right, and the exit fade.
void SoundTest_Update(struct Viewer *p)
{
    s32 direction, interval;
    switch (p->state) {
        case 0:
            if ((gUnk_03005260[1] & 2) && SoundTest_StopSelection(p)) {
                Menu_StartFadeOut(p->fadeShift);
                p->state = 1;
                p->exitFlag = 0;
                Sound_PlaySong(222);
                break;
            }
            SoundTest_UpdatePlaybackIndicators(p);
            if (gUnk_03005260[1] & 1)
                SoundTest_PlaySelection(p);
            if (gUnk_03005260[1] & 64) {
                if (--p->selection < 0)
                    p->selection = 2;
                SoundTest_MoveCategoryCursor(p);
                Sound_PlaySong(0x37c);
            } else if (gUnk_03005260[1] & 128) {
                if (++p->selection > 2)
                    p->selection = 0;
                SoundTest_MoveCategoryCursor(p);
                Sound_PlaySong(0x37c);
            }
            direction = 0;
            interval = 5;
            if (gUnk_03005260[1] & 32)
                direction = 1;
            else if (gUnk_03005260[1] & 16)
                direction = 2;
            else if (gUnk_03005260[0] & 48) {
                if (++p->repeatTimer > 14) {
                    if (p->repeatTimer > 199)
                        interval = 2;
                    if (Mod(p->repeatTimer, interval) == 0) {
                        if (gUnk_03005260[0] & 32)
                            direction = 1;
                        else
                            direction = 2;
                    }
                }
            } else
                p->repeatTimer = 0;
            if (direction == 1)
                SoundTest_SelectPrevious(p);
            else if (direction == 2)
                SoundTest_SelectNext(p);
            Palette_UpdateHighlightPulse((u8 *)p + 0x618, (u8 *)p + 0x620);
            break;
        case 1:
            if (p->exitTimer > (1 << p->fadeShift)) {
                if (!p->exitFlag)
                    Script_Call(gSoundTestParameters.confirmProc, 0);
                Task_Kill(p);
            }
            p->exitTimer++;
            break;
        case 2:
            break;
    }
}
