#ifndef GUARD_MENU_PARAMETERS_H
#define GUARD_MENU_PARAMETERS_H

#include "global.h"

/* Keyword letters are the script options; their roles remain provisional. */
struct OptionMenuCallbackNames {
    s32 g, o, s, c, r, l, d, f;
};
struct OptionMenuParameters {
    union {
        struct OptionMenuCallbackNames keyword;
        u32 entries[8];
    } callbacks;
};
extern struct OptionMenuParameters gOptionMenuParameters;

struct ChoiceMenuParameters {
    u8 *mBlock, *eBlock;
    s32 s, r, x, c, n, b, n2, p;
};
extern struct ChoiceMenuParameters gChoiceMenuParameters;

struct MessageMenuParameters {
    u8 *lBlock;
    s32 o, c, f;
};
extern struct MessageMenuParameters gMessageMenuParameters;

struct RegionMenuParameters {
    u8 *nBlock;
    u32 table, confirmProc, cancelProc, flag;
};
extern struct RegionMenuParameters gRegionMenuParameters;

struct DateMenuParameters {
    u8 unk0[8];
    u32 confirmProc, cancelProc, flag;
};
extern struct DateMenuParameters gDateMenuParameters;

struct SelectionMenuParameters {
    u8 *pBlock, *PBlock, *NBlock;
    s32 s, f, c, S;
};
extern struct SelectionMenuParameters gSelectionMenuParameters;

struct ScriptMessageParameters {
    u8 *text;
    u8 *pc;
    s32 value;
};
extern struct ScriptMessageParameters gScriptMessageParameters;

struct ListMenuParameters {
    u8 *mBlock, *aBlock;
    s32 l, F, f, c, b;
};
extern struct ListMenuParameters gListMenuParameters;

struct SoundTestParameters {
    u8 *text;
    u32 confirmProc;
};
extern struct SoundTestParameters gSoundTestParameters;

#endif
