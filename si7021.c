#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"


#define Si7021Addresse 0X40
#define Si7021VitesseBus 400000

#define I2Cport i2c1
#define GP2 2 //data
#define GP3 3 //clock

const uint8_t Si7021MesureTemperature=0xE3;
const uint8_t Si7021MesureHumidite=0xE5;

uint8_t controleCrc(uint32_t mot)
{
uint32_t crc = 0b100110001<<15;
uint32_t resultat = mot^crc;
uint32_t mask = 0b100000000000000000000000;
uint8_t position = 0;

while(position<=15)
{
if((resultat&mask)==0){
mask=mask>>1;
position++;
}
else{
resultat = resultat^(crc>>position);
}
}
return resultat;
}


float LectureTemperature(){

uint8_t buf[3];

i2c_write_blocking(I2Cport,Si7021Addresse,&Si7021MesureTemperature,1,true);
i2c_read_blocking(I2Cport,Si7021Addresse,buf,3,false);

uint32_t checksum = (((buf[0])<<16)|((buf[1])<<8)|(buf[2]));
if ((controleCrc(checksum))==0)
{printf("continue\n");}
else
{printf("reset\n");}

uint16_t temp_code = ((buf[0])<<8)|(buf[1]);

//printf("msb=%x,lsb=%x,checksum=%x,concatenation=%x\n",buf[0],buf[1],buf[2],temp_code);
//printf("msb=%x,lsb=%x,concatenation=%x\n",buf[0],buf[1],temp_code);

float temperature = (((175.72f*temp_code)/65536.0)-46.85f);
return temperature;
}

float LectureHumidite(){

uint8_t buf[3];

i2c_write_blocking(I2Cport,Si7021Addresse,&Si7021MesureHumidite,1,true);
i2c_read_blocking(I2Cport,Si7021Addresse,buf,3,false);

uint32_t checksum = (((buf[0])<<16)|((buf[1])<<8)|(buf[2]));
if ((controleCrc(checksum))==0)
{printf("continue\n");}
else
{printf("reset\n");}

uint16_t RH_code = ((buf[0])<<8)|(buf[1]&0XFC);

//printf("msb=%x,lsb=%x,concatenation=%x\n",buf[0],buf[1],RH_code);
//printf("msb=%x,lsb=%x,checksum=%x,concatenation=%x\n",buf[0],buf[1],buf[2],RH_code);

float humidite = (((125*RH_code)/65536.0)-6);
return humidite;
}

int main(){ 

stdio_init_all(); 

sleep_ms(1000);
 
i2c_init(I2Cport,Si7021VitesseBus); 

gpio_set_function(GP2,GPIO_FUNC_I2C);
gpio_set_function(GP3,GPIO_FUNC_I2C);
 
gpio_pull_up(GP2);
gpio_pull_up(GP3);

while(1){
printf("Temperature captee=%.2f\n",LectureTemperature());
sleep_ms(1550);

printf("Humidite captee=%.2f\n",LectureHumidite());
sleep_ms(1550);
}



}





