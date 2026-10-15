// MATCHING_CONCESSION: register-pinning
#include "scene_panel_rig.h"
struct SceneRigDriverView {
    u8 unknown0[0xC0F];
    u8 suppressDrain;
    u8 unknownC10[0x5C];
    struct ScenePanelRig rig;
    u8 unknown1320[0x2C];
    volatile u16 frame, interval;
    u8 unknown1350[0x10];
    void *action;
};
extern void *gUnk_08605A94[];
void sub_0805A4FC(struct ScenePanelRig *);
void ScenePanelRig_TickResourceDrain(struct ScenePanelRig *);
void sub_0805AAC8(struct ScenePanelRig *, u32);
#define ScenePanel_TickRig sub_0805AC3C
void ScenePanel_TickRig(struct ScenePanel *input)
{
    struct SceneRigDriverView *parent = (void *)input;
    struct ScenePanelRig *rig = &parent->rig;
    u32 palette;
    sub_0805A4FC(rig);
    sub_0824923C(rig, gUnk_08605A94[parent->rig.state]);
    if (parent->rig.scale) {
        register u32 flagOffset asm("r1") =
            (u32) & ((struct SceneRigDriverView *)0)->rig.unknown620[6];
        u16 *flags;
        asm("" : "+r"(flagOffset));
        flags = (void *)((u8 *)parent + flagOffset);
        {
            register u32 value asm("r1") = *flags;
            {
                register u32 mask asm("r0") = 4;
                mask |= value;
                *flags = mask;
            }
        }
        parent->rig.scale--;
    } else {
        register u32 offset asm("r3") = (u32) & ((struct SceneRigDriverView *)0)->rig.unknown620[6];
        register u16 *flags asm("r2");
        asm("" : "+r"(offset));
        flags = (void *)((u8 *)parent + offset);
        *flags &= 0xFFFB;
        {
            register u32 colliderOffset asm("r1") =
                (u32) & ((struct SceneRigDriverView *)0)->rig.unknown620;
            void *collider;
            register u32 positionOffset asm("r2");
            void *position;
            collider = (u8 *)parent + colliderOffset;
            positionOffset = (u32) & ((struct SceneRigDriverView *)0)->rig.position;
            position = (u8 *)parent + positionOffset;
            Collider_SetPosition(collider, position, 0);
        }
    }
    {
        register u32 offset asm("r3") = (u32) & ((struct SceneRigDriverView *)0)->suppressDrain;
        asm("" : "+r"(offset));
        {
            register u8 *flag asm("r0") = (u8 *)parent + offset;
            if (*flag == 0)
                ScenePanelRig_TickResourceDrain(rig);
        }
    }
    palette = 91;
    {
        register u32 actionOffset asm("r1") = (u32) & ((struct SceneRigDriverView *)0)->action;
        {
            void **action = (void *)((u8 *)parent + actionOffset);
            if (*action) {
                register u32 frameOffset asm("r3") =
                    (u32) & ((struct SceneRigDriverView *)0)->frame;
                volatile u16 *framePtr;
                register volatile u16 *intervalPtr asm("r1");
                register u32 intervalOffset asm("r0");
                u32 first;
                register u32 interval asm("r3");
                asm("" : "+r"(frameOffset));
                framePtr = (void *)((u8 *)parent + frameOffset);
                intervalOffset = (u32) & ((struct SceneRigDriverView *)0)->interval;
                intervalPtr = (void *)((u8 *)parent + intervalOffset);
                first = *framePtr;
                interval = *intervalPtr;
                asm("" : "+r"(first), "+r"(interval));
                if (first >= interval)
                    goto highlight;
                {
                    register u32 frame asm("r3") = *framePtr;
                    u32 phase;
                    if ((s32)frame < (s32)*intervalPtr - 90)
                        goto done;
                    phase = frame >> 2;
                    asm("" : "+r"(phase) : "r"(frame));
                    phase &= 1;
                    if (!phase)
                        goto done;
                }
            highlight:
                palette = 92;
            }
        }
    }
done:
    sub_0805AAC8(rig, palette);
}
