#include "global.h"
struct Part {
    u8 pad[6];
    u16 frames;
};
/* Keyframe display state handed to SceneBody_SetKeyframeDisplay (kind 2). */
struct Keyframes {
    u8 pad[0x3a];
    u16 frames;
};
struct Object {
    u8 pad0[0x1c];
    u16 frames, savedFrames;
    u8 pad20[0x3c];
    u8 motion[0xc];
    u8 pos[8];
    u8 pad70[0x64];
    u32 drawable;
    u8 padd8[8];
    struct Part part;
    u8 pade8[0x14];
    u8 transform[4];
    struct Keyframes keyframes;
};
int DisplayAnimation_InitializeKeyframes(void *, int, int, void *);
int DisplayAnimation_InitializeSequence(void *, int, int, int, void *);
void SceneBody_SetKeyframeDisplay(void *, void *);
void SceneBody_SetSprite(void *, void *);
int DisplayAnimation_SetPriority(void *, int);
int Actor_InitDrawable(struct Object *p, int kind, int a, int b, int c)
{
    void *d = &p->drawable;
    int status = -1;
    int result;
    if (kind == 2) {
        struct Keyframes *keyframes;
        status = DisplayAnimation_InitializeKeyframes(d, a, c, p->pos);
        keyframes = &p->keyframes;
        p->frames = keyframes->frames;
        SceneBody_SetKeyframeDisplay(p->motion, keyframes);
    } else if (kind == 1) {
        struct Part *part;
        status = DisplayAnimation_InitializeSequence(d, a, b, c, p->pos);
        part = &p->part;
        p->frames = part->frames;
        SceneBody_SetSprite(p->motion, p->transform);
    }
    p->savedFrames = p->frames;
    result = DisplayAnimation_SetPriority(d, 2);
    if (status < 0)
        return -1;
    return result;
}
