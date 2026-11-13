#include "projected_display.h"

static inline void ProjectedDisplay_Project(struct ContactPosition *out,
                                            const struct ContactPosition *position)
{
    s32 x = position->x, z = position->z;
    s32 sum, vertical, y, depth, scaledSum;
    out->x = projectScale(3 * (x - z));
    scaledSum = projectScale(3 * (x + z));
    sum = scaledSum;
    vertical = projectScale(3 * position->y);
    y = sum - vertical;
    depth = sum + vertical;
    out->x = (u16)out->x - gCameraOrigin[0] + 120;
    out->y = y - gCameraOrigin[1] + 80;
    out->z = depth - gCameraOrigin[2];
}

struct ProjectedDisplay *ProjectedDisplay_PositionEntry(struct ProjectedDisplay *display, s32 index)
{
    struct SceneInputWidget *entry = &display->entries[index];
    if (index == 1)
        ProjectedDisplay_Project(&entry->position, &display->world);
    else
        entry->position = display->screen;
    entry->position.z += gUnk_0860594C[index];
}

struct ProjectedDisplay *ProjectedDisplay_PositionEntry(struct ProjectedDisplay *, s32);

void ProjectedDisplay_UpdateEntries(struct ProjectedDisplay *display)
{
    struct SceneInputWidget *entry = display->entries;
    s32 index = 0;
    do {
        Anim_Step(entry, &display->resource);
        ProjectedDisplay_PositionEntry(display, index);
        index++;
        entry++;
    } while (index <= 4);
}

struct ProjectedDisplayFrame {
    u16 frame, unknown2;
};

extern const struct ProjectedDisplayFrame gUnk_08605960[];

s32 Widget_InitLinked(void *, void *, u32, u32, u32, u32, u32, void *);

s32 ProjectedDisplay_InitializeEntries(struct ProjectedDisplay *display)
{
    struct SceneInputWidget *entry = display->entries;
    s32 index = 0;
    struct SceneInputResource *resource = &display->resource;
    s32 result;
    do {
        if (Widget_InitLinked(entry, resource, 0, 16, 2, 0, 60, &display->screen) >= 0) {
            Anim_SetKeyframes(entry, resource, gUnk_08605960[index].frame, 1, 4);
            ProjectedDisplay_PositionEntry(display, index);
            index++;
            entry++;
        } else {
            return -1;
        }
    } while (index <= 4);
    result = 0;
    return result;
}

void ProjectedDisplay_RemoveEntries(struct ProjectedDisplay *display)
{
    s32 remaining;
    struct SceneInputWidget *entry = display->entries;
    for (remaining = 4; remaining >= 0; remaining--, entry++)
        Anim_Remove(entry);
}

void ProjectedDisplay_UpdateScreenPosition(struct ProjectedDisplay *display)
{
    display->raised = display->world;
    display->raised.y += display->height;
    ProjectedDisplay_Project(&display->screen, &display->raised);
}

s32 ProjectedDisplay_Update(struct ProjectedDisplay *display)
{
    ProjectedDisplay_UpdateScreenPosition(display);
    ProjectedDisplay_UpdateEntries(display);
    return 0;
}

s32 ProjectedDisplay_Destroy(struct ProjectedDisplay *display)
{
    ProjectedDisplay_RemoveEntries(display);
    return 0;
}

void ProjectedDisplay_UpdateScreenPosition(struct ProjectedDisplay *);

s32 ProjectedDisplay_InitializeEntries(struct ProjectedDisplay *);

s32 ProjectedDisplay_Initialize(struct ProjectedDisplay *display,
                                const struct ContactPosition *position)
{
    struct SceneInputResource *resource;
    s32 result;
    display->height = 0;
    display->world = *position;
    ProjectedDisplay_UpdateScreenPosition(display);
    resource = ScriptProc_Start(0xCB05, 0x7516);
    if (!resource)
        return -1;
    display->resource = *resource;
    ResourceHeader_Relocate(&display->resource, resource);
    display->world = *position;
    display->raised = *position;
    ProjectedDisplay_InitializeEntries(display);
    result = 0;
    return result;
}

struct ProjectedDisplay *ProjectedDisplay_Create(const struct ContactPosition *position)
{
    struct ProjectedDisplay *display = Task_Create(10, sizeof(*display));
    if (display) {
        Task_SetCallbacks(display, (void (*)(void *))ProjectedDisplay_Update,
                          (void (*)(void *))ProjectedDisplay_Destroy);
        if (ProjectedDisplay_Initialize(display, position) < 0) {
            Task_Kill(display);
            return 0;
        }
    }
    return display;
}

u32 ProjectedDisplay_GetHeight(struct ProjectedDisplay *display)
{
    if (!display)
        return 0;
    return display->height;
}

/* Observed callers ignore the result; non-null fallthrough retains r0. */

u32 ProjectedDisplay_SetHeight(struct ProjectedDisplay *display, s32 height)
{
    if (!display)
        return 0;
    display->height = height;
}
