#include "mbed.h"
#include "stdlib.h"
#include "string.h"

Serial raspi(USBTX, USBRX);

//servo B3M
Serial B3M(p28, p27);
DigitalOut en(p15);

//mbed LED
DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalOut led4(LED4);



//Ticker

Ticker AllCrawl_ticker;
Ticker SpinCrawl_ticker;
Ticker LeftCrawl_ticker;
Ticker RightCrawl_ticker;
Ticker Butterfly_ticker;


char ch,flag=0,flag_c=0;

short int   //ID1=-14700,ID2=14700,ID3=26700,ID4=-26700,
ID1=0,ID2=0,ID3=0,ID4=0,Stopflag=0,i;

long data10_1 = 0;
long data10_2 = 0;
long data10_3 = 0;
long data10_4 = 0;
long Current1 = 0,Current2 = 0,Current3 = 0,Current4 = 0;

void raspi_comand()
{
    ch = raspi.getc();
    led1 = !led1;
}

void read(unsigned char id,unsigned char address,unsigned char length)
{
    unsigned char sum = (0x07+0x03+0x00+id+address+length)&0x00FF;
    char rx5;
    char rx6;
    char rx_kai[1] = "";
    char data16[5] = "";
    en=1;
    B3M.putc(0x07);       //SIZE
    B3M.putc(0x03);       //COMMAND
    B3M.putc(0x00);       //OPTION
    B3M.putc(id);         //ID
    B3M.putc(address);    //ADDRESS(Current 0x48,MotorTemperature 0x46)
    B3M.putc(0x02);     //LENGTH byte
    B3M.putc(sum);        //SUM
    wait(0.0000053);
    //wait(0.001);
    en=0;
    if(B3M.readable()==1) {
        char rx1 = B3M.getc();    //SIZE
        char rx2 = B3M.getc();    //COMMAND
        char rx3 = B3M.getc();    //STATUS
        char rx4 = B3M.getc();    //ID
        rx5 = B3M.getc();    //DATA1
        rx6 = B3M.getc();    //DATA2
        char rx7 = B3M.getc();    //SUM
    }
    switch (rx5) {
        case 0x0:
        case 0x1:
        case 0x2:
        case 0x3:
        case 0x4:
        case 0x5:
        case 0x6:
        case 0x7:
        case 0x8:
        case 0x9:
        case 0xa:
        case 0xb:
        case 0xc:
        case 0xd:
        case 0xe:
        case 0xf:
            sprintf(rx_kai,"0%x",rx5);
            sprintf(data16,"%x%s",rx6,rx_kai);
            break;
        default:
            sprintf(data16,"%x%x",rx6,rx5);
            break;
    }
    switch (id) {
        case 0x01:
            data10_1 = strtol(data16,NULL,16);
            break;
        case 0x02:
            data10_2 = strtol(data16,NULL,16);
            break;
        case 0x03:
            data10_3 = strtol(data16,NULL,16);
            break;
        case 0x04:
            data10_4 = strtol(data16,NULL,16);
            break;
    }
}

void write(unsigned char id,unsigned char mode)
{
    unsigned char sum = (0x08+0x04+0x00+id+mode+0x28+0x01)&0x00FF;
    en=1;
    B3M.putc(0x08);       //1 SIZE
    B3M.putc(0x04);       //2 COMMAND
    B3M.putc(0x00);       //3 OPTION
    B3M.putc(id);         //4 ID1
    B3M.putc(mode);       //5 DATA1
    B3M.putc(0x28);       //7 ADDRESS
    B3M.putc(0x01);       //8 COUNT
    B3M.putc(sum);        //9 SUM
    wait(0.0003);
    en=0;
}

