#include "sound_test.h"
#include "menu_parameters.h"
#include "input.h"
#include "render.h"

static inline s32 CountSoundEntries(const s16 *list)
{
    const s16 *cursor = list;
    s32 count = 0;
    s32 entry = *cursor;
    while (entry != -1) {
        cursor++;
        count++;
        entry = *cursor;
    }
    return count;
}

// Count entries in each of the three sound-test categories.

void SoundTest_CountEntries(struct SoundTest *p)
{
    p->firstCategoryCount = CountSoundEntries(gUnk_086136F0);
    p->secondCategoryCount = CountSoundEntries(gUnk_086137AE);
    p->thirdCategoryCount = CountSoundEntries(gUnk_08613BC4);
}

static inline void ShowWidget(struct SceneInputWidget *s)
{
    s->flags &= ~1;
}

// Show the selected category categoryLabel and its cursor.

void SoundTest_ShowCategory(struct SoundTest *p)
{
    ShowWidget(&p->cursor);
    Anim_LoadKeyframe(&p->categoryLabel, &p->digitsResource, (u16)(p->selection + 47), 0);
    p->categoryLabel.flags &= ~1;
}

static inline void MoveCategoryCursor(struct SceneInputWidget *s, s32 selection)
{
    s->position.y = (selection + 1) * 32;
}

// Move the category cursor and refresh the category categoryLabel.

void SoundTest_MoveCategoryCursor(struct SoundTest *p)
{
    MoveCategoryCursor(&p->cursor, p->selection);
    Anim_LoadKeyframe(&p->categoryLabel, &p->digitsResource, (u16)(p->selection + 47), 0);
    p->categoryLabel.flags &= ~1;
}

// Refresh the selected sound-test entry's text categoryLabel.

void SoundTest_DrawTrackName(struct SoundTest *p)
{
    const u8 *text;
    Bg_ClearTilemap(0);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    text =
        Text_LookupString(Script_ParseStringRef(gSoundTestParameters.text) + p->firstCategoryIndex);
    Menu_EraseRect(10, 4, 16, 2);
    Menu_DrawText(10, 4, text);
}

// Display the first three-digit value, least significant digit first.

void SoundTest_DrawSecondCategoryIndex(struct SoundTest *p)
{
    s32 value = p->secondCategoryIndex, i;
    for (i = 0; i < 3; i++) {
        Anim_LoadKeyframe(&p->secondCategoryDigits[2 - i], &p->digitsResource, (u16)Mod(value, 10),
                          0);
        p->secondCategoryDigits[2 - i].flags &= ~1;
        value = Div(value, 10);
    }
}

// Display the second three-digit value, least significant digit first.

void SoundTest_DrawThirdCategoryIndex(struct SoundTest *p)
{
    s32 value = p->thirdCategoryIndex, i;
    for (i = 0; i < 3; i++) {
        Anim_LoadKeyframe(&p->thirdCategoryDigits[2 - i], &p->digitsResource, (u16)Mod(value, 10),
                          0);
        p->thirdCategoryDigits[2 - i].flags &= ~1;
        value = Div(value, 10);
    }
}

// Play the selected sound, stopping the previous sound in that category.

void SoundTest_PlaySelection(struct SoundTest *p)
{
    switch (p->selection) {
        case 0:
            if (Sound_IsSongPlaying(gUnk_086136F0[p->lastFirstIndex]))
                Sound_StopSong(gUnk_086136F0[p->lastFirstIndex]);
            Sound_PlaySong(gUnk_086136F0[p->firstCategoryIndex]);
            p->lastFirstIndex = p->firstCategoryIndex;
            break;
        case 1:
            if (Sound_IsSongPlaying(gUnk_086137AE[p->lastSecondIndex]))
                Sound_StopSong(gUnk_086137AE[p->lastSecondIndex]);
            Sound_PlaySong(gUnk_086137AE[p->secondCategoryIndex]);
            p->lastSecondIndex = p->secondCategoryIndex;
            break;
        case 2:
            if (Sound_IsSongPlaying(gUnk_08613BC4[p->lastThirdIndex]))
                Sound_StopSong(gUnk_08613BC4[p->lastThirdIndex]);
            Sound_PlaySong(gUnk_08613BC4[p->thirdCategoryIndex]);
            p->lastThirdIndex = p->thirdCategoryIndex;
            break;
    }
    Anim_SetKeyframes(&p->playbackIndicators[p->selection], &p->digitsResource, 0, 1, 0);
}

// Stop the selected preview, or signal that all categories are already silent.

