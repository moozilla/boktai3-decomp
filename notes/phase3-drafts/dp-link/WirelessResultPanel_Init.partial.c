#include "display_sequence_sprite.h"
// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "ram.h"
struct Blob {
    u32 words[8];
};
struct Drawing {
    u8 bytes[0x60];
};
struct ResultMenu {
    u8 task[0x18];
    u32 timer;
    u8 state, entered;
    u8 pad1e[0x12];
    struct Blob primary, secondary;
    struct DisplaySequenceSprite sprite;
    u8 glyph[0x1c];
    struct Drawing draws[22];
    void *onClose, *resource;
    int windows[4];
    u8 left[16], right[16];
};
int Script_SeekToKeyword(int);
void *Script_GetTextSourcePc(void);
void Widget_InitLinked(void *, void *, int, int, int, int, int, void *);
void Anim_RestartSequence(void *, void *, int, int, int);
void *WirelessExchange_GetPeerEntry(int);
int WirelessResultPanel_Init(struct ResultMenu *arg)
{
    struct ResultMenu *p = arg;
    struct Blob *res;
    struct Blob *first;
    int pack;
    struct Blob *primary;
    struct Blob *secondary;
    int i;
    void *resource;
    void *glyph;
    struct Saved {
        struct DisplaySequenceState animation;
        u8 *visible;
    } saved;
    struct DisplaySequenceState *a;
    const u16 *frame;
    struct DisplaySequenceSprite *sprite;
    struct DisplaySequenceSprite *savedSprite;
    int flags;
    gResultMenu = p;
    if (Script_SeekToKeyword(115)) {
        void *value = Script_GetTextSourcePc();
        void **slot = &p->resource;
        *slot = value;
        if (value)
            goto initialize;
    }
failed:
    return -1;
initialize:
    pack = 0xCB05;
    res = ScriptProc_Start(pack, 0x530D);
    if (!res)
        goto failed;
    asm("" : "+r"(res));
    p->primary = *res;
    first = &p->primary;
    ResourceHeader_Relocate(first, res);
    res = ScriptProc_Start(pack, 0x7C8B);
    primary = first;
    if (!res)
        goto failed;
    asm("" : "+r"(res));
    p->secondary = *res;
    secondary = &p->secondary;
    ResourceHeader_Relocate(secondary, res);
    {
        struct Drawing *draw = p->draws;
        u32 drawFlags = 0x2011;
        Widget_InitLinked(draw, secondary, 30, drawFlags, 0, 0, 60, 0);
        draw = p->draws + 1;
        Widget_InitLinked(draw, secondary, 2, drawFlags, 0, 0, 60, 0);
        draw = p->draws + 2;
        glyph = p->glyph;
        savedSprite = &p->sprite;
        {
            register u8 *visible asm("r1") = &p->sprite.priority;
            saved.visible = visible;
        }
        {
            register int zero asm("r5") = 0;
            int count = 19;
            do {
                Widget_InitLinked(draw, primary, 51, 0x2011, zero, zero, 60, (void *)zero);
                count--;
                draw++;
            } while (count >= 0);
        }
    }
    resource = ScriptProc_Start(0x922E, 0x4BBF);
    AnimData_Load(glyph, 0x593A);
    AnimData_SelectPaletteBank(glyph, 503);
    Sprite_Init(savedSprite, glyph, 0x2011);
    Anim_RestartSequence(&saved.animation, resource, 0, 0, 0);
    sprite = savedSprite;
    {
    struct DisplaySequenceState *state = &saved.animation;
    const u16 *command;
    u32 ended;
    command = state->commands + state->index;
    sprite->frame = *command >> 6;
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 1;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            p->sprite.flags |= 4;
        else
            p->sprite.flags &= ~4;
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
    }
    if (++state->elapsed >= state->duration) {
        state->elapsed = 0;
        if (state->flags & 4) {
            if (state->index == 0) {
                if (state->flags & 8)
                    state->elapsed = state->duration;
                else
                    state->index = state->frameCount - 1;
                ended = 1;
            } else {
                state->index--;
                ended = 0;
            }
        } else if (++state->index >= state->frameCount) {
            if ((u8)(state->flags & 8)) {
                state->elapsed = state->duration;
                state->index = state->frameCount - 1;
                ended = 1;
            } else {
                state->index = 0;
                ended = 1;
            }
        } else
            ended = 0;
        command = state->commands + state->index;
        DisplaySequence_UpdateWeight(state, command);
        if (!state->duration)
            state->duration = 1;
    } else
        ended = 0;

    }
    {
        int zero = 0;
        {
            u8 *visible = saved.visible;
            *visible = zero;
        }
        Memory_Copy(p->left, WirelessExchange_GetPeerEntry(0), 16);
        Memory_Copy(p->right, WirelessExchange_GetPeerEntry(1), 16);
        {
            struct ResultMenu *write = p;
            int one;
            one = 1;
            write->state = zero;
            write->entered = one;
            write->timer = zero;
        }
        {
            void **callback = &p->onClose;
            *callback = (void *)zero;
        }
    }
    return 0;
}
