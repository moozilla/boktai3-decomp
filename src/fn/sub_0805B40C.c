#include "scene_panel_interaction.h"
// MATCHING_CONCESSION: register-pinning

void ScenePanel_AdvanceTetherAcquisition(struct ScenePanel *input)
{
    struct ScenePanelInteractionView *panel = (void *)input;
    register u16 *framePointer asm("r9") = &panel->frame;
    u32 frame;
    asm("" : "+r"(panel));
    {
        u32 value = *framePointer;
        value++;
        *framePointer = value;
        frame = (u16)value;
    }
    if (frame <= 5) {
        struct ScenePanelInteractionPackedPosition position;
        s32 divisor;
        s32 previousWeight;
        s32 sourceX;
        u32 direction;
        {
            register u16 *sourceFrame asm("r2") = framePointer;
            divisor = 6 - *sourceFrame;
        }
        sourceX = gUnk_02000488->position.x;
        previousWeight = divisor - 1;
        position.x = Div(sourceX * previousWeight + panel->center.x, divisor);
        position.z = Div(gUnk_02000488->position.z * previousWeight + panel->center.z, divisor);
        direction = gUnk_02000488->heading;
        if (*framePointer & 1) {
            u32 facing = direction;
            asm("" : "+r"(facing));
            if (facing) {
                if (facing > 4)
                    direction++;
                else
                    direction--;
                direction &= 7;
            }
        }
        TetherActor_SetInteractionTarget(&position, direction);
    } else if (frame == 6) {
        gUnk_02000488->state = 12;
        gUnk_02000488->position = panel->center;
        {
            u8 *heading = &gUnk_02000488->heading;
            register u32 zero asm("r0");
            zero = 0;
            *heading = zero;
        }
        panel->mainSpriteFlags |= 1;
        Sound_PlaySong(249);
    } else if (frame <= 69) {
        if (panel->variant != 2 && frame == 20) {
            ScenePanel_SetState((void *)panel, ScenePanel_BeginLongSequence);
            Sound_PlaySong(294);
        }
        sub_082260A4((70 - panel->frame) >> 1);
    } else if (panel->variant == 0) {
        s32 i = 0;
        if (i < panel->subjectCount) {
            struct ScenePanelInteractionSubject **subject = panel->subjects;
            do {
                ScenePanelSubject_SetState((void *)*subject++, 1);
                i++;
            } while (i < panel->subjectCount);
        }
        PlayerCue_Stand(SCENE_PANEL_PLAYER, -1);
        Sound_PlaySong(250);
        ScenePanel_SetEffectState((void *)panel, 2);
    } else if (panel->variant == 1) {
        s32 anyActive;
        register u32 count asm("r0");
        PlayerCue_Stand(SCENE_PANEL_PLAYER, -1);
        sub_0815C99C(SCENE_PANEL_PLAYER);
        anyActive = 0;
        count = panel->subjectCount;
        if (anyActive < (s32)count) {
            struct ScenePanelInteractionSubject **subject = panel->subjects;
            s32 remaining = count;
            do {
                if ((*subject)->state == 4)
                    anyActive = 1;
                subject++;
                remaining--;
            } while (remaining != 0);
        }
        if (anyActive)
            ScenePanel_SetEffectState((void *)panel, 4);
        else
            ScenePanel_SetEffectState((void *)panel, 3);
    } else {
        PlayerCue_Stand(SCENE_PANEL_PLAYER, -1);
        sub_0815C99C(SCENE_PANEL_PLAYER);
        Sound_PlaySong(490);
        gUnk_02000488->state = 14;
        ScenePanelRig_SetState(&panel->rig, 1);
        ScenePanel_SetEffectState((void *)panel, 10);
    }
}