void speed(unsigned char id,short speed)
{
    unsigned char speed_l = (unsigned char)(speed)&0x00FF;
    unsigned char speed_h = (unsigned char)(speed>>8)&0x00FF;
    unsigned char sum = (0x09+0x04+0x00+id+speed_l+speed_h+0x30+0x01)&0x00FF;
    en=1;
    B3M.putc(0x09);//SIZE
    B3M.putc(0x04);//COMMAND
    B3M.putc(0x00);//OPTION
    B3M.putc(id);//ID
    B3M.putc(speed_l);//SPEED_LOWBYTE
    B3M.putc(speed_h);//SPEED_HIGHBYTE
    B3M.putc(0x30);//ADRESS(SPEED 0x03)
    B3M.putc(0x01);//COUNT
    B3M.putc(sum);//SUM
    wait(0.0003);
    en=0;
}

//軌道生成コマンド
void kidou(unsigned char id)
{
    unsigned char sum = (0x08+0x04+id+0x01+0x29+0x01)&0x00FF;
    en=1;
    B3M.putc(0x08);   //1 SIZE
    B3M.putc(0x04);   //2 COMMAND
    B3M.putc(0x00);   //3 OPTION
    B3M.putc(id);     //4 ID
    B3M.putc(0x01);   //5 data
    B3M.putc(0x29);   //6 adr
    B3M.putc(0x01);   //7 cnt
    B3M.putc(sum);    //8 SUM
    wait(0.0003);
    en=0;
}

//トルク制御コマンド
void power(unsigned char id,short power)
{
    unsigned char power_l = (unsigned char)(power) & 0x00FF;
    unsigned char power_h = (unsigned char)(power >> 8) & 0x00FF;

    unsigned char sum = (0x09+0x04+id+power_l+power_h+0x3C+0x01)&0x00FF;
    en=1;
    B3M.putc(0x09);   //1 SIZE
    B3M.putc(0x04);   //2 COMMAND
    B3M.putc(0x00);   //3 OPTION
    B3M.putc(id);     //4 ID
    B3M.putc(power_l);   //5 data
    B3M.putc(power_h);   //6 adr
    B3M.putc(0x3C);   //7 adr
    B3M.putc(0x01);   //8 cnt
    B3M.putc(sum);    //9 SUM
    wait(0.0003);
    en=0;
}

//制御ゲイン
void gain(unsigned char id,unsigned char gain)
{

    unsigned char sum = (0x08+0x04+id+gain+0x5C+0x01)&0x00FF;
    en=1;
    B3M.putc(0x08);   //1 SIZE
    B3M.putc(0x04);   //2 COMMAND
    B3M.putc(0x00);   //3 OPTION
    B3M.putc(id);     //4 ID
    B3M.putc(gain);   //data
    B3M.putc(0x5C);   //7 adr
    B3M.putc(0x01);   //7 cnt
    B3M.putc(sum);    //8 SUM
    wait(0.0003);
    en=0;
}

//バタフライ時の速度ゲイン
void sgain(unsigned char id)
{
    unsigned char sum = (0x0B+0x04+id+0x20+0x4E);
    unsigned char sums = (sum+0x76+0x01)&0x00FF;
    en=1;
    B3M.putc(0x0B);//SIZE
    B3M.putc(0x04);//COMMAND
    B3M.putc(0x00);//OPTION
    B3M.putc(id);//ID
    B3M.putc(0x20);//DATA(1)
    B3M.putc(0x4E);//DATA(2)
    B3M.putc(0x00);//DATA(3)
    B3M.putc(0x00);//DATA(4)
    B3M.putc(0x76);//ADDRESS
    B3M.putc(0x01);//COUNT
    B3M.putc(sums);//SUM
    wait(0.0005);
    en=0;
}

