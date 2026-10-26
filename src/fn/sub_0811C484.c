#include "global.h"
#include "ram.h"

/* tentative: counts a held charge, spawning particles, and fires the callback on release */

struct Pos {
    s16 x, y, z, w;
};

struct Size {
    s16 w, h, a, b;
};

struct Input {
    u16 held;
    u16 pad;
    u16 pressed;
};

struct Owner {
    u8 filler0[0x20];
    struct Pos pos;
    u8 filler28[0x251 - 0x28];
    s8 markerSlot;
};

struct Self {
    u8 filler0[0x58];
    s16 value;
    u8 filler5A[0x2B0 - 0x5A];
    u32 callback;
    u8 filler2B4[0x2B8 - 0x2B4];
    u16 threshold;
    u8 filler2BA[0x2BE - 0x2BA];
    u16 timer;
};

extern const u16 gUnk_086141BC[];

void Sound_PlaySong(u32);
s32 Mod(s32, s32);
void ActorMarker_ShowSlot(s32);
void ActorMarker_HideSlot(s32);
void sub_08249240(void *, u32, u32);
s32 SpriteParticle_SpawnSingleFrameBatch(struct Pos *, struct Pos *, struct Size *, u32, u32, u32,
                                         u32);

void ChargeMarker_TickHold(struct Self *self, struct Owner *owner, struct Input *input)
{
    u16 held;

    if ((held = input->held & 2) != 0) {
        if (self->value >= self->threshold) {
            u16 t = ++self->timer;
            if (t == 10)
                Sound_PlaySong(0x1F8);
            else if (t == 50)
                Sound_PlaySong(0x1F8);
            if (self->timer > 60) {
                ActorMarker_ShowSlot(owner->markerSlot);
            } else {
                s32 r = Mod(self->timer, 12);
                if (r == 0) {
                    struct Pos pos;
                    struct Size size;
                    u32 n;
                    pos = owner->pos;
                    pos.x += 32;
                    pos.y -= 16;
                    size.w = 0x16;
                    size.h = 0x14;
                    gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 0x3FF;
                    n = gUnk_086141BC[gEffectRandomTableIndex];
                    n &= 3;
                    n++;
                    SpriteParticle_SpawnSingleFrameBatch(&pos, &owner->pos, &size, n, 1, r, r);
                }
            }
        } else {
            self->timer = 0;
            ActorMarker_HideSlot(owner->markerSlot);
        }
    } else if ((input->pressed & 2) && self->timer) {
        sub_08249240(&self->callback, 0, self->callback);
        self->timer = held;
        ActorMarker_HideSlot(owner->markerSlot);
    }
}
