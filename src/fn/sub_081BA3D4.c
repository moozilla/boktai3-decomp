#include "global.h"

struct Registry2388 {
    u8 unknown0[0x524];
    u32 unknown524;
    u16 availableEntries;
    u16 disableCancel;
    s32 selection;
    s16 initialSelection;
    s16 cancelSelection;
    s32 entryValues[9];
    void *selectionCursor;
};
void Video_SetRenderCallbacks(s32, void *, void *, void *);
void sub_08217F4C(void);
void sub_08214B44(void);
void sub_08219624(void);
void Registry2388_InitializeBackground(struct Registry2388 *);
void Registry2388_InitializeAnimations(struct Registry2388 *);
void Registry2388_ApplyPalettePhase(struct Registry2388 *, s32);
void Registry2388_CreateSelectionCursor(struct Registry2388 *, s32);
s32 Script_GetValue(void);
s32 Script_SeekToKeyword(u32);
s32 SunClock_GetPhase(void);
void MenuCursor_Hide(void *);
void Registry2388_BeginFadeIn(void *);

s32 Registry2388_InitializeFromScript(struct Registry2388 *object, void *arg)
{
    s32 i;
    s32 *value;
    Video_SetRenderCallbacks(0, sub_08217F4C, sub_08214B44, sub_08219624);
    Registry2388_InitializeBackground(object);
    Registry2388_InitializeAnimations(object);
    if (Script_SeekToKeyword(0x70)) {
        value = object->entryValues;
        i = 8;
        do {
            *value++ = Script_GetValue();
        } while (--i >= 0);
    } else {
        s32 zero = 0;
        i = 8;
        do {
            object->entryValues[i] = zero;
        } while (--i >= 0);
    }
    if (Script_SeekToKeyword(0x50))
        object->initialSelection = Script_GetValue();
    else
        object->initialSelection = 0;
    if (Script_SeekToKeyword(0x43))
        object->cancelSelection = Script_GetValue();
    else
        object->cancelSelection = -1;
    if (Script_SeekToKeyword(0x63) && Script_GetValue() == 0)
        object->disableCancel = 1;
    object->availableEntries = 0;
    if (Script_SeekToKeyword(0x66)) {
        for (i = 0; i < 9; i++) {
            if (Script_GetValue())
                object->availableEntries |= 1 << i;
        }
    }
    object->selection = object->initialSelection;
    object->unknown524 = 0;
    if (Script_SeekToKeyword(0x62))
        Registry2388_ApplyPalettePhase(object, Script_GetValue());
    else
        Registry2388_ApplyPalettePhase(object, SunClock_GetPhase());
    Registry2388_CreateSelectionCursor(object, object->initialSelection);
    MenuCursor_Hide(object->selectionCursor);
    Registry2388_BeginFadeIn(object);
    return 0;
}
