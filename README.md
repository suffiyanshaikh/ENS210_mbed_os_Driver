# ENS210_mbed_os_Driver

The following project involves the interfacing of an ENS210 sensor with the mbed OS platform. One relative humidity sensor and one high-accuracy temperature sensor are integrated into the ENS210. It has an I2C slave interface for interacting with a master processor. The driver has been tested using the STM32l476RGV Controller.To know more about ENS210 sensor please refer the datasheets below.

## Important links

* [https://os.mbed.com/mbed-os/](https://os.mbed.com/mbed-os/)
* [https://www.sciosense.com/wp-content/uploads/documents/ENS210.pdf](https://www.sciosense.com/wp-content/uploads/documents/ENS210.pdf)
* [https://os.mbed.com/docs/mbed-os/v6.15/apis/i2c.html](https://os.mbed.com/docs/mbed-os/v6.15/apis/i2c.html)

## Notes
* mbed os version 5.14.2.
* Change the I2C and UART pins to match the controller.
* The driver can be used on any controller that is supported by the mbed operating system.

## Results

<img width="909" alt="Screenshot 2022-09-18 at 8 36 48 PM" src="https://user-images.githubusercontent.com/42150715/190920399-1db1fc69-fc58-4463-9918-208d338a2a21.png">


<img width="907" alt="Screenshot 2022-09-18 at 8 36 54 PM" src="https://user-images.githubusercontent.com/42150715/190920440-1edd2934-35ba-4874-aec4-3e4d8181294d.png">
