#include "mbed.h"
 
I2C i2c(p9, p10);        // sda, scl
Serial pc(USBTX, USBRX); // tx, rx
 
const int addr = 0x00 ... 0x07; // define the correct I2C Address    
 
int main() 
{
    char regaddr[1];
    char readdata[8]; // room for length and 7 databytes
    char writedata[9]; // room for reg address, length and 7 databytes
    while (1) 
    {
        // read the data
        regaddr[0] = 0xE0;
        i2c.write(addr, regaddr, 1, true);  // select the register, no I2C Stop
        i2c.read(addr, readdata, 8);        // read the length byte and the 7 databytes
        
        wait (1);
 
        // print the data to the screen
        pc.printf("Register 0x%x = 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\r\n",
                  regaddr[0],
                  readdata[1], readdata[2], readdata[3], readdata[4],
                  readdata[5], readdata[6], readdata[7] );
        wait(1);
 
        // copy the data, starting with register address
        writedata[0] = regaddr[0];  // register address
        writedata[1] = readdata[0]; // length, should be 7
        writedata[2] = readdata[1]; // byte 1
        writedata[3] = readdata[2];
        writedata[4] = readdata[3];
        writedata[5] = readdata[4];
        writedata[6] = readdata[5];
        writedata[7] = readdata[6];
        writedata[8] = readdata[7]; // byte 7
 
        // write the data
        i2c.write(addr, writedata, 9); // select the register, 
                                       // write the length, write 7 databytes      
        wait(1);
 
    }
}
