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
const u8 *TextState_ProcessTag(struct MessagePlaybackState *state, const u8 *tag)
{
    const u8 *end = tag;
    int result;

    if (*tag != '/') {
        if (!Text_ComparePrefix(tag, gUnk_0824DC80, 6)) {
            state->palette = 1;
            end = Text_FindChar(tag, '>');
        } else if (!Text_ComparePrefix(tag, gUnk_0824DC88, 6)) {
            state->palette = 2;
            end = Text_FindChar(tag, '>');
        } else if (!Text_ComparePrefix(tag, gUnk_0824DC90, 6)) {
            state->palette = 2;
            end = Text_FindChar(tag, '>');
        } else if (!Text_ComparePrefix(tag, gUnk_0824DC98, 4)) {
            end = TextState_ParseTagValue(state, tag);
            if (state->tag.hasValue && state->tag.length)
                state->characterDelay = Text_ParseDecimal(state->tag.value, state->tag.length);
            else {
                int delay = (s16)gGameState->unk12;

                state->characterDelay = delay;
            }
        } else if (!Text_ComparePrefix(tag, gUnk_0824DCA0, 4)) {
            int choice;

            end = TextState_ParseTagValue(state, tag);
            if (state->tag.hasValue && state->tag.length)
                choice = Text_ParseDecimal(state->tag.value, state->tag.length);
            else
                choice = 0;
            state->selectedChoices[state->selectedCount] = choice;
            state->selectedCount++;
        } else if (!Text_ComparePrefix(tag, gUnk_0824DCA8, 4)) {
            state->streams[0] = PlayerName_GetBuffer();
            TextState_PushStream(state, 1);
            end = Text_FindChar(tag, '>');
            state->hold = 1;
        } else if (!Text_ComparePrefix(tag, gUnk_0824DCB0, 4)) {
            state->streams[3] = GameState_GetBikeName();
            TextState_PushStream(state, 4);
            end = Text_FindChar(tag, '>');
            state->hold = 1;
        } else if (!Text_ComparePrefix(tag, gUnk_0824DCB8, 5)) {
            const u8 *name;
            int speaker;

            end = TextState_ParseTagValue(state, tag);
            if (!state->tag.hasValue || !state->tag.length) {
                speaker = 255;
            } else {
                name = state->tag.value;
                if (!Text_ComparePrefix(name, gUnk_0824DCC0, 4))
                    speaker = 255;
                else if ((speaker = Text_ComparePrefix(name, gUnk_0824DCC8, 8)) &&
                         (speaker = Text_ComparePrefix(name, gUnk_0824DCD4, 8)) &&
                         (speaker = Text_ComparePrefix(name, gUnk_0824DCE0, 8))) {
                    if (!Text_ComparePrefix(name, gUnk_0824DCEC, 6))
                        speaker = 3;
                    else if (!Text_ComparePrefix(name, gUnk_0824DCF4, 8))
                        speaker = 5;
                    else if (!Text_ComparePrefix(name, gUnk_0824DD00, 6))
                        speaker = 4;
                    else if (!Text_ComparePrefix(name, gUnk_0824DD08, 5))
                        speaker = 6;
                    else if (!Text_ComparePrefix(name, gUnk_0824DD10, 4))
                        speaker = 7;
                    else if (!Text_ComparePrefix(name, gUnk_0824DD18, 4))
                        speaker = 8;
                    else if (!Text_ComparePrefix(name, gUnk_0824DD20, 6))
                        speaker = 9;
                    else if (!Text_ComparePrefix(name, gUnk_0824DD28, 4))
                        speaker = 10;
                    else if (!Text_ComparePrefix(name, gUnk_0824DD30, 3))
                        speaker = 11;
                    else if (!Text_ComparePrefix(name, gUnk_0824DD34, 6))
                        speaker = 12;
                    else if (!Text_ComparePrefix(name, gUnk_0824DD3C, 4))
                        speaker = 13;
                    else if (!Text_ComparePrefix(name, gUnk_0824DD44, 6))
                        speaker = 14;
                    else if (!Text_ComparePrefix(name, gUnk_0824DD4C, 5))
                        speaker = 15;
                    else if (!Text_ComparePrefix(name, gUnk_0824DD54, 7))
                        speaker = 16;
                    else if (!Text_ComparePrefix(name, gUnk_0824DD5C, 7))
                        speaker = 17;
                    else if (!Text_ComparePrefix(name, gUnk_0824DD64, 7))
                        speaker = 18;
                    else if (!Text_ComparePrefix(name, gUnk_0824DD6C, 9))
                        speaker = 19;
                    else if (!Text_ComparePrefix(name, gUnk_0824DD78, 4))
                        speaker = 20;
                    else if (!Text_ComparePrefix(name, gUnk_0824DD80, 4))
                        speaker = 21;
                    else if (!Text_ComparePrefix(name, gUnk_0824DD88, 4))
                        speaker = 22;
                    else if (!Text_ComparePrefix(name, gUnk_0824DD90, 7))
                        speaker = 23;
                    else if (!Text_ComparePrefix(name, gUnk_0824DD98, 9))
                        speaker = 24;
                    else if (!Text_ComparePrefix(name, gUnk_0824DDA0, 7))
                        speaker = 25;
                    else if (!Text_ComparePrefix(name, gUnk_0824DDA8, 7))
                        speaker = 26;
                    else if (!Text_ComparePrefix(name, gUnk_0824DDB0, 6))
                        speaker = 27;
                    else {
                        result = (int)name;
                        if (!Text_ComparePrefix((const u8 *)result, gUnk_0824DDB8, 11))
                            speaker = 30;
                        else if (!Text_ComparePrefix((const u8 *)result, gUnk_0824DDC4, 6))
                            speaker = TextState_GetPairSpeakerLabel();
                        else {
                            result = 0;
                            if (!Text_ComparePrefix(name, gUnk_0824DDCC, 6))
                                speaker = TextState_GetPlayerSpeakerLabel();
                            else
                                speaker = 255;
                        }
                    }
                }
            }
            {
                s8 *selection = &state->speaker;

                *selection = speaker;
            }
            state->busy = 1;
            state->defaultDelay = state->characterDelay;
            state->characterDelay = 0;
            end = Text_FindChar(end, '>');
        } else if (!Text_ComparePrefix(tag, gUnk_0824DDD4, 7)) {
            state->speaker = -1;
            end = Text_FindChar(tag, '>');
        } else if (!Text_ComparePrefix(tag, gUnk_0824DDDC, 4)) {
            int value;

            end = TextState_ParseTagValue(state, end);
            if (state->tag.hasValue && state->tag.length)
                value = Text_ParseDecimal(state->tag.value, state->tag.length);
            else
                value = 0;
            state->frame = value;
        } else if (!Text_ComparePrefix(tag, gUnk_0824DDE4, 6)) {
            int value;

            end = TextState_ParseTagValue(state, end);
            if (state->tag.hasValue && state->tag.length)
                value = Text_ParseDecimal(state->tag.value, state->tag.length);
            else
                value = 0;
            state->streams[2] = state->strings[value];
            TextState_PushStream(state, 3);
            end = Text_FindChar(end, '>');
            state->hold = 1;
        } else if (!Text_ComparePrefix(tag, gUnk_0824DDEC, 5)) {
            int value;

            end = TextState_ParseTagValue(state, end);
            if (state->tag.hasValue && state->tag.length)
                value = Text_ParseDecimal(state->tag.value, state->tag.length);
            else
                value = 221;
            Sound_PlaySong(value);
        } else if (!Text_ComparePrefix(tag, gUnk_0824DDF4, 3)) {
            int number;
            u8 *buffer;

            end = TextState_ParseTagValue(state, end);
            if (state->tag.hasValue && state->tag.length)
                number = Text_ParseDecimal(state->tag.value, state->tag.length);
            else
                number = 0;
            buffer = state->tag.number;
            Text_FormatDecimal(buffer, state->numbers[number]);
            state->streams[1] = buffer;
            TextState_PushStream(state, 2);
            end = Text_FindChar(end, '>');
            state->hold = 1;
        } else if (!(result = Text_ComparePrefix(tag, gUnk_0824DDF8, 6))) {
            const u8 *name;

            end = TextState_ParseTagValue(state, end);
            name = state->tag.value;
            if (!Text_ComparePrefix(name, gUnk_0824DE00, 13))
                state->spacing = 0;
            else if (!Text_ComparePrefix(name, gUnk_0824DE10, 11))
                state->spacing = 1;
            else
                state->spacing = 0;
        } else if (!Text_ComparePrefix(tag, gUnk_0824DE1C, 3)) {
            state->streams[0] = PlayerName_GetBuffer();
            end = Text_FindChar(end, '>');
            state->complete = 1;
        } else if (!Text_ComparePrefix(tag, gUnk_0824DE20, 5)) {
            if (state->unknown10) {
                TextState_PlayGlyphSound(state, '[');
                TextState_DrawNarrowGlyph(state, ';');
            }
            end = Text_FindChar(end, '>');
        } else if (!Text_ComparePrefix(tag, gUnk_0824DE28, 5)) {
            state->palette = 1;
            end = Text_FindChar(end, '>');
        }
    } else if (!Text_ComparePrefix(tag, gUnk_0824DE30, 7)) {
        state->palette = 0;
        end = Text_FindChar(tag, '>');
    } else if (!Text_ComparePrefix(tag, gUnk_0824DE38, 7)) {
        state->palette = 0;
        end = Text_FindChar(tag, '>');
    } else if (!Text_ComparePrefix(tag, gUnk_0824DE40, 7)) {
        state->palette = 0;
        end = Text_FindChar(tag, '>');
    } else if (!Text_ComparePrefix(tag, gUnk_0824DE48, 6)) {
        state->busy = 0;
        state->characterDelay = state->defaultDelay;
        end = Text_FindChar(tag, '>');
    } else if (!Text_ComparePrefix(tag, gUnk_0824DE50, 5)) {
        if (state->unknown10) {
            TextState_PlayGlyphSound(state, ']');
            TextState_DrawNarrowGlyph(state, '=');
        }
        end = Text_FindChar(tag, '>');
    } else if (!Text_ComparePrefix(tag, gUnk_0824DE58, 6)) {
        state->palette = 0;
        end = Text_FindChar(tag, '>');
    }
    if (!state->unknown14)
        state->hold = 1;
    if (state->hold)
        end++;
    tag = end;
    return tag;
}
