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
/* 0x020002A0 */
extern struct OptionMenuParameters gOptionMenuParameters;

struct ChoiceMenuParameters {
    u8 *mBlock, *eBlock;
    s32 s, r, x, c, n, b, n2, p;
};
/* 0x020002C0 */
extern struct ChoiceMenuParameters gChoiceMenuParameters;

struct MessageMenuParameters {
    u8 *lBlock;
    s32 o, c, f;
};
/* 0x020002E8 */
extern struct MessageMenuParameters gMessageMenuParameters;

struct RegionMenuParameters {
    u8 *nBlock;
    u32 table, confirmProc, cancelProc, flag;
};
/* 0x02000310 */
extern struct RegionMenuParameters gRegionMenuParameters;

struct DateMenuParameters {
    u8 unk0[8];
    u32 confirmProc, cancelProc, flag;
};
/* 0x02000340 */
extern struct DateMenuParameters gDateMenuParameters;

struct SelectionMenuParameters {
    u8 *pBlock, *PBlock, *NBlock;
    s32 s, f, c, S;
};
/* 0x02000358 */
extern struct SelectionMenuParameters gSelectionMenuParameters;

struct ScriptMessageParameters {
    u8 *text;
    u8 *pc;
    s32 value;
};
/* 0x02000378 */
extern struct ScriptMessageParameters gScriptMessageParameters;

struct ListMenuParameters {
    u8 *mBlock, *aBlock;
    s32 l, F, f, c, b;
};
/* 0x020003A0 */
extern struct ListMenuParameters gListMenuParameters;

struct SoundTestParameters {
    u8 *text;
    u32 confirmProc;
};
/* 0x020003C0 */
extern struct SoundTestParameters gSoundTestParameters;

struct TimeDisplayParameters {
    s32 g, o, l, s, d, c;
    s16 timeout, silent;
    s32 fadeOut, confirmFade;
    s32 M;
};
/* 0x020003D0 */
extern struct TimeDisplayParameters gTimeDisplayParameters;

#endif
