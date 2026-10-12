#include "global.h"
// MATCHING_CONCESSION: register-pinning
/* Hardware startup, subsystem initialization, then the task scheduler loop. */
struct Dma {vu32 source,dest,control;};
void RegisterRamReset(u32);
void sub_08248970(void);void sub_082284F4(void);void sub_082290F4(void);void m4aSoundInit(void);void sub_08213D10(void);
void sub_082485EC(u32);void sub_08224EEC(void);void sub_0821B084(void);void sub_08219AAC(void);void sub_08213DCC(void);void sub_0821BAFC(void);void sub_08215118(void);
void *sub_0821A520(u32,u32);void sub_08215148(void*);void sub_082151C0(void*);void sub_082229E8(void);void Time_CalculateSunriseSunset(s32,s32,s32);void sub_0821B048(void);void sub_08225328(void);void sub_0821A054(void);
extern u8 _08000350[],AgbMain__addr[],gUnk_03000000[];
void AgbMain(void){
 u32 saved0,saved1,zero;u32 *cookie0,*cookie1;
 *(vu16*)0x04000204=0x4014;
 {register u32 anchor asm("r4")=0x0401fb14;register u32 offset asm("r0")=anchor+0xfdfe0c00;cookie0=(u32*)(anchor-offset);saved0=*cookie0;anchor+=4;cookie1=(u32*)(anchor-offset);saved1=*cookie1;}
 RegisterRamReset(1);
 *cookie0=saved0;*cookie1=saved1;
 {struct Dma *dma;zero=0;dma=(struct Dma*)0x040000d4;dma->source=(u32)&zero;dma->dest=0x03000000;dma->control=0x85001f40;dma->control;while(dma->control&0x80000000);}
 sub_08248970();sub_082284F4();sub_082290F4();m4aSoundInit();sub_08213D10();
 CpuSet(_08000350,gUnk_03000000,(((s32)(AgbMain__addr-_08000350)/4)&0x1fffff)|0x04000000);
 sub_082485EC(64);sub_08224EEC();sub_0821B084();sub_08219AAC();sub_08213DCC();sub_0821BAFC();sub_08215118();
 sub_08215148(sub_0821A520(0xceaa,0x3002));sub_082151C0(sub_0821A520(0x98f5,0xff54));
 {vu16 *ime=(vu16*)0x04000208;vu16 *ie;u16 flags;*ime=0;ie=(vu16*)0x04000200;flags=*ie;*ie=0;*ie=4|flags;*ime=1;}
 sub_082229E8();Time_CalculateSunriseSunset(0x23a666,0x8bc000,0);sub_0821B048();sub_08225328();
 for(;;)sub_0821A054();
}
