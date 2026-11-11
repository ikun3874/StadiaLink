#include "../driver/configuration.h"
#include <stdio.h>
#include <stdlib.h>
static int checks=0;
static void Assert(bool value,const char* text) {checks++;if(!value){printf("FAIL: %s\n",text);exit(1);}}
static int Read16(const uint8_t* b){return b[0]|(b[1]<<8);}
int main() {
    STADIA_CONFIG c;StadiaConfigDefault(&c);Assert(sizeof(c)==64,"ABI size");Assert(StadiaConfigValid(&c),"default accepted");
    uint8_t raw[10]={3,8,0,0,128,128,128,128,0,0},out[16];
    StadiaTranslate(raw,&c,out);for(int i=0;i<4;i++)Assert(Read16(out+2*i)==32768,"idle axes centered");Assert(out[12]==0 && out[13]==0 && out[14]==0 && out[15]==0,"idle buttons released");
    raw[3]=64;StadiaTranslate(raw,&c,out);Assert(out[13]==1,"default A");c.Map[0]=2;StadiaTranslate(raw,&c,out);Assert(out[13]==2,"A to B");c.Map[0]=0;StadiaTranslate(raw,&c,out);Assert(out[13]==0,"A disabled");
    raw[3]=0;raw[2]=1;c.Map[11]=1;StadiaTranslate(raw,&c,out);Assert(out[13]==1,"capture mapped to A");
    raw[2]=2;c.Map[12]=8;StadiaTranslate(raw,&c,out);Assert(out[13]==128,"assistant mapped to Menu");raw[2]=0;StadiaConfigDefault(&c);
    raw[8]=128;raw[9]=64;StadiaTranslate(raw,&c,out);Assert(Read16(out+8)==128*1023/255 && Read16(out+10)==64*1023/255,"independent analog triggers");
    c.Map[17]=1;StadiaTranslate(raw,&c,out);Assert(Read16(out+8)==0 && (out[13]&1)!=0,"trigger to digital threshold");raw[8]=100;StadiaTranslate(raw,&c,out);Assert((out[13]&1)==0,"trigger below threshold");
    StadiaConfigDefault(&c);c.Map[0]=18;raw[3]=64;StadiaTranslate(raw,&c,out);Assert(Read16(out+8)==1023,"button to full analog trigger");raw[3]=0;
    c.Flags=32;raw[8]=255;raw[9]=0;StadiaTranslate(raw,&c,out);Assert(Read16(out+8)==0 && Read16(out+10)==1023,"swap triggers");
    StadiaConfigDefault(&c);raw[8]=raw[9]=0;raw[1]=1;StadiaTranslate(raw,&c,out);Assert(out[12]==2,"diagonal dpad");raw[1]=8;
    c.Map[0]=14;c.Map[1]=15;raw[3]=64|32;StadiaTranslate(raw,&c,out);Assert(out[12]==0,"opposite dpad cancellation");raw[3]=0;
    // Exhaustive stick bounds, monotonicity and inversion across the supported
    // settings range, including the measured 15-count onset and full deflection.
    for(int dz=0;dz<=40;dz++)for(int sens=50;sens<=200;sens++) {
        int previous=0;
        for(int r=0;r<=255;r++) {
            int x,y,ix,iy;StadiaStick((uint8_t)r,128,dz,sens,0,0,&x,&y);StadiaStick((uint8_t)r,128,dz,sens,1,0,&ix,&iy);
            Assert(x>=2 && x<=65534,"axis bounds");Assert(x>=previous,"axis monotonic");Assert(x+ix==65536,"axis inversion symmetry");previous=x;
        }
        int x,y;StadiaStick(128,128,dz,sens,0,0,&x,&y);Assert(x==32768&&y==32768,"stick exact center");
    }
    int x,y;
    StadiaStick(113,128,0,100,0,0,&x,&y);Assert(x==32768&&y==32768,"zero additional deadzone compensates negative 15-count onset");
    StadiaStick(143,128,0,100,0,0,&x,&y);Assert(x==32768&&y==32768,"zero additional deadzone compensates positive 15-count onset");
    StadiaStick(112,128,1,100,0,0,&x,&y);Assert(x==32768&&y==32768,"one-percent deadzone adds to the measured onset");
    StadiaStick(111,128,0,100,0,0,&x,&y);Assert(x<32768&&y==32768,"travel resumes smoothly after measured onset");
    StadiaStick(128,113,0,100,0,0,&x,&y);Assert(x==32768&&y==32768,"measured onset compensation applies to Y axis");
    StadiaStick(117,117,0,100,0,0,&x,&y);Assert(x<32768&&y<32768,"diagonal travel outside radial threshold is not trapped by square threshold");
    c.LeftSensitivity=0;c.Checksum=StadiaConfigChecksum(&c);Assert(!StadiaConfigValid(&c),"reject zero sensitivity");
    StadiaConfigDefault(&c);c.Map[0]=255;c.Checksum=StadiaConfigChecksum(&c);Assert(!StadiaConfigValid(&c),"reject invalid destination");
    StadiaConfigDefault(&c);c.Checksum^=1;Assert(!StadiaConfigValid(&c),"reject corrupt report");
    for(int gain=0;gain<=200;gain++) {
        int previous=0;
        for(int input=0;input<=255;input++) {
            int value=StadiaMotor((uint8_t)input,(uint8_t)gain);
            Assert(value==(input*gain/100>255?255:input*gain/100),"motor saturation");
            Assert(value>=previous,"motor monotonicity");previous=value;
            if(gain==100)Assert(value==input,"unity gain unchanged");
            if(input==0||gain==0)Assert(value==0,"motor silence preserved");
        }
    }
    StadiaConfigDefault(&c);c.StrongScale=200;c.WeakScale=150;c.Checksum=StadiaConfigChecksum(&c);Assert(StadiaConfigValid(&c),"amplified profile accepted");
    c.StrongScale=201;c.Checksum=StadiaConfigChecksum(&c);Assert(!StadiaConfigValid(&c),"excessive gain rejected");
    printf("PASS: %d checks against the driver's actual translation code.\n",checks);return 0;
}