s32 SoundTest_StopSelection(struct SoundTest *p)
{
    if (!Sound_IsSongPlaying(gUnk_086136F0[p->lastFirstIndex]) &&
        !Sound_IsSongPlaying(gUnk_086137AE[p->lastSecondIndex]) &&
        !Sound_IsSongPlaying(gUnk_08613BC4[p->lastThirdIndex])) {
        p->playbackIndicators[0].flags |= 1;
        p->playbackIndicators[1].flags |= 1;
        p->playbackIndicators[2].flags |= 1;
        Sound_PlaySong(222);
        return 1;
    }
    switch (p->selection) {
        case 0:
            Sound_StopSong((u16)gUnk_086136F0[p->lastFirstIndex]);
            break;
        case 1:
            Sound_StopSong((u16)gUnk_086137AE[p->lastSecondIndex]);
            break;
        case 2:
            Sound_StopSong((u16)gUnk_08613BC4[p->lastThirdIndex]);
            break;
        default:
            return 0;
    }
    p->playbackIndicators[p->selection].flags |= 1;
    return 0;
}

// Move to the previous sound in the selected category, wrapping at its ends.

void SoundTest_SelectPrevious(struct SoundTest *p)
{
    switch (p->selection) {
        case 0:
            if (--p->firstCategoryIndex < 0)
                p->firstCategoryIndex = p->firstCategoryCount - 1;
            SoundTest_DrawTrackName(p);
            break;
        case 1:
            if (--p->secondCategoryIndex < 0)
                p->secondCategoryIndex = p->secondCategoryCount - 1;
            SoundTest_DrawSecondCategoryIndex(p);
            break;
        case 2:
            if (--p->thirdCategoryIndex < 0)
                p->thirdCategoryIndex = p->thirdCategoryCount - 1;
            SoundTest_DrawThirdCategoryIndex(p);
            break;
    }
    Sound_PlaySong(0x37d);
}

// Move to the next sound in the selected category, wrapping at its ends.

void SoundTest_SelectNext(struct SoundTest *p)
{
    switch (p->selection) {
        case 0:
            if (++p->firstCategoryIndex >= p->firstCategoryCount)
                p->firstCategoryIndex = 0;
            SoundTest_DrawTrackName(p);
            break;
        case 1:
            if (++p->secondCategoryIndex >= p->secondCategoryCount)
                p->secondCategoryIndex = 0;
            SoundTest_DrawSecondCategoryIndex(p);
            break;
        case 2:
            if (++p->thirdCategoryIndex >= p->thirdCategoryCount)
                p->thirdCategoryIndex = 0;
            SoundTest_DrawThirdCategoryIndex(p);
            break;
    }
    Sound_PlaySong(0x37d);
}

// Advance each category's indicator animation while its song plays.

void SoundTest_UpdatePlaybackIndicators(struct SoundTest *p)
{
    s32 i;
    for (i = 0; i <= 2; i++) {
        const s16 *table;
        s32 index;
        struct SceneInputWidget *s = &p->playbackIndicators[i];
        switch (i) {
            case 0:
                table = gUnk_086136F0;
                index = p->lastFirstIndex;
                break;
            case 1:
                table = gUnk_086137AE;
                index = p->lastSecondIndex;
                break;
            case 2:
                table = gUnk_08613BC4;
                index = p->lastThirdIndex;
                break;
            default:
                return;
        }
        if (Sound_IsSongPlaying(table[index])) {
            s->flags &= ~1;
            Anim_Step(s, &p->digitsResource);
        } else if (!(s->flags & 1)) {
            s->attribute1B = 2;
            if (Anim_Step(s, &p->digitsResource))
                s->flags |= 1;
        }
    }
}

// Handle sound-test controls, accelerating held left/right, and the exit fade.

