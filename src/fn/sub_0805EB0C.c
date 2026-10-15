// MATCHING_CONCESSION: register-pinning
#include "scene_input_panel.h"
#define SceneInputPanel_InitWidgets sub_0805EB0C
void SceneInputPanel_InitWidgets(struct SceneInputPanel *owner, s32 mode)
{
    register struct SceneInputPanel *incoming asm("r0") = owner;
    struct SceneInputPanel *panel;
    struct SceneInputPackedPosition position;
    struct DisplaySequenceState sequence;
    struct SceneInputResource *source;
    void *sequenceResource;
    register const volatile u16 *command asm("r6");
    register struct DisplaySequenceState *state asm("r5");
    register struct SceneInputPanelDrawable *drawable asm("r6");
    void *graphics;
    struct SceneInputPanelDrawable *active;
    u32 flags, duration, mask;
    register u32 reverseDuration asm("r6");
    register u32 bit asm("r3");
    s32 i;
    panel = incoming;
    source = ScriptProc_Start(0xCB05, 0x5D04);
    panel->gridResource = *source;
    ResourceHeader_Relocate(&panel->gridResource, source);
    source = ScriptProc_Start(0xCB05, 0x530D);
    panel->widgetResource = *source;
    ResourceHeader_Relocate(&panel->widgetResource, source);
    {
        struct SceneInputPackedPosition *initialPosition;
        position.z = position.y = position.x = 0;
        initialPosition = &position;
        {
            register u32 zero asm("r3") = 0;
            sub_0821983C(&panel->indicator, &panel->widgetResource, gUnk_0824DEEA[mode], 0x30, zero,
                         zero, 60, initialPosition);
            asm("" : : : "r10");
        }
    }
    sequenceResource = ScriptProc_Start(0x922E, 0x4BBF);
    graphics = panel->drawable.graphics;
    AnimData_Load(graphics, gUnk_0824DF00[mode]);
    AnimData_SelectPaletteBank(graphics, gUnk_0824DF14[mode]);
    drawable = &panel->drawable;
    Sprite_Init(drawable, graphics, 0x30);
    sub_08220D20(&sequence, sequenceResource, gUnk_0824DF1E[mode], 0, 0);
    active = drawable;
    asm("" : "+r"(active));
    state = &sequence;
    asm("" : "+r"(state));
    {
        register u32 offset asm("r0") = state->index << 1;
        register const u16 *table asm("r1");
        asm("" : "+r"(offset));
        table = sequence.commands;
        asm("" : "+r"(table));
        command = (void *)((u32)table + offset);
    }
    {
        u32 frame = *command >> 6;
        register struct SceneInputPanelDrawable *target asm("r1") = active;
        asm("" : "+r"(target));
        target->frame = frame;
    }
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        bit = 1;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit))
            panel->drawable.flags |= 4;
        else
            panel->drawable.flags &= ~4;
    }
    flags = state->flags;
    {
        register u32 first asm("r1");
        bit = 2;
        first = bit;
        first &= flags;
        first = (u8)first;
        if (first != (((*command & 0x30) >> 4) & bit)) {
            register struct SceneInputPanelDrawable *target asm("r2") = active;
            target->flags |= 8;
        } else {
            register struct SceneInputPanelDrawable *target asm("r3") = active;
            target->flags &= ~8;
        }
    }
    {
        u32 elapsed = state->elapsed + 1;
        state->elapsed = elapsed;
        {
            register u32 temp asm("r6") = 0xFFFF;
            u32 savedMask;
            asm("" : "+r"(temp));
            savedMask = temp;
            mask = savedMask;
        }
        duration = state->duration;
        elapsed = (u16)elapsed;
        reverseDuration = duration;
        if (elapsed >= duration) {
            state->elapsed = 0;
            {
                register u32 reverseFlags asm("r2") = flags;
                if (reverseFlags & 4) {
                    u32 index = state->index;
                    if (index == 0) {
                        if (reverseFlags & 8)
                            state->elapsed = reverseDuration;
                        else
                            state->index = state->frameCount - 1;
                    } else
                        state->index = index - 1;
                } else {
                    u32 index = state->index + 1;
                    u32 frameCount;
                    state->index = index;
                    frameCount = state->frameCount;
                    if ((index & mask) >= frameCount) {
                        if (({
                                u32 endFlag = flags & 8;
                                asm("" : "+r"(endFlag));
                                (u8) endFlag;
                            })) {
                            state->elapsed = duration;
                            {
                                u32 lastIndex = frameCount - 1;
                                state->index = lastIndex;
                            }
                        } else
                            state->index = 0;
                    }
                }
            }
            {
                register u32 offset asm("r0") = state->index << 1;
                register const u16 *table asm("r1");
                table = state->commands;
                command = (void *)((u32)table + offset);
            }
            state->frameWeight = *command & 15;
            {
                register s32 weight asm("r1") = state->frameWeight;
                s32 speed = state->speed;
                speed = (speed * weight) >> 6;
                state->duration = speed;
                if (!(u8)speed)
                    state->duration = 1;
            }
        }
    }
    panel->drawable.priority = 0;
    panel->drawable.position.x = 8;
    panel->drawable.position.y = 80;
    {
        register struct SceneInputResource *widgets asm("r6");
        {
            struct SceneInputWidget *modeWidget = &panel->mode;
            widgets = &panel->widgetResource;
            sub_0821983C(modeWidget, widgets, gUnk_0824DEF4[mode], 0x30, 0, 0, 60, &position);
        }
        sub_0821983C(&panel->valueA, widgets, 0x41, 0x31, 0, 0, 60, &position);
        position.x = 8;
        sub_0821983C(&panel->valueB, widgets, 0x42, 0x31, 0, 0, 60, &position);
        {
            u32 cachedMode;
            struct SceneInputResource *lateResource;
            register struct SceneInputResource *grid asm("r10");
            {
                register u32 offset asm("r5") = (u32) & ((struct SceneInputPanel *)0)->gridResource;
                asm("" : "+r"(offset));
                offset += (u32)panel;
                grid = (void *)offset;
            }
            lateResource = widgets;
            {
                register u32 copy asm("r6") = mode << 1;
                asm("" : "+r"(copy));
                cachedMode = copy;
            }
            for (i = 0; i <= 15; i++) {
                position.x = (i & 3) * 24 + 112;
                position.y = (i >> 2) * 24 + 32;
                sub_0821983C(&panel->primary[i], grid, 0x151, 0x31, 0, 0, 60, &position);
            }
            for (i = 0; i <= 7; i++) {
                position.x = (i & 3) * 24 + 8;
                position.y = (i >> 2) * 24 + 32;
                sub_0821983C(&panel->secondary[i], grid, 0x151, 0x31, 0, 0, 60, &position);
            }
            position.x = 0;
            position.y = 128;
            sub_0821983C(&panel->banner, grid, 0x189, 0x31, 0, 0, 60, &position);
            position.x = 216;
            position.y = 56;
            sub_0821983C(&panel->buttons[1], grid, 0x192, 0x30, 0, 0, 60, &position);
            position.x = 216;
            position.y = 80;
            sub_0821983C(&panel->buttons[2], grid, 0x191, 0x30, 0, 0, 60, &position);
            position.x = 216;
            position.y = 32;
            sub_0821983C(&panel->buttons[0], grid, 0x18F, 0x30, 0, 0, 60, &position);
            position.x = 216;
            position.y = 104;
            sub_0821983C(&panel->buttons[3], grid, 0x190, 0x30, 0, 0, 60, &position);
            sub_0821983C(&panel->category, lateResource, 0x4E, 0x31, 0, 0, 60, &position);
            position.x = 112;
            position.y = 32;
            sub_0821983C(&panel->compareA, lateResource, 0x4D, 0x31, 0, 0, 60, &position);
            position.x = 216;
            position.y = 32;
            sub_0821983C(&panel->modeCursor, lateResource, 0x4D, 0x30, 0, 0, 60, &position);
            position.x = 0;
            position.y = 0;
            sub_0821983C(&panel->heading, lateResource,
                         *(const u16 *)((const u8 *)gUnk_0824DEE0 + cachedMode), 0x30, 0, 0, 60,
                         &position);
            position.x = 8;
            position.y = 0;
            sub_0821983C(&panel->valueLabel, lateResource, 0x40, 0x30, 0, 0, 60, &position);
            position.x = 0;
            position.y = 0;
            sub_0821983C(&panel->glyph, lateResource, 0x67, 0x30, 0, 0, 60, &position);
            position.x = 56;
            position.y = 0;
            sub_0821983C(&panel->meterGlyph, lateResource, 0x68, 0x30, 0, 0, 60, &position);
            {
                u32 cachedPriority;
                {
                    u32 zero = 0;
                    struct SceneInputWidget *widget = panel->digits;
                    for (i = 7; i >= 0; i--) {
                        sub_0821983C(widget, lateResource, 0x50, 0x30, zero, zero, ({
                                         register u32 priority asm("r1") = 60;
                                         cachedPriority = priority;
                                         priority;
                                     }),
                                     ({
                                         register struct SceneInputPackedPosition *out asm("r2") =
                                             &position;
                                         out;
                                     }));
                        widget++;
                    }
                }
                {
                    sub_0821983C(({
                                     register u32 offset asm("r3") =
                                         (u32) & ((struct SceneInputPanel *)0)->numericLabel;
                                     asm("" : "+r"(offset));
                                     (void *)((u32)panel + offset);
                                 }),
                                 lateResource, 0xA9, 0x31, 0, 0, ({
                                     register u32 priority asm("r4") = cachedPriority;
                                     asm("" : "+r"(priority));
                                     priority;
                                 }),
                                 ({
                                     register struct SceneInputPackedPosition *out asm("r5") =
                                         &position;
                                     out;
                                 }));
                }
            }
            {
                struct SceneInputPackedPosition *cachedPosition;
                register struct SceneInputPackedPosition *lastPosition asm("r2");
                register u32 cachedPriority asm("r9");
                {
                    struct SceneInputPackedPosition *work = &position;
                    asm("" : "+r"(work));
                    work->x -= 11;
                    cachedPosition = work;
                }
                {
                    register struct SceneInputWidget *widget asm("r5");
                    register u32 zero asm("r6");
                    {
                        register u32 offset asm("r6") =
                            (u32) & ((struct SceneInputPanel *)0)->smallDigits;
                        asm("" : "+r"(offset));
                        widget = (void *)((u32)panel + offset);
                    }
                    zero = 0;
                    for (i = 3; i >= 0; i--) {
                        sub_0821983C(widget, lateResource, 0xAA, 0x31, zero, zero, ({
                                         register u32 priority asm("r0") = 60;
                                         cachedPriority = priority;
                                         priority;
                                     }),
                                     ({
                                         register struct SceneInputPackedPosition *out asm("r1") =
                                             &position;
                                         out;
                                     }));
                        lastPosition = cachedPosition;
                        lastPosition->x += 6;
                        widget++;
                    }
                }
                {
                    struct SceneInputPackedPosition *out;
                    position.x = 0;
                    asm("" : : "r"(lastPosition));
                    out = &position;
                    out->y = 0;
                    sub_0821983C(({
                                     register u32 offset asm("r4") =
                                         (u32) & ((struct SceneInputPanel *)0)->prompt;
                                     (void *)((u32)panel + offset);
                                 }),
                                 lateResource, 0x4B, 0x11, 0, 0, ({
                                     u32 priority = cachedPriority;
                                     priority;
                                 }),
                                 out);
                }
            }
        }
    }
}
