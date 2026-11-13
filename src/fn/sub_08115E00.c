#include "display_sequence_sprite.h"
#include "background_controller.h"
struct Camera {
    u8 unknown0[0x44];
    s32 screenX, screenY;
};
// Updates a dropped sprite only when its manager requests an animation tick.
struct Render {
    u32 flags;
    u8 pad4[12];
    u16 frame;
};
struct Drop {
    u8 pad0[0x1C];
    struct Render render;
    u8 pad30[0x38 - 0x30];
    s16 screenX, screenY, screenZ;
    u8 pad3E[0x48 - 0x3E];
    struct DisplaySequenceState sequence;
    u8 pad58[4];
    u8 entry[0x40];
    s16 x, y, z;
    u8 padA2[0xB1 - 0xA2];
    u8 tick;
};
void SpatialIndex_SetPosition(void *, s32, s32, s32);
void SpatialIndex_InsertEntry(void *);
u8 SpriteDrop_IsNearPlayer(void *);
void SpriteDrop_UpdateEntry(struct Drop *p)
{
    struct Render *render;
    if (p->tick) {
        p->screenY = p->y - gSceneCamera->screenY;
        p->screenZ = p->z;
        p->render.flags &= ~1;
        {
            void *entry = p->entry;
            s32 x = p->x;
            s32 y = p->y;
            s32 z;
            y -= 96;
            z = p->z;
            SpatialIndex_SetPosition(entry, x, y - z, z);
        }
        render = &p->render;
        DisplaySequence_StepSprite((struct DisplaySequenceSprite *)render, &p->sequence);

        p->tick = 0;
    }
    p->screenX = p->x - gSceneCamera->screenX;
    if (SpriteDrop_IsNearPlayer(p))
        SpatialIndex_InsertEntry(p->entry);
}
