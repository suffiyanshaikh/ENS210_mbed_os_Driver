/*
* Example to code to read Temperature and humidity from ENS210 in single shot mode
*   
* Hardware Used:STM32l476RGV
*  MBED OS Version:5.14.2
*/

#include "mbed.h"
#include "ens210.h"
#include <cstdint>


Serial  pc(PA_9,PA_10,9600); //TX,Rx
I2C i2c(PB_7 ,PB_6);         //SDA,SCL


// main() runs in its own thread in the OS
int main()
{

    i2c.frequency(100000);
    ENS210 ens210; //Create ens210 object
     
    //Resets the sensor,boot in to active mode,read partid & UID
    ens210.begin();

    uint32_t t_data,h_data;

    while (true) {

        ens210.single_measure(&t_data, &h_data);
        wait_ms(500);

        float TinK=(float)t_data/64; //Temp in kelvin
        float TinC = TinK - 273.15; // Temperature in Celsius
        float H = (float)h_data/512; // relative humidity (in %)

        pc.printf("Temperature :%0.2f C, Humidity :%0.2f %% \n",TinC,H);
        pc.printf("\n");
        wait_ms(2000);

    }
}

