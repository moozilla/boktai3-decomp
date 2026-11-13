#ifndef GUARD_SOUND_TEST_H
#define GUARD_SOUND_TEST_H
#include "scene_input.h"
#include "menu_parameters.h"
struct SoundTest {
    struct Task task;
    u32 unknown18[2];
    struct SceneInputWidget background;
    struct SceneInputResource backgroundResource;
    u32 unknownA0[2];
    struct SceneInputWidget panel;
    struct SceneInputResource panelResource;
    u32 unknown128[2];
    struct SceneInputWidget cursor;
    struct SceneInputResource cursorResource;
    u32 unknown1B0[2];
    struct SceneInputWidget categoryLabel;
    struct SceneInputResource labelResource;
    u32 unknown238[2];
    struct SceneInputResource digitsResource;
    struct SceneInputWidget secondCategoryDigits[3], thirdCategoryDigits[3], playbackIndicators[3];
    s32 state, exitFlag, exitTimer, fadeShift, selection, firstCategoryCount, secondCategoryCount,
        thirdCategoryCount, firstCategoryIndex, secondCategoryIndex, thirdCategoryIndex,
        lastFirstIndex, lastSecondIndex, lastThirdIndex, repeatTimer;
    const u16 *savedPalette;
    u16 palette[16];
    u8 pulseState[4];
};
struct SoundTestPalette {
    u32 header[5];
    u16 colors[256];
};
void SoundTest_CountEntries(struct SoundTest *p);
void SoundTest_ShowCategory(struct SoundTest *p);
void SoundTest_MoveCategoryCursor(struct SoundTest *p);
void SoundTest_DrawTrackName(struct SoundTest *p);
void SoundTest_DrawSecondCategoryIndex(struct SoundTest *p);
void SoundTest_DrawThirdCategoryIndex(struct SoundTest *p);
void SoundTest_PlaySelection(struct SoundTest *p);
s32 SoundTest_StopSelection(struct SoundTest *p);
void SoundTest_SelectPrevious(struct SoundTest *p);
void SoundTest_SelectNext(struct SoundTest *p);
void SoundTest_UpdatePlaybackIndicators(struct SoundTest *p);
void SoundTest_Update(struct SoundTest *p);
void SoundTest_Destroy(struct SoundTest *p);
s32 SoundTest_Initialize(struct SoundTest *p);
void SoundTest_ConfigureFromScript(void);
void *SoundTest_Create(void);
s32 Mod(s32, s32);
s32 Div(s32, s32);
extern const s16 gUnk_086136F0[], gUnk_086137AE[], gUnk_08613BC4[];
void Menu_EraseRect(s32, s32, s32, s32);
void Menu_DrawText(s32, s32, const u8 *);
u32 Sound_IsSongPlaying(s32);
s32 Anim_Step(void *, void *);
void Menu_StartFadeOut(s32);
void Anim_Remove(void *);
void Bg_SetupLayers(s32, s32, void *, s32, s32, s32, void *);
void Menu_StartFadeIn(s32);
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
#endif
