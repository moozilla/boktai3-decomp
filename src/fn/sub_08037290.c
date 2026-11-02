#include "display_sequence_inline.h"
#include "global.h"

// Tentative layouts for the wireless peer panel and its initial animation.
struct Sprite {
    u32 flags;
    u8 pad4[3], visible;
    u8 pad8[8];
    u16 tile;
    u8 pad12[10];
    u16 x, y;
    u8 pad20[12];
};

struct PeerPanel {
    u8 active, pad1[3];
    u32 flags;
    struct Sprite sprite;
    u8 glyph[0x1C];
    u8 drawings[5][0x60];
    u8 name[16];
    u8 pad240[4];
    u8 changed;
};

struct PeerStyle {
    int resource;
    u8 tile, animation, priority, x, y, pad9[3];
};

struct Owner {
    u8 pad[0x5F4];
    u8 descriptor[1];
};

extern const struct PeerStyle gUnk_0824DE64[];
void *ScriptProc_Start(int, int);
void AnimData_Load(void *, int);
void AnimData_SelectPaletteBank(void *, int);
void Sprite_Init(void *, void *, int);
void Anim_RestartSequence(void *, void *, int, int, int);
void Widget_InitLinked(void *, void *, int, int, int, int, int, void *);

static inline void WirelessPeerPanel_StepSprite(struct Sprite *sprite,
                                                struct DisplaySequenceState *a)
{
    const u16 *frame;
    frame = a->commands + a->index;
    sprite->tile = *frame >> 6;

    // Combine the animation and frame orientation flags for the sprite.
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = a->flags;
        bit = 1;
        first = (u8)(control.raw & bit);
        if (first != (((*frame & 0x30) >> 4) & bit))
            sprite->flags |= 4;
        else
            sprite->flags &= ~4;
    }

    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = a->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != (((*frame & 0x30) >> 4) & bit))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
    }

    if (++a->elapsed >= a->duration) {
        DisplaySequence_SelectNext(a);
        frame = a->commands + a->index;
        DisplaySequence_UpdateWeight(a, frame);
        if (!a->duration)
            a->duration = 1;
    }
}
int WirelessPeerPanel_Init(struct Owner *owner, struct PeerPanel *panel, int kind)
{
    struct PeerPanel *p = panel;
    int type = kind;
    void *resource;
    void *glyph;
    const struct PeerStyle *style;
    struct Sprite *sprite;
    struct Sprite *initialSprite;
    struct DisplaySequenceState animation;

    resource = ScriptProc_Start(0x922E, 0x4BBF);
    glyph = p->glyph;
    {
        const struct PeerStyle *table = gUnk_0824DE64;
        {
            int index = type;
            {
                u32 offset = index << 1;
                offset += type;
                offset <<= 2;
                style = (const struct PeerStyle *)(offset + (u32)table);
            }
        }
    }
    AnimData_Load(glyph, style->resource);
    {
        int tile = style->tile;
        int base = 503;
        AnimData_SelectPaletteBank(glyph, tile + base);
    }
    initialSprite = &p->sprite;
    Sprite_Init(initialSprite, glyph, style->priority | 48);
    Anim_RestartSequence(&animation, resource, style->animation, 0, 0);
    sprite = initialSprite;
    WirelessPeerPanel_StepSprite(sprite, &animation);
    {
        int one = 1;
        struct PeerPanel *write;
        const struct PeerStyle *table;
        int kind;
        u32 offset;
        const struct PeerStyle *entry;
        write = p;
        write->sprite.visible = one;
        table = gUnk_0824DE64;
        kind = type;
        offset = kind << 1;
        offset += type;
        offset <<= 2;
        entry = (const struct PeerStyle *)(offset + (u32)table);
        write->sprite.x = entry->x;
        write->sprite.y = entry->y;
    }
    {
        int zero = 0;
        u8 *drawing;
        int count;
        drawing = p->drawings[0];
        count = 4;
        do {
            Widget_InitLinked(drawing, owner->descriptor, 0, 17, 1, zero, 60, (void *)zero);
            drawing += 0x60;
        } while (--count >= 0);
    }
    p->active = 0;
    p->name[0] = 0;
    p->changed = 0;
    return 0;
}
