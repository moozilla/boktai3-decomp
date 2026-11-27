#include "ui/dialogue/message_window.h"

int Text_ComparePrefix(const u8 *, const u8 *, int);
const u8 *TextState_ParseTagValue(struct MessagePlaybackState *, const u8 *);
int Text_ParseDecimal(const u8 *, int);
void TextState_PushStream(struct MessagePlaybackState *, int);
void TextState_PlayGlyphSound(struct MessagePlaybackState *, int);
int TextState_DrawNarrowGlyph(struct MessagePlaybackState *, int);
void Text_FormatDecimal(u8 *, int);
const u8 *Text_FindChar(const u8 *, u8);
const u8 *PlayerName_GetBuffer(void);
const u8 *GameState_GetBikeName(void);
int TextState_GetPairSpeakerLabel(void);
int TextState_GetPlayerSpeakerLabel(void);
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
const u8 *TextState_ProcessTag(struct MessagePlaybackState *p, const u8 *s)
{
    const u8 *end = s;
    int value;
    int cmp;
    if (*s == '/')
        goto close;
    if (!Text_ComparePrefix(s, gUnk_0824DC80, 6)) {
        p->palette = 1;
        goto findS;
    }
    if (!Text_ComparePrefix(s, gUnk_0824DC88, 6)) {
        p->palette = 2;
        goto findS;
    }
    if (!Text_ComparePrefix(s, gUnk_0824DC90, 6)) {
        p->palette = 2;
        goto findS;
    }
    if (!Text_ComparePrefix(s, gUnk_0824DC98, 4)) {
        end = TextState_ParseTagValue(p, s);
        if (p->tag.hasValue && p->tag.length)
            p->characterDelay = Text_ParseDecimal(p->tag.value, p->tag.length);
        else {
            int delay = (s16)gGameState->unk12;
            p->characterDelay = delay;
        }
        goto done;
    }
    if (!Text_ComparePrefix(s, gUnk_0824DCA0, 4)) {
        int value;
        end = TextState_ParseTagValue(p, s);
        if (p->tag.hasValue && p->tag.length)
            value = Text_ParseDecimal(p->tag.value, p->tag.length);
        else
            value = 0;
        p->selectedChoices[p->selectedCount] = value;
        p->selectedCount++;
        goto done;
    }
    if (!Text_ComparePrefix(s, gUnk_0824DCA8, 4)) {
        p->streams[0] = PlayerName_GetBuffer();
        TextState_PushStream(p, 1);
        end = Text_FindChar(s, '>');
        p->hold = 1;
        goto done;
    }
    if (!Text_ComparePrefix(s, gUnk_0824DCB0, 4)) {
        p->streams[3] = GameState_GetBikeName();
        TextState_PushStream(p, 4);
        end = Text_FindChar(s, '>');
        p->hold = 1;
        goto done;
    }
    if (!Text_ComparePrefix(s, gUnk_0824DCB8, 5)) {
        const u8 *name;
        end = TextState_ParseTagValue(p, s);
        if (!p->tag.hasValue)
            goto unknown;
        if (!p->tag.length)
            goto unknown;
        name = p->tag.value;
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
        cmp = (int)name;
        if (!Text_ComparePrefix((const u8 *)cmp, gUnk_0824DDB8, 11)) {
            value = 30;
            goto selected;
        }
        if (!Text_ComparePrefix((const u8 *)cmp, gUnk_0824DDC4, 6)) {
            value = TextState_GetPairSpeakerLabel();
            goto selected;
        }
        cmp = 0;
        if (!Text_ComparePrefix(name, gUnk_0824DDCC, 6)) {
            value = TextState_GetPlayerSpeakerLabel();
            goto selected;
        }
    unknown:
        value = 255;
    selected: {
        s8 *selection = &p->speaker;
        *selection = value;
    }
        p->busy = 1;
        p->defaultDelay = p->characterDelay;
        p->characterDelay = 0;
        end = Text_FindChar(end, '>');
        goto done;
    }
    if (!Text_ComparePrefix(s, gUnk_0824DDD4, 7)) {
        p->speaker = -1;
        goto findS;
    }
    if (!Text_ComparePrefix(s, gUnk_0824DDDC, 4)) {
        end = TextState_ParseTagValue(p, end);
        if (p->tag.hasValue && p->tag.length)
            value = Text_ParseDecimal(p->tag.value, p->tag.length);
        else
            value = 0;
        p->frame = value;
        goto done;
    }
    if (!Text_ComparePrefix(s, gUnk_0824DDE4, 6)) {
        end = TextState_ParseTagValue(p, end);
        if (p->tag.hasValue && p->tag.length)
            value = Text_ParseDecimal(p->tag.value, p->tag.length);
        else
            value = 0;
        p->streams[2] = p->strings[value];
        TextState_PushStream(p, 3);
        end = Text_FindChar(end, '>');
        p->hold = 1;
        goto done;
    }
    if (!Text_ComparePrefix(s, gUnk_0824DDEC, 5)) {
        end = TextState_ParseTagValue(p, end);
        if (p->tag.hasValue && p->tag.length)
            value = Text_ParseDecimal(p->tag.value, p->tag.length);
        else
            value = 221;
        Sound_PlaySong(value);
        goto done;
    }
    if (!Text_ComparePrefix(s, gUnk_0824DDF4, 3)) {
        u8 *buffer;
        int number;
        end = TextState_ParseTagValue(p, end);
        if (p->tag.hasValue && p->tag.length)
            number = Text_ParseDecimal(p->tag.value, p->tag.length);
        else
            number = 0;
        buffer = p->tag.number;
        Text_FormatDecimal(buffer, p->numbers[number]);
        p->streams[1] = buffer;
        TextState_PushStream(p, 2);
        end = Text_FindChar(end, '>');
        p->hold = 1;
        goto done;
    }
    cmp = Text_ComparePrefix(s, gUnk_0824DDF8, 6);
    if (!cmp) {
        const u8 *name;
        end = TextState_ParseTagValue(p, end);
        name = p->tag.value;
        if (!Text_ComparePrefix(name, gUnk_0824DE00, 13)) {
            p->spacing = cmp;
            goto done;
        }
        if (!Text_ComparePrefix(name, gUnk_0824DE10, 11))
            p->spacing = 1;
        else
            p->spacing = cmp;
        goto done;
    }
    if (!Text_ComparePrefix(s, gUnk_0824DE1C, 3)) {
        p->streams[0] = PlayerName_GetBuffer();
        end = Text_FindChar(end, '>');
        p->complete = 1;
        goto done;
    }
    if (!Text_ComparePrefix(s, gUnk_0824DE20, 5)) {
        if (p->unknown10) {
            TextState_PlayGlyphSound(p, 91);
            TextState_DrawNarrowGlyph(p, 59);
        }
        end = Text_FindChar(end, '>');
        goto done;
    }
    if (!Text_ComparePrefix(s, gUnk_0824DE28, 5)) {
        p->palette = 1;
        end = Text_FindChar(end, '>');
        goto done;
    }
    goto done;
close:
    if (!Text_ComparePrefix(s, gUnk_0824DE30, 7)) {
        p->palette = 0;
        goto findS;
    }
    if (!Text_ComparePrefix(s, gUnk_0824DE38, 7)) {
        p->palette = 0;
        goto findS;
    }
    if (!Text_ComparePrefix(s, gUnk_0824DE40, 7)) {
        p->palette = 0;
        goto findS;
    }
    if (!Text_ComparePrefix(s, gUnk_0824DE48, 6)) {
        p->busy = 0;
        p->characterDelay = p->defaultDelay;
        goto findS;
    }
    if (!Text_ComparePrefix(s, gUnk_0824DE50, 5)) {
        if (p->unknown10) {
            TextState_PlayGlyphSound(p, 93);
            TextState_DrawNarrowGlyph(p, 61);
        }
        goto findS;
    }
    if (Text_ComparePrefix(s, gUnk_0824DE58, 6))
        goto done;
    p->palette = 0;
findS:
    end = Text_FindChar(s, '>');
done:
    if (!p->unknown14)
        p->hold = 1;
    if (p->hold)
        end++;
    s = end;
    return s;
}
