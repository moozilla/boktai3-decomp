#include "global.h"
struct TextVm {
    u8 x, y, left, top, width, height, palette, style, entered, mode, skip, busy, previousMode,
        choiceCount, advance, fresh, brackets, voice, toggle, lastWidth, allowRun;
    s8 selection;
    u8 instant, wrap;
    u32 frame;
    const u8 *pc, *streams[4];
    int hasValue, length;
    u8 text[64];
    u8 pad78[4];
    u8 numberText[12];
    s32 numbers[16];
    u32 choices[16];
    u8 depth, styles[7];
    const u8 *strings[16];
    u32 selectedCount;
    u8 selected[32];
};
int Text_ComparePrefix(const u8 *, const u8 *, int);
const u8 *TextState_ParseTagValue(struct TextVm *, const u8 *);
int Text_ParseDecimal(const u8 *, int);
void TextState_PushStream(struct TextVm *, int);
void TextState_PlayGlyphSound(struct TextVm *, int);
int TextState_DrawNarrowGlyph(struct TextVm *, int);
void Text_FormatDecimal(u8 *, int);
void Sound_PlaySong(int);
const u8 *Text_FindChar(const u8 *, u8);
const u8 *PlayerName_GetBuffer(void);
const u8 *GameState_GetBikeName(void);
int TextState_GetPairSpeakerLabel(void);
int TextState_GetPlayerSpeakerLabel(void);
struct Sun {
    u8 pad[0x12];
    s16 level;
};
extern struct Sun *gGameState;
extern const u8 gUnk_0824DC80[]; /* WEIGHT */
extern const u8 gUnk_0824DC88[]; /* NONSEL */
extern const u8 gUnk_0824DC90[]; /* UVMOJI */
extern const u8 gUnk_0824DC98[]; /* WAIT */
extern const u8 gUnk_0824DCA0[]; /* PROC */
extern const u8 gUnk_0824DCA8[]; /* NAME */
extern const u8 gUnk_0824DCB0[]; /* BIKE */
extern const u8 gUnk_0824DCB8[]; /* LABEL */
extern const u8 gUnk_0824DCC0[]; /* NONE */
extern const u8 gUnk_0824DCC8[]; /* R_DJANGO */
extern const u8 gUnk_0824DCD4[]; /* B_DJANGO */
extern const u8 gUnk_0824DCE0[]; /* W_DJANGO */
extern const u8 gUnk_0824DCEC[]; /* OTENKO */
extern const u8 gUnk_0824DCF4[]; /* SABATA_B */
extern const u8 gUnk_0824DD00[]; /* SABATA */
extern const u8 gUnk_0824DD08[]; /* SMITH */
extern const u8 gUnk_0824DD10[]; /* RITA */
extern const u8 gUnk_0824DD18[]; /* ZAJI */
extern const u8 gUnk_0824DD20[]; /* SUMIRE */
extern const u8 gUnk_0824DD28[]; /* KURO */
extern const u8 gUnk_0824DD30[]; /* KID */
extern const u8 gUnk_0824DD34[]; /* SHAIAN */
extern const u8 gUnk_0824DD3C[]; /* LADY */
extern const u8 gUnk_0824DD44[]; /* COFFIN */
extern const u8 gUnk_0824DD4C[]; /* CLOCK */
extern const u8 gUnk_0824DD54[]; /* PEOPLE1 */
extern const u8 gUnk_0824DD5C[]; /* PEOPLE2 */
extern const u8 gUnk_0824DD64[]; /* PEOPLE3 */
extern const u8 gUnk_0824DD6C[]; /* HAKUSYAKU */
extern const u8 gUnk_0824DD78[]; /* FRES */
extern const u8 gUnk_0824DD80[]; /* NEEZ */
extern const u8 gUnk_0824DD88[]; /* RATA */
extern const u8 gUnk_0824DD90[]; /* KARMIRA */
extern const u8 gUnk_0824DD98[]; /* COACH */
extern const u8 gUnk_0824DDA0[]; /* ROCKMAN */
extern const u8 gUnk_0824DDA8[]; /* TRINITY */
extern const u8 gUnk_0824DDB0[]; /* HATENA */
extern const u8 gUnk_0824DDB8[]; /* GARIGARIKUN */
extern const u8 gUnk_0824DDC4[]; /* FUTARI */
extern const u8 gUnk_0824DDCC[]; /* PLAYER */
extern const u8 gUnk_0824DDD4[]; /* FACEOFF */
extern const u8 gUnk_0824DDDC[]; /* LOCK */
extern const u8 gUnk_0824DDE4[]; /* EXTEND */
extern const u8 gUnk_0824DDEC[]; /* SOUND */
extern const u8 gUnk_0824DDF4[]; /* VAR */
extern const u8 gUnk_0824DDF8[]; /* MOJISE */
extern const u8 gUnk_0824DE00[]; /* MOJISE_SYSTEM */
extern const u8 gUnk_0824DE10[]; /* MOJISE_TALK */
extern const u8 gUnk_0824DE1C[]; /* END */
extern const u8 gUnk_0824DE20[]; /* PAREN */
extern const u8 gUnk_0824DE28[]; /* ALTER */
extern const u8 gUnk_0824DE30[]; /* /WEIGHT */
extern const u8 gUnk_0824DE38[]; /* /NONSEL */
extern const u8 gUnk_0824DE40[]; /* /UVMOJI */
extern const u8 gUnk_0824DE48[]; /* /LABEL */
extern const u8 gUnk_0824DE50[]; /* /PAREN */
extern const u8 gUnk_0824DE58[]; /* /ALTER */
// MATCHING_CONCESSION: register-pinning
const u8 *TextState_ProcessTag(struct TextVm *p, const u8 *s)
{
    const u8 *end = s;
    int value;
    int cmp;
    int paletteValue;
    const u8 *findPtr;
    register struct TextVm *pushP asm("r0");
    int pushStyle;
    if (*s == '/')
        goto close;
    if (!Text_ComparePrefix(s, gUnk_0824DC80, 6)) {
        paletteValue = 1;
        goto palette;
    }
    if (!Text_ComparePrefix(s, gUnk_0824DC88, 6)) {
        paletteValue = 2;
        goto palette;
    }
    if (!Text_ComparePrefix(s, gUnk_0824DC90, 6)) {
        paletteValue = 2;
        asm("" : "+r"(paletteValue));
        goto palette;
    }
    if (!Text_ComparePrefix(s, gUnk_0824DC98, 4)) {
        end = TextState_ParseTagValue(p, s);
        if (p->hasValue && p->length)
            p->mode = Text_ParseDecimal(p->text, p->length);
        else {
            int level = gGameState->level;
            p->mode = level;
        }
        goto done;
    }
    if (!Text_ComparePrefix(s, gUnk_0824DCA0, 4)) {
        int value;
        end = TextState_ParseTagValue(p, s);
        if (p->hasValue && p->length)
            value = Text_ParseDecimal(p->text, p->length);
        else
            value = 0;
        p->selected[p->selectedCount] = value;
        p->selectedCount++;
        goto done;
    }
    if (!Text_ComparePrefix(s, gUnk_0824DCA8, 4)) {
        p->streams[0] = PlayerName_GetBuffer();
        TextState_PushStream(p, 1);
        findPtr = s;
        goto findAdvanceCall;
    }
    if (!Text_ComparePrefix(s, gUnk_0824DCB0, 4)) {
        p->streams[3] = GameState_GetBikeName();
        TextState_PushStream(p, 4);
        findPtr = s;
        asm("" : "+r"(findPtr));
        goto findAdvanceCall;
    }
    if (!Text_ComparePrefix(s, gUnk_0824DCB8, 5)) {
        const u8 *name;
        end = TextState_ParseTagValue(p, s);
        if (!p->hasValue)
            goto unknown;
        if (!p->length)
            goto unknown;
        name = p->text;
        if (!Text_ComparePrefix(name, gUnk_0824DCC0, 4))
            goto unknown;
        value = Text_ComparePrefix(name, gUnk_0824DCC8, 8);
        if (!value)
            goto selected;
        value = Text_ComparePrefix(name, gUnk_0824DCD4, 8);
        if (!value)
            goto selected;
        value = Text_ComparePrefix(name, gUnk_0824DCE0, 8);
        if (!value)
            goto selected;
        if (!Text_ComparePrefix(name, gUnk_0824DCEC, 6)) {
            value = 3;
            goto selected;
        }
        if (!Text_ComparePrefix(name, gUnk_0824DCF4, 8)) {
            value = 5;
            goto selected;
        }
        if (!Text_ComparePrefix(name, gUnk_0824DD00, 6)) {
            value = 4;
            goto selected;
        }
        if (!Text_ComparePrefix(name, gUnk_0824DD08, 5)) {
            value = 6;
            goto selected;
        }
        if (!Text_ComparePrefix(name, gUnk_0824DD10, 4)) {
            value = 7;
            goto selected;
        }
        if (!Text_ComparePrefix(name, gUnk_0824DD18, 4)) {
            value = 8;
            goto selected;
        }
        if (!Text_ComparePrefix(name, gUnk_0824DD20, 6)) {
            value = 9;
            goto selected;
        }
        if (!Text_ComparePrefix(name, gUnk_0824DD28, 4)) {
            value = 10;
            goto selected;
        }
        if (!Text_ComparePrefix(name, gUnk_0824DD30, 3)) {
            value = 11;
            goto selected;
        }
        if (!Text_ComparePrefix(name, gUnk_0824DD34, 6)) {
            value = 12;
            goto selected;
        }
        if (!Text_ComparePrefix(name, gUnk_0824DD3C, 4)) {
            value = 13;
            goto selected;
        }
        if (!Text_ComparePrefix(name, gUnk_0824DD44, 6)) {
            value = 14;
            goto selected;
        }
        if (!Text_ComparePrefix(name, gUnk_0824DD4C, 5)) {
            value = 15;
            goto selected;
        }
        if (!Text_ComparePrefix(name, gUnk_0824DD54, 7)) {
            value = 16;
            goto selected;
        }
        if (!Text_ComparePrefix(name, gUnk_0824DD5C, 7)) {
            value = 17;
            goto selected;
        }
        if (!Text_ComparePrefix(name, gUnk_0824DD64, 7)) {
            value = 18;
            goto selected;
        }
        if (!Text_ComparePrefix(name, gUnk_0824DD6C, 9)) {
            value = 19;
            goto selected;
        }
        if (!Text_ComparePrefix(name, gUnk_0824DD78, 4)) {
            value = 20;
            goto selected;
        }
        if (!Text_ComparePrefix(name, gUnk_0824DD80, 4)) {
            value = 21;
            goto selected;
        }
        if (!Text_ComparePrefix(name, gUnk_0824DD88, 4)) {
            value = 22;
            goto selected;
        }
        if (!Text_ComparePrefix(name, gUnk_0824DD90, 7)) {
            value = 23;
            goto selected;
        }
        if (!Text_ComparePrefix(name, gUnk_0824DD98, 9)) {
            value = 24;
            goto selected;
        }
        if (!Text_ComparePrefix(name, gUnk_0824DDA0, 7)) {
            value = 25;
            goto selected;
        }
        if (!Text_ComparePrefix(name, gUnk_0824DDA8, 7)) {
            value = 26;
            goto selected;
        }
        if (!Text_ComparePrefix(name, gUnk_0824DDB0, 6)) {
            value = 27;
            goto selected;
        }
        {
            register const u8 *alias asm("r6") = name;
            if (!Text_ComparePrefix(alias, gUnk_0824DDB8, 11)) {
                value = 30;
                goto selected;
            }
            if (!Text_ComparePrefix(alias, gUnk_0824DDC4, 6)) {
                value = TextState_GetPairSpeakerLabel();
                goto selected;
            }
        }
        if (!Text_ComparePrefix(name, gUnk_0824DDCC, 6)) {
            value = TextState_GetPlayerSpeakerLabel();
            goto selected;
        }
    unknown:
        value = 255;
    selected:
        p->selection = value;
        {
            register int zero asm("r0") = 0;
            int one = 1;
            p->busy = one;
            one = p->mode;
            p->previousMode = one;
            p->mode = zero;
        }
        findPtr = end;
        goto findEndCall;
    }
    if (!Text_ComparePrefix(s, gUnk_0824DDD4, 7)) {
        p->selection = -1;
        goto findS;
    }
    if (!Text_ComparePrefix(s, gUnk_0824DDDC, 4)) {
        end = TextState_ParseTagValue(p, end);
        if (p->hasValue && p->length)
            value = Text_ParseDecimal(p->text, p->length);
        else
            value = 0;
        p->frame = value;
        goto done;
    }
    if (!Text_ComparePrefix(s, gUnk_0824DDE4, 6)) {
        end = TextState_ParseTagValue(p, end);
        if (p->hasValue && p->length)
            value = Text_ParseDecimal(p->text, p->length);
        else
            value = 0;
        {
            u32 index = value << 2;
            register const u8 **slot asm("r0");
            slot = p->strings;
            slot = (const u8 **)((u8 *)slot + index);
            p->streams[2] = *slot;
        }
        pushP = p;
        pushStyle = 3;
        goto push;
    }
    if (!Text_ComparePrefix(s, gUnk_0824DDEC, 5)) {
        end = TextState_ParseTagValue(p, end);
        if (p->hasValue && p->length)
            value = Text_ParseDecimal(p->text, p->length);
        else
            value = 221;
        Sound_PlaySong(value);
        goto done;
    }
    if (!Text_ComparePrefix(s, gUnk_0824DDF4, 3)) {
        u8 *buffer;
        int number;
        end = TextState_ParseTagValue(p, end);
        if (p->hasValue && p->length)
            number = Text_ParseDecimal(p->text, p->length);
        else
            number = 0;
        buffer = p->numberText;
        {
            u32 index = number << 2;
            s32 *slot;
            slot = p->numbers;
            slot = (s32 *)((u8 *)slot + index);
            Text_FormatDecimal(buffer, *slot);
        }
        p->streams[1] = buffer;
        pushP = p;
        pushStyle = 2;
    push:
        TextState_PushStream(pushP, pushStyle);
    findAdvance:
        findPtr = end;
    findAdvanceCall:
        end = Text_FindChar(findPtr, '>');
        p->advance = 1;
        goto done;
    }
    cmp = Text_ComparePrefix(s, gUnk_0824DDF8, 6);
    if (!cmp) {
        const u8 *name;
        end = TextState_ParseTagValue(p, end);
        name = p->text;
        if (!Text_ComparePrefix(name, gUnk_0824DE00, 13)) {
            p->voice = cmp;
            goto done;
        }
        if (!Text_ComparePrefix(name, gUnk_0824DE10, 11))
            p->voice = 1;
        else
            p->voice = cmp;
        goto done;
    }
    if (!Text_ComparePrefix(s, gUnk_0824DE1C, 3)) {
        p->streams[0] = PlayerName_GetBuffer();
        end = Text_FindChar(end, '>');
        p->fresh = 1;
        goto done;
    }
    if (!Text_ComparePrefix(s, gUnk_0824DE20, 5)) {
        if (p->brackets) {
            TextState_PlayGlyphSound(p, 91);
            TextState_DrawNarrowGlyph(p, 59);
        }
        findPtr = end;
        asm("" : "+r"(findPtr));
        goto findEndCall;
    }
    if (!Text_ComparePrefix(s, gUnk_0824DE28, 5)) {
        p->palette = 1;
        findPtr = end;
        goto findEndCall;
    }
    goto done;
close:
    value = Text_ComparePrefix(s, gUnk_0824DE30, 7);
    if (!value)
        goto paletteClose;
    value = Text_ComparePrefix(s, gUnk_0824DE38, 7);
    if (!value)
        goto paletteClose;
    value = Text_ComparePrefix(s, gUnk_0824DE40, 7);
    if (!value)
        goto paletteClose;
    if (!Text_ComparePrefix(s, gUnk_0824DE48, 6)) {
        p->busy = 0;
        p->mode = p->previousMode;
        goto findS;
    }
    if (!Text_ComparePrefix(s, gUnk_0824DE50, 5)) {
        if (p->brackets) {
            TextState_PlayGlyphSound(p, 93);
            TextState_DrawNarrowGlyph(p, 61);
        }
        goto findS;
    }
    value = Text_ComparePrefix(s, gUnk_0824DE58, 6);
    if (value)
        goto done;
paletteClose:
    p->palette = value;
    goto findS;
palette:
    p->palette = paletteValue;
findS:
    findPtr = s;
    goto findEndCall;
findEndCall:
    end = Text_FindChar(findPtr, '>');
done:
    if (!p->allowRun)
        p->advance = 1;
    if (p->advance)
        end++;
    s = end;
    return s;
}
