#include "gprs.h"
#include "mbed.h"


char Phone_No[11]="";
char Direction = "";
uint16_t Distace_to_Start = 0;
uint16_t Length_of_Diagnostic = 0;
uint16_t Distance_to_Home = 0;

InterruptIn risingEdge(PB_5);
 
DigitalOut myled(LED1);
DigitalOut myled2(LED2);

DigitalOut drive1(PB_12);
DigitalOut drive2(PB_13);
DigitalOut reverse1(PB_14);
DigitalOut reverse2(PB_15);

DigitalIn Lock(PB_17);


PwmOut throttle(PB_11);
PwmOut brake(PB_10);



Serial pc(USBTX, USBRX);
GPRS gprs(PB_6,PB_7,9600,Phone_No);
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
        Direction = pc.getc();
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
    throttle.write(20%);
    sleep(2000);
    throttle.write(40%);
    sleep(2000);
    throttle.write(80%);
    sleep(2000);
    throttle.write(100%);
}

void Drive() {
    reverse1 = 1;
    reverse2 = 1;
    throttle.write(20%);
    sleep(2000);
    throttle.write(40%);
    sleep(2000);
    throttle.write(80%);
    sleep(2000);
    throttle.write(100%);
}

void Test() {
    Drive();
    sleep(1000);
    Reverse();
    sleep(1000);
    Stop();
}

void Stop() {
    brake.write(100%);
}

int main() {
    Init_GPRS();
    Input_params();
    
    if(msg == 'brake') {
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
        uint32_t temp += c;
        pc.printf("Count: %d", temp);
        double circumference = 0.25 * 3.1416; // 25 cm wheel diameter * pi 
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
            if (Direction == "Front") {
                while(temp <= Distace_to_Start + Length_of_Diagnostic) {
                   Drive();
                }
            } else if (Direction = "Reverse") {
                while(temp <= Distace_to_Start + Length_of_Diagnostic) {
                   Reverse();
                }
            } else {
             Test();
            }
        }       
    }
}
