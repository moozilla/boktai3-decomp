#include "global.h"
void ResourceStation_ReadDisplayCommands(u8 *);
void ResourceStation_CheckInteraction(u8 *);
void ResourceStation_TransferStoredResource(u8 *);
void ResourceStation_UpdateAnimation(u8 *);
void ResourceStation_UpdateDigits(u8 *);
u32 ResourceStation_Tick(u8 *p)
{
    ResourceStation_ReadDisplayCommands(p);
    switch (p[0x262]) {
        case 0:
            ResourceStation_CheckInteraction(p);
            break;
        case 1:
            ResourceStation_TransferStoredResource(p);
            break;
    }
    ResourceStation_UpdateAnimation(p);
    ResourceStation_UpdateDigits(p);
    return 0;
}
