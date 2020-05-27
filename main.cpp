
#include "mbed.h"


char Phone_No[11]="";
char Direction;
uint16_t Distace_to_Start = 0;
uint16_t Length_of_Diagnostic = 0;
uint16_t Distance_to_Home = 0;

Timer t;

InterruptIn risingEdge(PB_5);
 
DigitalOut myled(LED1);
DigitalOut myled2(LED2);

DigitalOut drive1(PB_12);
DigitalOut drive2(PB_13);
DigitalOut reverse1(PB_14);
DigitalOut reverse2(PB_15);

DigitalIn Lock(PB_4);


PwmOut throttle(PC_11);
PwmOut brake(PC_10);



Serial pc(USBTX, USBRX);
//GPRS gprs(PB_6,PB_7,9600,Phone_No);
char Reply[100]={'\0'};

 
volatile long int c;
 
void pulses() {
    if(myled2 == 1) {
        myled2 = 0;
    } else {
        myled2 = 1;
    }
    c++;
}

void Init_GPRS() {
    gprs.init();
    if(gprs.checkSIMStatus()==0)
    { 
        pc.printf("SIM Status is GOOD\r\n"); 
    } else {
        pc.printf("SIM NOT Preset \r\n"); 
    }
    
     if(gprs.checkSignalStrength ()>=3) {
         pc.printf("Signal strength Good \r\n");        
     } else {
        pc.printf("Signal strength Weak\r\n");  
     }
        
     if(gprs.checkSIMStatus()==0) {
         pc.printf("SIM Status is GOOD\r\n"); 
     } else {
         pc.printf("SIM NOT Present \r\n"); 
     }  

     gprs.getSMS(msg);
}

void Input_params() {
 if(Lock.read() == 1) {
        pc.printf("Выключите замок зажигания");
    } else {
        pc.printf("Инспектор путей \n Перед началом работы введите параметры для диагностики n\ Направление диагностики (FRONT/REVERSE)");
        Direction += pc.getc();
        pc.printf("Расстояние до начала диагностики (м)");
        Distace_to_Start = pc.getc();
        pc.printf("Длина диагностирумого участка (м)");
        Length_of_Diagnostic = pc.getc();
        pc.printf("Расстояние до точки выгрузки данных (м)");
        Distance_to_Home = pc.getc();
    }
}

void Drive() {
    drive1 = 1;
    drive2 = 1;
    throttle.write(0.2);
    wait(2);
    throttle.write(0.4);
    wait(2);
    throttle.write(0.8);
    wait(2);
    throttle.write(1);
}

void Reverse() {
    reverse1 = 1;
    reverse2 = 1;
    throttle.write(0.2);
    wait(2);
    throttle.write(0.4);
    wait(2);
    throttle.write(0.8);
    wait(2);
    throttle.write(1);
}

void Stop() {
    brake.write(1);
}

void Test() {
    Drive();
    wait(1);
    Reverse();
    wait(1);
    Stop();
}



int main() {
    Init_GPRS();
    Input_params();
    
    if('as' == 'brake') {
       Stop();
    }
    
    risingEdge.rise(&pulses); 
    while(1) {
        t.reset();
        t.start();
        c = 0;
        while(t.read_ms() < 1001) {
            ;
        }     
        t.stop();
        uint32_t temp = c;
        pc.printf("Count: %d", temp);
        double circumference = 0.25 * 3.1416;
        double rev = (double)c;
        double rpm = rev*60;
        double speed = circumference * rev;  
        pc.printf(" %0.2f RPM", rpm);
        pc.printf(" speed: %f m/s", speed);
        pc.putc(0xA);
        pc.putc(0xD);
     
        throttle.period(0.02);
        brake.period(0.02);
     
        if (Lock.read() == 1) {
            if (Direction == 'Front') {
                while(temp <= Distace_to_Start + Length_of_Diagnostic) {
                   Drive();
                }
            } else if (Direction = 'Reverse') {
                while(temp <= Distace_to_Start + Length_of_Diagnostic) {
                   Reverse();
                }
            } else {
             Test();
            }
        }       
    }
}
