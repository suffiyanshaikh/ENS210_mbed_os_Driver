#ifndef ENS210_H
#define ENS210_H

#include "mbed.h"
#include <cstdint>

// Chip constants
#define ENS210_Slave_ADDR       0x43<<1
#define ENS210_PARTID          0x0210 // The expected part id of the ENS210
#define ENS210_BOOTING_MS           2 // Booting time in ms (also after reset, or going to high power)

// Addresses of the ENS210 registers
#define ENS210_REG_PART_ID       0x00
#define ENS210_REG_UID           0x04
#define ENS210_REG_SYS_CTRL      0x10
#define ENS210_REG_SYS_STAT      0x11
#define ENS210_REG_SENS_RUN      0x21
#define ENS210_REG_SENS_START    0x22
#define ENS210_REG_SENS_STOP     0x23
#define ENS210_REG_SENS_STAT     0x24
#define ENS210_REG_T_VAL         0x30
#define ENS210_REG_H_VAL         0x33

//CRC Constants
//               7654 3210
// Polynomial 0b 1000 1001 ~ x^7+x^3+x^0
//            0x    8    9
#define CRC7WIDTH  7    // A 7 bits CRC has polynomial of 7th order, which has 8 terms
#define CRC7POLY   0x89 // The 8 coefficients of the polynomial
#define CRC7IVEC   0x7F // Initial vector has all 7 bits high
// Payload data
#define DATA7WIDTH 17
#define DATA7MASK  ((1UL<<DATA7WIDTH)-1) // 0b 0 1111 1111 1111 1111
#define DATA7MSB   (1UL<<(DATA7WIDTH-1)) // 0b 1 0000 0000 0000 0000

extern Serial pc;
extern I2C i2c;


class ENS210{

    public:

    /* Boot in to active mode,read partid & UID.
    *
    */
    void begin(void);
    
    /* Resets the sensor,
    *
    */
    bool reset();

    /* Control Sensor power modes
    *
    */
    void config_power(bool enable);

    /** Take Readings in Single Shot Mode
    *
    * @param t_data varialbe to receive  temp conversion values
    * @param h_data variable to receive humidity conversion values
    */
    void single_measure(uint32_t *t_data, uint32_t *h_data);

    /** values conversion
    *
    */
    void conversion(char buff[6],uint32_t *t_data, uint32_t *h_data);

    /** CRC Calculation
    */
    uint32_t crc7(uint32_t val);

    //Common Variables
    uint8_t err_t;
    uint8_t i2c_cmd[3];
    char i2c_data[6];
    uint16_t partid;
    uint64_t uid;
    uint32_t hi_uid;
    uint32_t lo_uid;

};















#endif /* ENS210_H */
