// INCLUDE NEEDED LIBRARY
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <wiringPiSPI.h>
#include <time.h>
//DEFINE I2C ADDRESS & SPI CHANNEL
#define BMP180_Address 0x77
#define SPI_CHANNEL 0
#define SPI_SPEED 8000000

//Operating Modes
#define BMP180_ULTRALOWPOWER    0
#define BMP180_STANDARD         1
#define BMP180_HIGHRES          2
#define BMP180_ULTRAHIGHRES     3

//BMP180 Registers
#define OSS 1                         //<--------------- CHANGE YOUR OPERATION MODE BY CHANGE OSS VALUE (0;1;2;3)
#define BMP180_CAL_AC1          0xAA  //Calibration data (16 bits)
#define BMP180_CAL_AC2          0xAC  //Calibration data (16 bits)
#define BMP180_CAL_AC3          0xAE  //Calibration data (16 bits)
#define BMP180_CAL_AC4          0xB0  //Calibration data (16 bits)
#define BMP180_CAL_AC5          0xB2  //Calibration data (16 bits)
#define BMP180_CAL_AC6          0xB4  //Calibration data (16 bits)
#define BMP180_CAL_B1           0xB6  //Calibration data (16 bits)
#define BMP180_CAL_B2           0xB8  //Calibration data (16 bits)
#define BMP180_CAL_MB           0xBA  //Calibration data (16 bits)
#define BMP180_CAL_MC           0xBC  //Calibration data (16 bits)
#define BMP180_CAL_MD           0xBE  //Calibration data (16 bits)
#define BMP180_CONTROL          0xF4
#define BMP180_TEMPDATA         0xF6
#define BMP180_PRESSUREDATA     0xF6

//COMMANDS
#define BMP180_READTEMPCMD      0x2E
#define BMP180_READPRESSURECMD  0x34

short AC1,AC2,AC3,B1,B2,MB,MC,MD;
unsigned short AC4,AC5,AC6;
int fd;

//I2C MACRO
char I2C_readByte(int reg)
{
    return (char)wiringPiI2CReadReg8(fd,reg);
}

unsigned short I2C_readU16(int reg)
{
    int MSB,LSB;
    MSB = I2C_readByte(reg);
    LSB = I2C_readByte(reg + 1);
    int value = (MSB << 8) +LSB;
    return (unsigned short)value;
}

short I2C_readS16(int reg)
{
     int MSB = I2C_readByte(reg);
    int LSB = I2C_readByte(reg + 1);
    return (short)((MSB << 8) | LSB);
}
void I2C_writeByte(int reg,int val)
{
    wiringPiI2CWriteReg8(fd,reg,val);
}

//BMP180_MACRO
//BMP180_INITIALIZING
void load_calibration()
{
    AC1 = I2C_readS16(BMP180_CAL_AC1);
    AC2 = I2C_readS16(BMP180_CAL_AC2);
    AC3 = I2C_readS16(BMP180_CAL_AC3);
    AC4 = I2C_readU16(BMP180_CAL_AC4);
    AC5 = I2C_readU16(BMP180_CAL_AC5);
    AC6 = I2C_readU16(BMP180_CAL_AC6);
    B1  = I2C_readS16(BMP180_CAL_B1);
    B2  = I2C_readS16(BMP180_CAL_B2);
    MB  = I2C_readS16(BMP180_CAL_MB);
    MC  = I2C_readS16(BMP180_CAL_MC);
    MD  = I2C_readS16(BMP180_CAL_MD);
}

//READ_RAW_TEMPERATURE
int read_raw_temp()
{
    int raw;
    I2C_writeByte(0XF4,0X2E);
    delay(5);  //5ms;
    raw = I2C_readByte(BMP180_TEMPDATA) << 8;
    raw += I2C_readByte(BMP180_TEMPDATA+1);
    return raw;
}

//RAW_TEMPEATURE_TO_TRUE_TEMPERATURE
float read_temperature()
{
    float T;
    int UT,X1,X2,B5;
    UT = read_raw_temp();
    X1 = ((UT - AC6)*AC5) >> 15;
    X2 = (MC << 11) / (X1 + MD);
    B5 = X1 + X2;
    T = ((B5 + 8) >> 4) /10.0;
    return T;
}

//READ_RAW_PRESSURE
int read_raw_pres()
{
    int MSB,LSB,XLSB,raw;
    I2C_writeByte(BMP180_CONTROL, BMP180_READPRESSURECMD + (OSS << 6));
    // DELAY_BY_OSS_SELECT
    switch (OSS) {
        case 0: delay(5); break;
        case 1: delay(8); break;
        case 2: delay(14); break;
        case 3: delay(26); break;
    }

    MSB  = I2C_readByte(BMP180_PRESSUREDATA);
    LSB  = I2C_readByte(BMP180_PRESSUREDATA + 1);
    XLSB = I2C_readByte(BMP180_PRESSUREDATA + 2);
    raw = ((MSB << 16) + (LSB << 8) + XLSB) >> (8 - OSS);
    return raw;
}

