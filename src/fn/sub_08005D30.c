#include "global.h"
#include "scene_object.h"
/* Tentative: trigger the type-six object's palette-switch animation. */
s32 sub_0821ABA8(s32, s32);
s32 sub_08215284(struct SceneAnimation *, u16);
s32 sub_0822B2F8(s32);
s32 SceneObjects_SwitchPaletteFromScript(void)
{
    struct SceneObjectManager *manager = gUnk_02000024;
    struct SceneObject *p;
    struct SceneDraw *draw;
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
            sub_08215284(&p->animationData, 4);
        } else {
            p->initialAnimation = 5;
            sub_08215284(&p->animationData, 5);
        }
    } else {
        if (draw->mode == 0 || draw->mode == 2) {
            p->initialAnimation = 6;
            sub_08215284(&p->animationData, 6);
            p->animationData.palette = manager->paletteA;
        } else {
            p->initialAnimation = 7;
            sub_08215284(&p->animationData, 7);
            p->animationData.palette = manager->paletteB;
        }
        manager->stopFade = 1;
        if (sound)
            sub_0822B2F8(0x149);
    }
}