void SoundTest_Update(struct SoundTest *p)
{
    s32 direction, interval;
    switch (p->state) {
        case 0:
            if ((gPadState[1] & 2) && SoundTest_StopSelection(p)) {
                Menu_StartFadeOut(p->fadeShift);
                p->state = 1;
                p->exitFlag = 0;
                Sound_PlaySong(222);
                break;
            }
            SoundTest_UpdatePlaybackIndicators(p);
            if (gPadState[1] & 1)
                SoundTest_PlaySelection(p);
            if (gPadState[1] & 64) {
                if (--p->selection < 0)
                    p->selection = 2;
                SoundTest_MoveCategoryCursor(p);
                Sound_PlaySong(0x37c);
            } else if (gPadState[1] & 128) {
                if (++p->selection > 2)
                    p->selection = 0;
                SoundTest_MoveCategoryCursor(p);
                Sound_PlaySong(0x37c);
            }
            direction = 0;
            interval = 5;
            if (gPadState[1] & 32)
                direction = 1;
            else if (gPadState[1] & 16)
                direction = 2;
            else if (gPadState[0] & 48) {
                if (++p->repeatTimer > 14) {
                    if (p->repeatTimer > 199)
                        interval = 2;
                    if (Mod(p->repeatTimer, interval) == 0) {
                        if (gPadState[0] & 32)
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
            Palette_UpdateHighlightPulse(&p->palette[12], p->pulseState);
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

// Release every sprite used by the sound-test menu.

void SoundTest_Destroy(struct SoundTest *p)
{
    s32 i;
    struct SceneInputWidget *a, *b, *c;
    {
        s32 mask = 3;
        gDispCntShadow = ~mask & gDispCntShadow;
    }
    p->cursor.flags |= 1;
    p->panel.flags |= 1;
    p->background.flags |= 1;
    c = p->thirdCategoryDigits;
    b = p->secondCategoryDigits;
    a = p->playbackIndicators;
    for (i = 2; i >= 0; i--) {
        Anim_Remove(a);
        Anim_Remove(b);
        Anim_Remove(c);
        c++;
        b++;
        a++;
    }
    Anim_Remove(&p->cursor);
    Anim_Remove(&p->panel);
    Anim_Remove(&p->categoryLabel);
    Anim_Remove(&p->background);
}

static inline void ClearPosition(u32 *value, void *dest)
{
    *value = 0;
    CpuSet(value, dest, 0x05000002);
}

// Load sound-test assets and initialize its category playbackIndicators, counters and palette.

s32 SoundTest_Initialize(struct SoundTest *p)
{
    struct ContactPosition pos;
    u32 zero;
    void *asset;
    s32 option, i;
    ClearPosition(&zero, &pos);
    Bg_ClearTilemap(0);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    Bg_ClearTilemap(2);
    BgGfx_LoadTilemapSimple(2, 0, 0, 0, 0);
    Bg_ClearTilemap(3);
    {
        void *background = ScriptProc_Start(0xc091, 0x3536);
        option = 10;
        Bg_SetupLayers(2, 0, background, 0, 0, 1, &option);
    }
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
    CpuSet(((struct SoundTestPalette *)ScriptProc_Start(0x92b3, 0x204))->colors, gBgPaletteBuffer,
           256);
    asset = ScriptProc_Start(0xcb05, 0xe2ab);
    ResourceHeader_Relocate(&p->backgroundResource, asset);
    p->digitsResource = p->backgroundResource;
    Widget_InitLinked(&p->background, &p->backgroundResource, 59, 16, 0, 0, 0, 0);
    ResourceHeader_Relocate(&p->labelResource, asset);
    Widget_InitLinked(&p->categoryLabel, &p->labelResource, 47, 16, 0, 0, 0, 0);
    for (i = 0; i < 3; i++) {
        Widget_InitLinked(&p->playbackIndicators[i], &p->digitsResource, 43, 48, 1, 0, 60, &pos);
        p->playbackIndicators[i].flags |= 1;
        pos.y += 32;
    }
    pos.x = 184;
    pos.y = 64;
    for (i = 0; i < 3; i++) {
        Widget_InitLinked(&p->secondCategoryDigits[i], &p->digitsResource, 0, 48, 0, 0, 60, &pos);
        p->secondCategoryDigits[i].flags |= 1;
        pos.x += 8;
    }
    pos.x = 184;
    pos.y = 96;
    for (i = 0; i < 3; i++) {
        Widget_InitLinked(&p->thirdCategoryDigits[i], &p->digitsResource, 0, 48, 0, 0, 60, &pos);
        p->thirdCategoryDigits[i].flags |= 1;
        pos.x += 8;
    }
    asset = ScriptProc_Start(0xcb05, 0x530d);
    ResourceHeader_Relocate(&p->panelResource, asset);
    Widget_InitLinked(&p->panel, &p->panelResource, 139, 16, 0, 0, 0, 0);
    ResourceHeader_Relocate(&p->cursorResource, asset);
    Widget_InitLinked(&p->cursor, &p->cursorResource, 71, 16, 0, 0, 0, 0);
    p->savedPalette = p->cursor.palette;
    CpuSet(p->savedPalette, p->palette, 16);
    p->cursor.palette = p->palette;
    p->selection = 0;
    p->fadeShift = 3;
    Menu_StartFadeIn(3);
    SoundTest_CountEntries(p);
    SoundTest_ShowCategory(p);
    SoundTest_MoveCategoryCursor(p);
    SoundTest_DrawTrackName(p);
    SoundTest_DrawSecondCategoryIndex(p);
    SoundTest_DrawThirdCategoryIndex(p);
    return 0;
}

void SoundTest_ConfigureFromScript(void)
{
    if (Script_SeekToKeyword(0x72))
        gSoundTestParameters.text = Script_GetPc();
    if (Script_SeekToKeyword(0x63))
        gSoundTestParameters.confirmProc = Script_GetValue();
}

void *SoundTest_Create(void)
{
    void *r = Task_Create(0xb, sizeof(struct SoundTest));
    if (r != 0) {
        Task_SetCallbacks(r, SoundTest_Update, SoundTest_Destroy);
        if (SoundTest_Initialize(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
