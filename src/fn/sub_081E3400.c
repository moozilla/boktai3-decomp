#include "global.h"
#define Presentation_ConfigureLayout sub_081E2F44
#define Presentation_ConfigureFrameRange sub_081E3354
#define Presentation_Initialize sub_081E3400
// Provisional name: Presentation_Initialize
struct Sequence {u8 pad[0x21c];u16 option;u8 gap[0x230-0x21e];u32 parameter;};
s32 Script_SeekToKeyword(s32);s32 Script_GetValue(void);
void sub_081E2DA0(void*);void sub_081E2E7C(void*);void Presentation_ConfigureLayout(void*);void Presentation_ConfigureFrameRange(void*);
s32 sub_082279A8(s32,s32,s32,s32,s32,s32,s32);
// Parse sequence options and initialize its scripted presentation components.
s32 Presentation_Initialize(struct Sequence*p){s32 found;
 found=Script_SeekToKeyword('i');if(found)p->option=Script_GetValue();else p->option=found;
 found=Script_SeekToKeyword('p');if(found)p->parameter=Script_GetValue();else p->parameter=found;
 sub_081E2DA0(p);sub_081E2E7C(p);Presentation_ConfigureLayout(p);Presentation_ConfigureFrameRange(p);
 return sub_082279A8(0,4,4,4,4,0xffff,0);
}
