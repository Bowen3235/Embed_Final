#include "Global.h"
#include "main.h"
 
DigitalOut led(PA_5);
DigitalOut orange(D7);
DigitalOut green(D9);
DigitalIn  button(PC_13);
Serial pc(USBTX, USBRX); // tx, rx
Serial uart(PA_9, PA_10);  // tx, rx 
Timer timer;
 
#define ARRAY_SIZE   24
I2C       i2c(PB_9,PB_8);   // SDA, SCL
I2CSlave  slave(PB_3,PB_10);   // SDA, SCL
int i;
int step;
int  nRet = 1;
int  waitEnable = 0;
const int  addr = 0xA0;
char nameDevice[16] = {0};
char MasterTX[] = "Message яяяя Master";
char SlaveTX[] = "Message from Slave";
char MasterRX[sizeof(SlaveTX)] = {0};
char SlaveRX[sizeof(MasterTX)] = {0};
int nCounter = 0;
 
#define MASTER      
int main() {
    led = 0;
    green = 0;
    orange = 0;
    uart.baud(9600);
    //pc.printf("SystemCoreClock = %d Hz\n", SystemCoreClock);
    
#ifdef MASTER       
    int  search = 0;
    strcpy(nameDevice,"Master");
  uart.printf("Uart start %s\r\n",nameDevice);
    
    i2c.frequency(100000); //Set the clock frequency
 
    // Send a start message to RealTerm
    pc.printf("\fmbed I2C debug tool ready\n\r");
    
    // Scan for I2C devices that reply with ack
    for (i=0; i<=254; i=i+2) {
        if (i2c.read(i, &MasterRX[0], 1) == 0) 
        {
            pc.printf("I2C device detected at address = 0x%02X\n\r", i);
            search++;
        }
    }
    if (search == 0) {
        pc.printf("I2C Slaves NO DETECTED!\n\r");
    } else {
        pc.printf("I2C Slaves %d\n\r",search);
    }
    MasterRX[0] = 0;
 
    while(1) {
            nRet = i2c.write(addr,MasterTX,sizeof(MasterTX));
            if (nRet == 0) {     // Slave ответил
                orange = 1;
                //green = 0;
            } else {             // Slave не ответил
                orange = 0;
                //green = 1;
            }
            
            memset(MasterRX,0,sizeof(MasterRX));
            strcpy(MasterRX,"ProbaOdnako");
            nRet = i2c.read(addr,MasterRX,4/*sizeof(MasterRX)*/);
            if (nRet == 0) {     // Slave ответил
                green = 1;
                uart.printf("Master RX %d: %s\r\n",nCounter,MasterRX);
            } else {             // Slave не ответил
                green = 0;
            }   
            
            led = 1;
            wait(1.5);
            led = 0;
            wait(1.5);
            nCounter++;
  }         
#else
    int begin = 0, end = 0;
    strcpy(nameDevice,"Slave");
  uart.printf("Uart start %s\r\n",nameDevice);
    
    timer.start();
    slave.frequency(100000); //Set the clock frequency
    slave.address(addr);
    pc.printf("Slave start\r\n");
    
    while(1) {  
        begin = timer.read_us();
        for (i=0;i<1000L;i++) {  // Задержка 1000
            led = 1;
            waitEnable++;
        }
        end = timer.read_us();
        
        memset(SlaveRX,0,sizeof(SlaveRX));
        step = slave.receive();
        switch(step) {
            case I2CSlave::ReadAddressed:   // Master запрашивает ответ от Slave
                strcpy(SlaveTX,"123\0");
                slave.write(SlaveTX,4);
                printf("Slave TX: %s\r\n",SlaveTX);
                uart.printf("Slave TX: %s\r\n",SlaveTX);
                orange = 1;
            break;
            
            case I2CSlave::WriteGeneral:    // Бродкаст
                slave.read(SlaveRX,sizeof(SlaveRX));
                printf("Broadcast RX: %s\r\n",SlaveRX);
                orange = 1;
            break;
            
            case I2CSlave::WriteAddressed: // Master спрашивает текущий Slave
                slave.read(SlaveRX,sizeof(SlaveRX));
                //wait(0.01);
                printf("Slave RX %d: %s\r\n",nCounter,SlaveRX);
                uart.printf("Uart Slave %d: %s\r\n",nCounter,SlaveRX);
                //uart.printf("No critical I2C delay %d us\r\n", end - begin);
                orange = 1;
            break;
            
            case I2CSlave::NoData:
                orange = 0;
            break;
        }       
        nCounter++;
  }         
#endif
 
}
