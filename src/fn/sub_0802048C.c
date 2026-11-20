#include "heap_memory.h"
#include "scene_object_controller.h"
void ResourceRegistry_RegisterResource(u32, void *, u32);

// confidence=low: loads the animation resources of the scene object manager and allocates its slot pool.
s32 SceneObjects_Init(struct SceneObjPool *p, s32 messageId, s32 unused)
{
    s32 flag;
    s32 i;
    struct SceneAnimation *q;
    p->messageId = messageId;
    p->count = p->selection = 0;
    flag = Script_GetKeywordValue(0x65, 1);
    p->separatePalettes = flag;
    if (flag) {
        struct SceneAnimation **t0, **t1, **t2, **t3, **t4, **t5, **t6;
        *(t0 = &p->paletteAnimationData[0]) = Heap_Alloc(sizeof(p->animationData[0]));
        *(t1 = &p->paletteAnimationData[1]) = Heap_Alloc(sizeof(p->animationData[0]));
        *(t2 = &p->paletteAnimationData[2]) = Heap_Alloc(sizeof(p->animationData[0]));
        *(t3 = &p->paletteAnimationData[3]) = Heap_Alloc(sizeof(p->animationData[0]));
        *(t4 = &p->paletteAnimationData[4]) = Heap_Alloc(sizeof(p->animationData[0]));
        *(t5 = &p->paletteAnimationData[5]) = Heap_Alloc(sizeof(p->animationData[0]));
        *(t6 = &p->paletteAnimationData[6]) = Heap_Alloc(sizeof(p->animationData[0]));
        if (!*t0 || !*t1 || !*t2 || !*t3 || !*t4 || !*t5 || !*t6) {
            Task_Kill(p);
            return -1;
        }
        Memory_Clear(*t0, sizeof(p->animationData[0]));
        Memory_Clear(*t1, sizeof(p->animationData[0]));
        Memory_Clear(*t3, sizeof(p->animationData[0]));
        Memory_Clear(*t4, sizeof(p->animationData[0]));
        Memory_Clear(*t5, sizeof(p->animationData[0]));
        Memory_Clear(*t6, sizeof(p->animationData[0]));
    } else {
        p->paletteAnimationData[0] = NULL;
        p->paletteAnimationData[1] = NULL;
        p->paletteAnimationData[2] = NULL;
        p->paletteAnimationData[3] = NULL;
        p->paletteAnimationData[4] = NULL;
        p->paletteAnimationData[5] = NULL;
        p->paletteAnimationData[6] = NULL;
    }
    if (Script_SeekToKeyword(0x64)) {
        i = 0;
        q = p->animationData[0];
        for (; i <= 4; i++) {
            u32 id;
            if (Script_GetPc())
                id = (u16)Script_GetValue();
            else
                id = 0x7ACD;
            if (!AnimData_Load(q, id))
                return -1;
            q++;
        }
    } else {
        i = 0;
        q = p->animationData[0];
        for (; i <= 4; i++) {
            if (!AnimData_Load(q, 0x7ACD))
                return -1;
            q++;
        }
    }
    if (Script_SeekToKeyword(0x6b)) {
        struct SceneAnimation *b;
        i = 0;
        b = p->animationData[2];
        for (; i <= 4; i++) {
            u32 id;
            if (Script_GetPc())
                id = (u16)Script_GetValue();
            else
                id = 0x7AD3;
            if (!AnimData_Load(b, id))
                return -1;
            if (p->separatePalettes) {
                AnimData_Load(&p->paletteAnimationData[0][i], id);
                AnimData_Load(&p->paletteAnimationData[1][i], id);
                AnimData_Load(&p->paletteAnimationData[2][i], id);
                AnimData_Load(&p->paletteAnimationData[3][i], id);
                AnimData_Load(&p->paletteAnimationData[4][i], id);
                AnimData_Load(&p->paletteAnimationData[5][i], id);
                AnimData_Load(&p->paletteAnimationData[6][i], id);
            }
            b++;
        }
    } else {
        i = 0;
        q = p->animationData[2];
        for (; i <= 4; i++) {
            if (!AnimData_Load(q, 0x7AD3))
                return -1;
            q++;
        }
    }
    if (Script_SeekToKeyword(0x53)) {
        u32 id;
        if (Script_GetPc())
            id = (u16)Script_GetValue();
        else
            id = 0x7AD2;
        i = 0;
        q = p->animationData[1];
        for (; i <= 4; i++) {
            if (!AnimData_Load(q, id))
                return -1;
            q++;
        }
    } else {
        i = 0;
        q = p->animationData[1];
        for (; i <= 4; i++) {
            if (!AnimData_Load(q, 0x7AD2))
                return -1;
            q++;
        }
    }
    if (Script_SeekToKeyword(0x62)) {
        struct SceneAnimation *b;
        struct SceneAnimation *c;
        i = 0;
        b = p->animationData[4];
        c = p->animationData[3];
        for (; i <= 4; i++) {
            u32 id;
            if (Script_GetPc())
                id = (u16)Script_GetValue();
            else
                id = 0x7AEE;
            if (!AnimData_Load(c, id))
                return -1;
            if (!AnimData_Load(b, id))
                return -1;
            b++;
            c++;
        }
    } else {
        struct SceneAnimation *b;
        struct SceneAnimation *c;
        i = 0;
        b = p->animationData[4];
        c = p->animationData[3];
        for (; i <= 4; i++) {
            if (!AnimData_Load(c, 0x7AD3))
                return -1;
            if (!AnimData_Load(b, 0x7AD3))
                return -1;
            b++;
            c++;
        }
    }
    if (Script_SeekToKeyword(0x73)) {
        i = 0;
        q = p->animationData[5];
        for (; i <= 4; i++) {
            u32 id;
            if (Script_GetPc())
                id = (u16)Script_GetValue();
            else
                id = 0x7AF3;
            if (!AnimData_Load(q, id))
                return -1;
            q++;
        }
    } else {
        i = 0;
        q = p->animationData[5];
        for (; i <= 4; i++) {
            if (!AnimData_Load(q, 0x7AF3))
                return -1;
            q++;
        }
    }
    if (Script_SeekToKeyword(0x75)) {
        i = 0;
        q = p->animationData[6];
        for (; i <= 4; i++) {
            u32 id;
            if (Script_GetPc())
                id = (u16)Script_GetValue();
            else
                id = 0x7AF3;
            if (!AnimData_Load(q, id))
                return -1;
            q++;
        }
    } else {
        i = 0;
        q = p->animationData[6];
        for (; i <= 4; i++) {
            if (!AnimData_Load(q, 0x7AF3))
                return -1;
            q++;
        }
    }
    {
        s32 count = Script_GetKeywordValue(0x6d, 0x10);
        void *buf;
        p->capacity = count;
        buf = Heap_Alloc(count * sizeof(*p->slots));
        p->slots = buf;
        Memory_Clear(buf, p->capacity * sizeof(*p->slots));
    }
    Memory_Fill16Reverse(p->primary, sizeof(p->primary) / sizeof(p->primary[0]), 0);
    ResourceRegistry_RegisterResource(0x85E6, p, 0);
    return 0;
}
