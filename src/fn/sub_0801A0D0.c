#include "global.h"
#define Presentation_PlaySelectionSound sub_0801A0D0
void sub_0822B2F8(int);
void Presentation_PlaySelectionSound(void *p,int selection) {
 switch(selection) {
 case 0:sub_0822B2F8(0x394);break;
 case 1:sub_0822B2F8(0x395);break;
 case 2:sub_0822B2F8(0x396);break;
 }
}
