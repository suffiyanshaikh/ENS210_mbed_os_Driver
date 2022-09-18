#include "ens210.h"
#include "mbed.h"
#include <cstdint>


void ENS210::begin(){

  
  // Reset the sensor
  err_t= reset();  
  if( !err_t ) err_t= reset(); // Retry
  if( !err_t ) { 
      pc.printf("Ens210 Reset Failed ,I2C ACK:%d\n",err_t);  
    }

   //Must disable low power to read PART_ID or UID
  i2c_cmd[0]=ENS210_REG_SYS_CTRL;
  i2c_cmd[1]=0x00;

  config_power(false); //Turn to active mode
  
  //Read Part id
  i2c_cmd[0]=ENS210_REG_PART_ID; // Reg addrs
  err_t=i2c.write(ENS210_Slave_ADDR,(char *)i2c_cmd,1,true);//set power mode
  err_t=i2c.read(ENS210_Slave_ADDR,(char *)i2c_data,2);
  partid= i2c_data[1]*256U + i2c_data[0]*1U;

        if( partid !=ENS210_PARTID ) 
        pc.printf("ENS210 PARTID MISMATCH, PARTID:0x%x, I2C ACK:%d\n",partid,err_t);
        else 
         pc.printf("ENS210 PARTID MATCH, PARTID:0x%x, I2C ACK:%d\n",partid,err_t);


//     //Read UID
//   i2c_cmd[0]=ENS210_REG_UID; // Reg addrs
//   err_t=i2c.write(ENS210_Slave_ADDR,(char *)i2c_cmd,1,true);//set power mode
//   err_t=i2c.read(ENS210_Slave_ADDR,(char *)uid,8);

//     if(err_t==0){

//         hi_uid=uid>>32;
//         lo_uid=uid & 0xFFFFFFFF;
//         pc.printf("ENS210 UID=%08x %08x, I2C ACK:%d\n",hi_uid,lo_uid,err_t);
//     }
//     else
//         pc.printf("Failed to read UID, I2C ACK:%d\n",err_t);

    config_power(true); //Turn to low power mode


}
 
 void ENS210::config_power(bool enable) {

  i2c_cmd[0]=ENS210_REG_SYS_CTRL;
  i2c_cmd[1]=enable ? 0x01: 0x00;
  
  err_t=i2c.write(ENS210_Slave_ADDR,(char *)i2c_cmd,2,false);//Change power mode
  wait(ENS210_BOOTING_MS);                // Wait boot-time after power switch

  if(enable == 0x00){
    if( err_t==0 )
        pc.printf("ENS210 LOW POWER OFF ,I2C ACK:%d\n",err_t);
    else 
        pc.printf("ENS210 Failed to OFF LOW POWER ,I2C ACK:%d\n",err_t);

  }
  else{  
    if(err_t==0)
        pc.printf("ENS210 LOW POWER ON,I2C ACK:%d\n",err_t);
    else 
        pc.printf("ENS210 Failed to ON LOW POWER,I2C ACK:%d\n",err_t);
  
  }

}

// Sends a reset to the ENS210. Returns false on I2C problems.
bool ENS210::reset(void) {
  
   i2c_cmd[0]=ENS210_REG_SYS_CTRL; //Sys ctrl register address
   i2c_cmd[1]=0x80;               //Sys ctrl cmd

    err_t=i2c.write(ENS210_Slave_ADDR,(char *)i2c_cmd,2,false);//Reset the device
    wait_ms(ENS210_BOOTING_MS);
    
    if( err_t==0 ) pc.printf("ENS210 Reset OK ,I2C ACK:%d \n",err_t);
    return err_t==0;
}

