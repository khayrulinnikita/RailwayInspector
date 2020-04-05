#include "gprs.h"
#include "mbed.h"


char Phone_No[11]="";

InterruptIn risingEdge(PB_5);
 
DigitalOut myled(LED1);
DigitalOut myled2(LED2);

DigitalOut drive1(PB_12);
DigitalOut drive2(PB_13);
DigitalOut reverse1(PB_14);
DigitalOut reverse2(PB_15);


PwmOut throttle(PB_11);
PwmOut brake(PB_10);



Serial      pc(PA_2, PA_3);
GPRS   gprs(PB_6,PB_7,9600,Phone_No);
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


int main() {

    risingEdge.rise(&pulses); 
    while(1) {
        throttle.period(0.02);
        brake.period(0.02);
        t.reset();
        t.start();
        c = 0;
        while(t.read_ms() < 1001) {
            ;
        }     
        t.stop();
        long int temp = c;
        pc.printf("Count: %d", temp);
        double circumference = 0.25 * 3.1416; // 6 cm wheel diameter * pi 
        double rev = (double)temp;
        double rpm = rev*60;
        double speed = circumference * rev;  
        pc.printf(" %0.2f RPM", rpm);
        pc.printf(" speed: %f m/s", speed);
        pc.putc(0xA);
        pc.putc(0xD);

        
          
    gprs.init();
    if(gprs.checkSIMStatus()==0)
    { 
        pc.printf("SIM Status is GOOD\r\n"); 
        }
    else
    {
        pc.printf("SIM NOT Preset \r\n"); 
        }
    
     if(gprs.checkSignalStrength ()>=3)
     {
         pc.printf("Signal strength Good \r\n");  
           
        }  
        else
        {
        pc.printf("Signal strength Weak\r\n");  
        }
        
        if(gprs.checkSIMStatus()==0)
        {
             pc.printf("SIM Status is GOOD\r\n"); 
        }  
            
        else
        {
             pc.printf("SIM NOT Present \r\n"); 
        }  

        gprs.getSMS(msg);

        if(msg == 'drive'){
            drive1 = 1;
            drive2 = 2;
            throttle.write(20%);
            sleep(2000);
            throttle.write(40%);
            sleep(2000);
            throttle.write(80%);
            sleep(2000);
            throttle.write(100%);
        }

        if(msg == 'reverse'){
            reverse1 = 1;
            reverse2 = 2;
            throttle.write(20%);
            sleep(2000);
            throttle.write(40%);
            sleep(2000);
            throttle.write(80%);
            sleep(2000);
            throttle.write(100%);
        }

        if(msg == 'brake'){
            while (speed != 0)
            {
                brake.write(100%);
            }
            
        }
}