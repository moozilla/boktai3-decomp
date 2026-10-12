// MATCHING_CONCESSION: register-pinning
#include "global.h"
#define SceneMessages_Dispatch sub_08020AF0
extern u8*gUnk_0200047C;void sub_08020D54(void*,void*);void sub_08020A60(void*,void*,void*);void sub_08020AC0(void*,void*,void*);void sub_08020ADC(void*,void*,void*);
typedef struct{u16 id;u8 unk[4];u8 count[2];void*messages[2][4];u8 pad[8];void*next;} Entry;
int SceneMessages_Dispatch(u8*p){Entry*entry=(Entry*)(p+0x78);int i;i=0;if(i<entry->count[gUnk_0200047C[0x24]]){do{u8*message;register u8*controller asm("r0")=gUnk_0200047C;asm("" : "+r"(controller));message=entry->messages[controller[0x24]][i];sub_08020D54(entry,message);switch(message[6]){case 0:sub_08020A60(p,entry,message);break;case 1:sub_08020AC0(p,entry,message);break;case 2:sub_08020ADC(p,entry,message);break;}i++;}while(i<entry->count[gUnk_0200047C[0x24]]);}return 0;}
