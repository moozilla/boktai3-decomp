#include "text_sequence.h"

void TextSequence_SetState(struct TextSequence *p, u8 v)
{
    u8 *q = &p->phase;
    u32 z = 0;
    *q = v;
    *(u16 *)&p->timer = z;
}

void TextSequence_UpdateFadeIn(struct TextSequence *p, struct TextSequenceStep *e)
{
    if (p->timer == 0) {
        if (e->entry == 0x68) {
            TextSequence_SetState(p, 3);
            BestiaryDisplay_Hide(p->display);
            return;
        }
        if (e->entry == -1) {
            Task_Kill(p);
            BestiaryDisplay_Hide(p->display);
            return;
        }
        BestiaryDisplay_SetEntry(p->display, e->entry);
        BestiaryDisplay_Show(p->display);
        BestiaryDisplay_SetPosition(p->display, e->x, e->y);
        gObjFadeLevel = 0;
        gObjFadeColor = 0x1084;
    }
    if (++p->timer >= e->fadeInFrames) {
        gObjFadeLevel = 0x40;
        TextSequence_SetState(p, 1);
    } else {
        s32 level = Div(p->timer << 6, e->fadeInFrames);
        gObjFadeLevel = level;
    }
}

void TextSequence_UpdateHold(struct TextSequence *p, struct TextSequenceStep *step)
{
    s16 *c = &p->timer;
    (*c)++;
    if (*c >= step->holdFrames)
        TextSequence_SetState(p, 2);
}

void TextSequence_UpdateFadeOut(struct TextSequence *p, struct TextSequenceStep *step)
{
    s16 *timer = &p->timer;
    if (*timer == 0)
        gObjFadeColor = 0x1084;
    if (++*timer >= step->fadeOutFrames) {
        TextSequence_SetState(p, 0);
        do {
            ++p->stepIndex;
        } while (0);
        gObjFadeLevel = 0;
    } else {
        gObjFadeLevel = 64 - Div(*timer << 6, step->fadeOutFrames);
    }
}

void TextSequence_UpdateHiddenWait(struct TextSequence *p, struct TextSequenceStep *step)
{
    BestiaryDisplay_Hide(p->display);
    if (++p->timer > step->fadeOutFrames + step->fadeInFrames + step->holdFrames) {
        TextSequence_SetState(p, 0);
        ++p->stepIndex;
    }
}

void TextSequence_OnDestroy(struct TextSequence *p)
{
    BestiaryDisplay_Hide(p->display);
}

s32 TextSequence_Update(struct TextSequence *p)
{
    struct TextSequenceCallbacks callbacks = gUnk_0824FCA4;
    if (p->stepIndex <= p->lastStep) {
        struct TextSequenceStep *step = &p->steps[p->stepIndex];
        void *callback = callbacks.phase[p->phase];
        if (callback)
            sub_08249240(p, step, callback);
    }
    return 0;
}

s32 TextSequence_ReturnZero(void)
{
    return 0;
}

s32 TextSequence_InitializeFromScript(struct TextSequence *p, u32 arg)
{
    struct TextSequenceStep *e;
    s32 i;
    e = p->steps;
    if (Script_SeekToKeyword(0x74)) {
        Script_SetPc(sub_08227E9C());
        for (i = 0; i < 64; i++) {
            e->holdFrames = Script_GetValue();
            Script_GetValue();
            Script_GetValue();
            Script_GetValue();
            Script_GetValue();
            e->fadeInFrames = Script_GetValue();
            e->fadeOutFrames = Script_GetValue();
            if (e->holdFrames == -1) {
                p->lastStep = i - 1;
                break;
            }
            e++;
        }
    }
    e = p->steps;
    if (Script_SeekToKeyword(0x63)) {
        Script_SetPc(sub_08227E9C());
        for (i = 0; i < 64; i++) {
            e->x = Script_GetValue();
            e->y = Script_GetValue();
            if ((e->entry = Script_GetValue()) == -1)
                break;
            e++;
        }
    }
    p->timer = 0;
    p->stepIndex = 0;
    p->phase = 0;
    p->display = BestiaryDisplay_Create(arg, 0);
    BestiaryDisplay_Hide(p->display);
    BestiaryDisplay_SetPresentationMode(p->display, 1);
    return 0;
}

struct TextSequence *TextSequence_CreateFromScript(void *a)
{
    struct TextSequence *p = Task_Create(8, sizeof(*p));
    if (p) {
        Task_SetCallbacks(p, (void (*)(void *))TextSequence_Update,
                          (void (*)(void *))TextSequence_ReturnZero);
        if (TextSequence_InitializeFromScript(p, (u32)a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