void pos(unsigned char id,short deg,unsigned short time)
{
    unsigned char deg_l = (unsigned char)(deg)&0x00FF;
    unsigned char deg_h = (unsigned char)(deg>>8)&0x00FF;
    unsigned char time_l = (unsigned char)(time)&0x00FF;
    unsigned char time_h = (unsigned char)(time>>8)&0x00FF;
    unsigned char sum = (0x09+0x06+id+deg_l+deg_h+time_l+time_h)&0x00FF;
    en=1;
    B3M.putc(0x09);   //1 SIZE
    B3M.putc(0x06);   //2 COMMAND
    B3M.putc(0x00);   //3 OPTION
    B3M.putc(id);     //4 ID
    B3M.putc(deg_l);  //5 POS_L
    B3M.putc(deg_h);  //6 POS_H
    B3M.putc(time_l); //7 TIME_L
    B3M.putc(time_h); //8 TIME_H
    B3M.putc(sum);    //9 SUM
    wait(0.0003);
    en=0;
}

void reset()
{
    unsigned char sum = (0x06+0x05+0xff)&0x00FF;
    B3M.putc(0x06);
    B3M.putc(0x05);
    B3M.putc(0x00);
    B3M.putc(0x01);
    B3M.putc(0x01);
    B3M.putc(sum);
}



void current()
{
    read(0x01,0x50,0x02);
    wait(0.0001);
    read(0x02,0x50,0x02);
    wait(0.0001);
    read(0x03,0x50,0x02);
    wait(0.0001);
    read(0x04,0x50,0x02);
    wait(0.0001);
    read(0x01,0x50,0x02);
    Current4 = data10_1;
    read(0x02,0x50,0x02);
    Current1 = data10_2;
    read(0x03,0x50,0x02);
    Current2 = data10_3;
    read(0x04,0x50,0x02);
    Current3 = data10_4;
}

void read1()
{
    raspi.printf("%ld, %ld, %ld, %ld\r\n",Current1,Current2,Current3,Current4);
}

void crawl()//前進クロール
{
    if(Current1>=914 && Current1<=2880) {
        speed(0x01,-15000);//-15000
    } else {
        speed(0x01,-5000);//-5000
    }

    if(Current2>=170 && Current2<=2266) {
        speed(0x02,5000);
    } else {
        speed(0x02,15000);
    }

    if(Current3>=1820 && Current3<=3930) {
        speed(0x03,-5000);//-5000
    } else {
        speed(0x03,-15000);//-15000
    }

    if(Current4>=400 && Current4<=2325) {
        speed(0x04,15000);//15000
    } else {
        speed(0x04,5000);//5000
    }
}

void Scrawl()//超新地旋回
{
    if(Current1>=760 && Current1<=2750) {
        speed(0x01,15000);//-15000
    } else {
        speed(0x01,5000);//-5000
    }

    if(Current2>=159 && Current2<=2306) {
        speed(0x02,5000);
    } else {
        speed(0x02,15000);
    }

    if(Current3>=1660 && Current3<=3780) {
        speed(0x03,5000);//-5000
    } else {
        speed(0x03,15000);//-15000
    }

    if(Current4>=385 && Current4<=2328) {
        speed(0x04,15000);//15000
    } else {
        speed(0x04,5000);//5000
    }
}


void Lcrawl()//左旋回 2倍速
{
    if(Current1>=1130 && Current1<=2820) {//1115 2845
        speed(0x01,-32767);//--32767
    } else {
        speed(0x01,-1989);//-4234
    }

    if(Current2>=170 && Current2<=2310) {//15 2280
        speed(0x02,5000);//5000
    } else {
        speed(0x02,15000);//15000
    }

    if(Current3>=160 && Current3<=1790) {//1780 110
        speed(0x03,-32767);//-32767
    } else {
        speed(0x03,-1989);//-4234
    }

    if(Current4>=400 && Current4<=2370) {//420 2080
        speed(0x04,15000);//15000
    } else {
        speed(0x04,5000);//5000
    }
}

