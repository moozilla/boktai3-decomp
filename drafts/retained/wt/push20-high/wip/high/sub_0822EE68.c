#include "global.h"
u16 sub_08248764(u16,const u16*),sub_082488D8(u16,const u16*);
s32 sub_0822EE68(u16 start,const u16 *data,s32 length) {
 s32 blocks=length>>3,i,address;
 const u16 *p;
 u16 header[4];
 p=data;address=start;
 for(i=0;i<blocks;i++,p+=4,address++) if(sub_08248764(address,p)) goto fail;
 p=data;address=start;
 for(i=0;i<blocks;i++,p+=4,address++) if(sub_082488D8(address,p)) goto fail;
 header[0]=0;header[1]=0;
 if((length>>1)>0) {
 u32 x=header[0],sum=0;
 p=data;
 for(i=length>>1;i>0;i--,p++) {u32 v=*p;x^=v;sum+=v;}
 header[1]=sum;header[0]=x;
 }
 header[2]=length;header[3]=65535;
 if(sub_08248764((u16)address,header)) goto fail;
 if(sub_082488D8((u16)address,header)) goto fail;
 return length+8;
fail:return -1;
}
