#include "display_sequence_sprite.h"
#include "global.h"
#include "ram.h"
#include "scene_input_widget.h"
#include "sprite.h"
#include "task.h"
struct ResultMenu {
    struct Task task;
    u32 timer;
    u8 state, entered;
    u8 pad1e[0x12];
    struct SceneInputResource primary, secondary;
    struct DisplaySequenceSprite sprite;
    struct SpriteRenderResource glyph;
    struct SceneInputWidget draws[22];
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
    struct SceneInputResource *res;
    struct SceneInputResource *first;
    int pack;
    struct SceneInputResource *primary;
    struct SceneInputResource *secondary;
    void *resource;
    void *glyph;
    struct DisplaySequenceState animation;
    u8 *priority;
    struct DisplaySequenceSprite *sprite;
    struct DisplaySequenceSprite *savedSprite;
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
    /* Both bounded resource guards remain unresolved allocation-only forms. */
    {
        u32 raw = (u8)(u32)res;
        if ((s32)raw < 0)
            res = 0;
    }
    p->primary = *res;
    first = &p->primary;
    ResourceHeader_Relocate(first, res);
    res = ScriptProc_Start(pack, 0x7C8B);
    primary = first;
    if (!res)
        goto failed;
    {
        u32 raw = (u8)(u32)res;
        if ((s32)raw < 0)
            res = 0;
    }
    p->secondary = *res;
    secondary = &p->secondary;
    ResourceHeader_Relocate(secondary, res);
    {
        struct SceneInputWidget *draw = p->draws;
        u32 drawFlags = 0x2011;
        Widget_InitLinked(draw, secondary, 30, drawFlags, 0, 0, 60, 0);
        draw = p->draws + 1;
        Widget_InitLinked(draw, secondary, 2, drawFlags, 0, 0, 60, 0);
        draw = p->draws + 2;
        glyph = &p->glyph;
        savedSprite = &p->sprite;
        priority = &p->sprite.priority;
        {
            int count;
            int zero = 0;
            count = 19;
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
    Anim_RestartSequence(&animation, resource, 0, 0, 0);
    sprite = savedSprite;
    {
        struct DisplaySequenceState *state = &animation;
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
            u8 *visible = priority;
            *visible = zero;
        }
        Memory_Copy(p->left, WirelessExchange_GetPeerEntry(0), 16);
        Memory_Copy(p->right, WirelessExchange_GetPeerEntry(1), 16);
        p->state = zero;
        p->entered = 1;
        p->timer = zero;
        {
            void **callback = &p->onClose;
            *callback = (void *)zero;
        }
    }
    return 0;
}