//RAW_PRESSURE_TO_TRUE_PRESSURE
int read_pressure()
{
    int P;
    int UT,UP,X1,X2,X3,B3,B5,B6;
    unsigned int B4;
    int B7;
    UT = read_raw_temp();
    UP = read_raw_pres();

    X1 = ((UT - AC6)*AC5) >> 15;
    X2 = (MC << 11) / (X1 + MD);
    B5 = X1 + X2;

    //PRESSURE_CALCULATION
    B6 = B5 - 4000;
    X1 = (B2 * (B6 * B6) >> 12) >> 11;
    X2 = (AC2 * B6) >> 11;
    X3 = X1 + X2;
    B3 = (((AC1 * 4 + X3) << OSS) + 2) / 4;
    X1 = (AC3 * B6) >> 13;
    X2 = (B1 * ((B6 * B6) >> 12)) >> 16;
    X3 = ((X1 + X2) + 2) >> 2;
    B4 = (AC4 * (X3 + 32768)) >> 15;
    B7 = (UP - B3) * (50000 >> OSS);
    if (B7 < 0x80000000){P = (B7 * 2) / B4;}
    else {P = (B7 / B4) * 2;}
    X1 = (P >> 8) * (P >> 8);
    X1 = (X1 * 3038) >> 16;
    X2 = (-7357 * P) >> 16;
    P = P + ((X1 + X2 + 3791) >> 4);
    return P;
}

//MAX7219_MACRO
void send_data(uint8_t address, uint8_t data) {
    uint8_t buffer[2];
    buffer[0] = address;
    buffer[1] = data;
    wiringPiSPIDataRW(SPI_CHANNEL, buffer, 2);
}

//MAX7219_INITIALIZING
void max7219_init() {
    send_data(0x0F, 0x00);      // Test mode
    send_data(0x09, 0xCC);      // Decode mode 
    send_data(0x0A, 0x07);      // Brightness 50%
    send_data(0x0B, 0x07);      // Scan limit 8 Digit
    send_data(0x0C, 0x01);      // Shutdown mode off
}

//CLEAR_LED
void clear() {
    for (int i = 1; i <= 8; i++) {
        send_data(i, 0x0F); 
    }
}

// DISPLAY_TEMPERATURE
void display(float temp) {
    int rounded_temp= (int)round(temp);     //Round temperature value to display
    int digit1 = rounded_temp/10.0;   
    int digit2 = (int) rounded_temp % 10;    


    send_data(8, digit1);              // vị trí 8
    send_data(7, digit2);              // vị trí 7
    send_data(6, 0x63);                // vị trí 6
    send_data(5, 0x4E);                // vị trí 5
}

// DISPLAY_PRESSURE(Psi)
void display_small(float pressure) {
    int rounded_pressure= (int)round(pressure); //Round pressure value to dislay
    int d1 = rounded_pressure / 10.0;      
    int d2 = (int) rounded_pressure % 10;             

    send_data(4, d1);                 // vị trí 4
    send_data(3, d2);                 // vị trí 3
    send_data(2, 0x67);               // vị trí 2
    send_data(1, 0xDB);               // vị trí 1
}
//Write to CSV
void write_csv(float temperature, float pressure){
    FILE *fp;
    fp=fopen("/home/pi/Desktop/data_log.csv","a");
    if (fp==NULL){
        printf("Cannot open file to log\n");
        return;
    }

    time_t t;
    struct tm*tm_info;
    char time_buffer[26];
    time(&t);
    tm_info=localtime(&t);
    strftime(time_buffer,26,"%Y-%m-%d %H:%M:%S",tm_info);
    fprintf(fp, "%s,%.2f°C,%.3f Psi\n",time_buffer,temperature,pressure);
    fclose(fp);
}

//MAIN 
int main()
{
    //SETUP SPI & I2C
    fd = wiringPiI2CSetup(0x77);
    load_calibration();
    wiringPiSPISetup(SPI_CHANNEL, SPI_SPEED);
    max7219_init();
    clear();
    while(1)
    {
        float temperature=read_temperature();
        float pressure=read_pressure();
        pressure=pressure/6895.0;
        printf("Temperature: %.1f C\n",temperature);
        printf("Pressure: %.3f Psi\n",pressure);
        write_csv(temperature,pressure);
        display(temperature);
        display_small(pressure);
        delay(1000);
    }
    return 0;
}