void ENS210::single_measure(uint32_t *t_data, uint32_t *h_data){

    //start sensor is single mode
    i2c_cmd[0]=ENS210_REG_SENS_RUN;
    i2c_cmd[1]=0x00;
    i2c_cmd[2]=0x03;

    //Start Measurement in single mode
    err_t =i2c.write(ENS210_Slave_ADDR,(char *)&i2c_cmd,3); 
    //wait_ms(130); instead of waiting checking measurement state
    if(err_t==0){
        pc.printf("ENS210 Single Measurement Start ,I2C ACK:%d \n",err_t);

        i2c_cmd[0]=ENS210_REG_SENS_STAT;
        err_t =i2c.write(ENS210_Slave_ADDR,(char *)&i2c_cmd,1); //check measuremnt is complete or not

        if(err_t==0){
            uint8_t meas_stat;
            while(1){

                err_t=i2c.read(ENS210_Slave_ADDR,(char *)i2c_data,1);
                 meas_stat=i2c_data[0];
                    //pc.printf("meas_stat :%x \n",meas_stat);
                    if(meas_stat==0x00)
                        break;
                    else
                        pc.printf("Sensor is Measuring..., ENS210_REG_SENS_STAT:%x \n",meas_stat);

            }

            pc.printf("Sensor Measurement is Complete , ENS210_REG_SENS_STAT:%x \n",meas_stat);

            //Read Temp and Humidity values
            i2c_cmd[0]=ENS210_REG_T_VAL;
                
            err_t=i2c.write(ENS210_Slave_ADDR,(char *)i2c_cmd,1,true);
            if(err_t==0)
            {
            pc.printf("Reading Values ,I2C ACK:%d \n",err_t);
            err_t=i2c.read(ENS210_Slave_ADDR,(char *)i2c_data,6);
            wait_ms(100);

                conversion(i2c_data,t_data,h_data);
            }
            else 
            pc.printf("ENS210  Failed to read Measurement values ,I2C ACK:%d \n",err_t);
    
        }
        else {
                pc.printf("ENS210  Failed to read Measurement state ,I2C ACK:%d \n",err_t);

        }   
    } 
    else
        pc.printf("ENS210  Failed to start Measurement  ,I2C ACK:%d \n",err_t);
  
}

void ENS210::conversion(char buff[6],uint32_t *t_data, uint32_t *h_data){


uint32_t t_val= (buff[2]<<16) + (buff[1]<<8) + (buff[0]<<0);
uint32_t h_val= (buff[5]<<16) + (buff[4]<<8) + (buff[3]<<0);

//Process the T_values
 *t_data = (t_val>>0 ) & 0xffff;
uint32_t t_valid= (t_val>>16) & 0x1;
uint32_t t_crc = (t_val>>17) & 0x7f;

//Process the H values

*h_data = (h_val>>0 ) & 0xffff;
uint32_t h_valid= (h_val>>16) & 0x1;
uint32_t h_crc = (h_val>>17) & 0x7f;

//check temp crc
uint32_t t_payl = (t_val>>0 ) & 0x1ffff;
bool t_crc_ok= crc7(t_payl)==t_crc;

//check humi crc
uint32_t h_payl = (h_val>>0 ) & 0x1ffff;
bool h_crc_ok= crc7(h_payl)==h_crc;

if(t_crc_ok != true && h_crc_ok != true){
    pc.printf("CRC Error");

}

}
uint32_t ENS210::crc7(uint32_t val)
{
    
// Setup polynomial
uint32_t pol= CRC7POLY;
// Align polynomial with data
pol = pol << (DATA7WIDTH-CRC7WIDTH-1);
// Loop variable (indicates which bit to test, start with highest)
uint32_t bit = DATA7MSB;
// Make room for CRC value
val = val << CRC7WIDTH;
bit = bit << CRC7WIDTH;
pol = pol << CRC7WIDTH;
// Insert initial vector
val |= CRC7IVEC;
// Apply division until all bits done
while( bit & (DATA7MASK<<CRC7WIDTH) ) {
if( bit & val ) val ^= pol;
bit >>= 1;
pol >>= 1;
}
return val;

}