void Rcrawl()//右旋回
{
    if(Current1>=1000 && Current1<=2845) {
        speed(0x01,-11000);//-22500
    } else {
        speed(0x01,-1000);//-4500
    }

    if(Current2>=50 && Current2<=2300) {
        speed(0x02,500);
    } else {
        speed(0x02,18500);
    }

    if(Current3>=830 && Current3<=2950) {
        speed(0x03,-1000);//-4500
    } else {
        speed(0x03,-11000);//-22500
    }

    if(Current4>=360 && Current4<=2200) {
        speed(0x04,18500);//15000
    } else {
        speed(0x04,500);//5000
    }
}


void AllCrawl()
{
    current();
    read1();
    crawl();
}

void SpinCrawl()
{
    current();
    read1();
    Scrawl();
}

void LeftCrawl()
{
    current();
    read1();
    Lcrawl();
}

void RightCrawl()
{
    current();
    read1();
    Rcrawl();
}

void ButterflyGait()
{
    speed(0x01,-7000);
    speed(0x02,7000);
    speed(0x03,-7000);
    speed(0x04,7000);
}

int main()
{
    //Serial
    B3M.baud(1500000);
    raspi.baud(9600);

    B3M.format(8,Serial::None,1);
    raspi.format(8,Serial::None,1);

    //servo iti syokika
    speed(0xff,0);
    for(i=0x01; i<0x05; i++) {
        write(i,0x02);
    }
    raspi.printf("\nServo_Free_mode\r\n");
    wait(0.1);

    //aizu
    led1 = 1;
    wait(0.25);
    led2 = 1;
    wait(0.25);
    led3 = 1;
    wait(0.25);
    led4 = 1;
    wait(0.5);
    led1 = 0;
    led2 = 0;
    led3 = 0;
    led4 = 0;

    raspi.printf("\nPlese_command\r\n");

    while(1) {
        if(raspi.readable()==1) {
            ch=raspi.getc();
            Stopflag=0;
            switch(ch) {
                case 'z'://----------*----------------Reset_Parameters
                    raspi.printf("Parameters_Reset\r\n");
                    ID1=1;
                    ID2=0;
                    ID3=1;
                    ID4=0;
                    flag=0;
                    break;

                case 'f'://--------------------------Free
                    raspi.printf("Free_Mode\r\n");
                    write(0xff,0x02);
                    flag=0;
                    led1 = 0;
                    led2 = 1;
                    led3 = 0;
                    led4 = 1;
                    break;

                case 'o':
                    raspi.printf("All_Angle_Mode\r\n");
                    speed(0xff,0);
                    write(0xff,0x00);//0x00
                    flag='a';
                    led1 = 1;
                    led2 = 1;
                    led3 = 1;
                    led4 = 1;
                    break;

                    //----------------------------------------------------Front_Left_Arm(angle)

                case 'p':
                    if(flag=='a') {
                        raspi.printf("Default_Position\r\n");
                        pos(0x01,3000,1000);
                        pos(0x02,9000,1000);
                        pos(0x03,9000,1000);
                        pos(0x04,3000,1000);
                    } else {
                        raspi.printf("Erroer\r\n");
                    }
                    break;

                case 'n':
                    if(flag=='a') {
                        raspi.printf("Default_Position\r\n");
                        pos(0x01,3000,1000);
                        pos(0x02,9000,1000);
                        pos(0x03,9000,1000);
                        pos(0x04,600,1000);
                    } else {
                        raspi.printf("Erroer\r\n");
                    }
                    break;
                    
                case's'://----------------------------
                    if(flag=='a') {
                        raspi.printf("Speed_Mode\r\n");
                        write(0x01,0x06);
                        write(0x02,0x06);
                        write(0x03,0x06);
                        write(0x04,0x06);

                        gain(0x01,0x01);
                        gain(0x02,0x01);
                        gain(0x03,0x01);
                        gain(0x04,0x01);

                        sgain(0xff);

                        write(0x01,0x04);
                        write(0x02,0x04);
                        write(0x03,0x04);
                        write(0x04,0x04);
                        break;
                    }


                case 'c':

                    if(flag=='a') {

                        raspi.printf("\n\n");
                        AllCrawl_ticker.attach(&AllCrawl,0.05);
                        led3 = !led3;
                        ch = 'o';
                        flag_c='v';
                    }
                    break;

                case 'v':
                    if(flag_c=='v') {
                        AllCrawl_ticker.detach();
                        speed(0xff,0);
                        led3 = !led3;
                        ch = 'o';
                        flag='a';
                    }
                    break;

                case 'b':
                    if(flag=='a') {
                        raspi.printf("Butterfly_Position\r\n");
                        pos(0x01,0,1000);
                        pos(0x02,0,1000);
                        pos(0x03,0,1000);
                        pos(0x04,0,1000);
                    } else {
                        raspi.printf("Erroer\r\n");
                    }
                    break;

                case't':

                    raspi.printf("power\r\n");
                    write(0xff,0x06);
                    gain(0xff,0x01);
                    sgain(0xff);
                    write(0xff,0x04);
                    break;

                case 'g':

                    if(flag=='a') {

                        raspi.printf("\n\n");
                        Butterfly_ticker.attach(&ButterflyGait,0.05);
                        led3 = !led3;
                        ch = 'o';
                        flag_c='v';
                    }
                    break;

                case 'h':
                    if(flag_c=='v') {
                        Butterfly_ticker.detach();
                        speed(0xff,0);
                        led3 = !led3;
                        ch = 'o';
                        flag='a';
                    }
                    break;


                case 'j':
                    if(flag=='a') {
                        raspi.printf("spin_Position\r\n");
                        pos(0x01,3000,1000);
                        pos(0x02,-3000,1000);
                        pos(0x03,9000,1000);
                        pos(0x04,-9000,1000);
                    } else {
                        raspi.printf("Erroer\r\n");
                    }
                    break;

                case 'u'://SpinCrawl暴走中

                    if(flag=='a') {
                        raspi.printf("\n\n");
                        SpinCrawl_ticker.attach(&SpinCrawl,0.05);
                        led3 = !led3;
                        ch = 'o';
                        flag_c='v';
                    }
                    break;


                case 'i':

                    if(flag_c=='v') {
                        raspi.printf("\n\n");
                        SpinCrawl_ticker.detach();
                        speed(0xff,0);
                        led3 = !led3;
                        ch = 'o';
                        flag='a';
                    }
                    break;

                case 'k':

                    if(flag=='a') {

                        //raspi.printf("\n\n");
                        LeftCrawl_ticker.attach(&LeftCrawl,0.05);
                        led3 = !led3;
                        ch = 'o';
                        flag_c='v';
                    }
                    break;

                case 'l':
                    if(flag_c=='v') {
                        LeftCrawl_ticker.detach();
                        speed(0xff,0);
                        led3 = !led3;
                        ch = 'o';
                        flag='a';
                    }
                    break;

                case 'e':

                    if(flag=='a') {

                        raspi.printf("\n\n");
                        RightCrawl_ticker.attach(&RightCrawl,0.05);
                        led3 = !led3;
                        ch = 'o';
                        flag_c='v';
                    }
                    break;

                case 'r':
                    if(flag_c=='v') {
                        RightCrawl_ticker.detach();
                        speed(0xff,0);
                        led3 = !led3;
                        ch = 'o';
                        flag='a';
                    }
                    break;

                default:
                    raspi.printf("Please_command\r\n");
                    break;
            }
            wait(0.0025);
        }
    }//while(1)
}//main
//z;f;o;p;s;c;v;d

//o-p-s-c Crawl gait :v-Stop
//o-j-s-u Spin turn :i-Stop
//o-n-s-k Left Crawl :l-Stop
//o-p-s-e Right Crawl :r-Stop
//o-b-t-g Butterfly gait :h-Stop
//a
