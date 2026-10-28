#include "global.h"

/* tentative: loads a sprite resource and sets up five part drawings */

struct Drawing {
    u8 filler0[0x1A];
    u8 mode;
    u8 filler1B[0x20 - 0x1B];
    s16 x, y, z;
    u8 filler26[0x60 - 0x26];
};

struct Owner {
    struct Drawing drawings[5];
    u8 resource[0x200 - 0x1E0];
    void *data;
};

struct Actor {
    u8 filler0[0x10];
    s32 x;
    s32 y;
    s32 z;
    u8 filler1C[0x38 - 0x1C];
    s32 height;
    u8 filler3C[0x48 - 0x3C];
    u8 anim0;
    u8 anim1;
    u8 anim3;
    u8 filler4B;
    u8 anim2;
};

struct Camera {
    u8 filler0[0x44];
    s32 x;
};

extern struct Camera *gSceneCamera;

void *ScriptProc_Start(u32, u32);
void ResourceHeader_Relocate(void *, void *);
void ScenePalette_RefreshOnFlags(struct Actor *, struct Owner *);
s32 Widget_InitLinked(void *, void *, s32, s32, s32, s32, s32, s32);
void Anim_SetKeyframes(void *, void *, s32, s32, s32);
s32 Anim_Step(void *, void *);
void Actor_SetSpriteDepthOffsets(struct Owner *, s32);

void ActorSprite_InitPartDrawings(struct Actor *actor, struct Owner *owner, u32 id)
{
    struct {
        s16 x, y, z;
    } posv, *pos;
    s32 dx, dy;
    s32 frames;
    s32 i;
    s32 anim;
    u8 animId;
    s32 z;
    struct Drawing *d;
    void *res;

    owner->data = ScriptProc_Start(0xCB05, (u16)id);
    ResourceHeader_Relocate(owner->resource, owner->data);
    frames = 0;
    for (i = 0; i < 5; i++) {
        dx = 0;
        dy = 0;
        switch (i) {
            case 0:
                anim = actor->anim0;
                frames = 16;
                break;
            case 3:
                anim = actor->anim3;
                dx = 15;
                dy = -2;
                frames = 16;
                break;
            case 4:
                anim = actor->anim3;
                dx = -19;
                dy = -2;
                frames = 16;
                break;
            case 1:
                anim = actor->anim1;
                frames = 16;
                break;
            case 2:
                ScenePalette_RefreshOnFlags(actor, owner);
                anim = actor->anim2;
                frames = 16;
                break;
            default:
                anim = 0;
                break;
        }
        d = &owner->drawings[i];
        res = owner->resource;
        animId = anim;
        if (Widget_InitLinked(d, res, animId, frames, 0, 1, 0, 0) != 0)
            return;
        Anim_SetKeyframes(d, res, animId, 1, 0);
        Anim_Step(d, res);
        pos = &posv;
        pos->x = (actor->x >> 12) - gSceneCamera->x;
        pos->y = ((actor->z + (actor->y + actor->height)) >> 12) + 0x5A;
        pos->z = z = actor->z >> 12;
        pos->x += dx;
        pos->y += dy;
        d->x = pos->x;
        d->y = pos->y;
        d->z = pos->z;
        d->mode = 2;
    }
    Actor_SetSpriteDepthOffsets(owner, z);
}
