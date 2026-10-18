#include "global.h"
struct PresentationGraphicData {
    u8 kind;
    u8 unknown1;
    u16 amount;
};
u32 Presentation_SelectGraphicResource(struct PresentationGraphicData *data)
{
    if (data->amount == 0 || data->kind == 0x30 || data->kind == 0x2f)
        return 394;
    if (data->amount <= 250)
        return 398;
    if (data->amount <= 500)
        return 397;
    if (data->amount <= 750)
        return 396;
    else
        return 395;
}
