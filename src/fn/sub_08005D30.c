#include "global.h"
/* Tentative: trigger the type-six object's palette-switch animation. */
struct Animation {
    u8 pad[12];
    void *palette;
    u8 tail[12];
};
struct Draw {
    u8 pad[16];
    s16 mode;
};
struct Object {
    u8 type;
    u8 pad1[25];
    u16 initialAnimation;
    u8 pad1C[88];
    struct Animation animation;
    struct Draw draw;
};
struct Manager {
    u8 pad[31];
    u8 stopFade;
    u8 pad20[16];
    u8 paletteA[32], paletteB[32];
};
extern struct Manager *gUnk_02000024;
s32 sub_0821ABA8(s32, s32);
struct Object *sub_080057FC(struct Manager *, u32);
s32 sub_08215284(struct Animation *, u16);
s32 sub_0822B2F8(s32);
s32 SceneObjects_SwitchPaletteFromScript(void)
{
    struct Manager *manager = gUnk_02000024;
    struct Object *p;
    struct Draw *draw;
    s32 id, mode, sound;
    if (!manager || !(id = sub_0821ABA8('n', 0)) || !(p = sub_080057FC(manager, id)) ||
        p->type != 6)
        return -1;
    draw = &p->draw;
    mode = sub_0821ABA8('s', 0);
    sound = sub_0821ABA8('S', 1);
    if (!mode) {
        if (draw->mode == 0 || draw->mode == 2) {
            p->initialAnimation = 4;
            sub_08215284(&p->animation, 4);
        } else {
            p->initialAnimation = 5;
            sub_08215284(&p->animation, 5);
        }
    } else {
        if (draw->mode == 0 || draw->mode == 2) {
            p->initialAnimation = 6;
            sub_08215284(&p->animation, 6);
            p->animation.palette = manager->paletteA;
        } else {
            p->initialAnimation = 7;
            sub_08215284(&p->animation, 7);
            p->animation.palette = manager->paletteB;
        }
        manager->stopFade = 1;
        if (sound)
            sub_0822B2F8(0x149);
    }
}
