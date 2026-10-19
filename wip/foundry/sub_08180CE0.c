#define ActorMarker_InitPartSprites sub_08180CE0
#define gActorMarkerPartFrames gUnk_0824E6D4
#define gActorMarkerPartAnimationMap gUnk_0824E6E6
#include "global.h"
struct ActorMarkerSprite {
    u32 flags;
    u8 pad4[12];
    u16 frame;
    u8 pad12[0x40-0x12];
};
struct ActorMarkerPart {
    void (*update)(void *), (*draw)(void *);
    u8 spriteData[0x34];
    u16 savedFacing;
    u8 state, pad3F;
};
struct ActorMarkerDrawView { u8 pad0[0x418]; struct ActorMarkerSprite sprites[9]; };
struct ActorMarker {
    u8 pad0[0xd6];
    u16 facingValue;
    u8 padD8[0x2e0-0xd8];
    u8 animations[9][28];
    u8 pad3DC[0x410-0x3dc];
    struct ActorMarkerPart parts[9];
    u8 pad650[0xb38-0x650];
    u8 kind, direction;
};
void Sprite_Init(void *, void *, u32);
void sub_08180A88(void *);
void sub_08180AFC(void *);
extern const u8 gUnk_0824E6D4[], gUnk_0824E6E6[];
void sub_08180CE0(struct ActorMarker *p)
{
    s32 flip = 0;
    s32 row = 0;
    s32 i;
    switch (((p->direction + 0x60) >> 6) & 3) {
    case 0: row = 1; break;
    case 1: row = 0; break;
    case 2: row = 0; flip = 1; break;
    case 3: row = 1; flip = 1; break;
    }
    for (i = 0; i < 9; i++) {
        u32 frame = gUnk_0824E6D4[row * 9 + i];
        const u8 *map;
        u32 animation;
        struct ActorMarkerSprite *sprite;
        switch (p->kind) {
        case 0: case 1: case 2: case 4: case 6: case 8: case 10: case 11: case 12: case 14:
        default: map = gUnk_0824E6E6; map += 9; break;
        case 3: case 5: case 7: case 9: case 13: case 15: map = gUnk_0824E6E6; break;
        }
        animation = map[i];
        Sprite_Init(&((struct ActorMarkerDrawView *)p)->sprites[i], p->animations[animation], 0);
        { struct ActorMarkerSprite *s = &((struct ActorMarkerDrawView *)p)->sprites[i]; s->frame = frame; }
        sprite = &((struct ActorMarkerDrawView *)p)->sprites[i];
        sprite->flags = 2;
        if (flip) sprite->flags = 6;
        if (i & 1) sprite->flags |= 0x200;
        else sprite->flags |= 0x400;
        {
            struct ActorMarkerPart *part = &p->parts[i];
            void (*update)(void *) = sub_08180A88;
            void (*draw)(void *) = sub_08180AFC;
            part->update = update;
            part->draw = draw;
        }
        p->parts[i].state = 0;
        p->parts[i].savedFacing = p->facingValue;
    }
}
