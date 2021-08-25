#include <SPI.h>
// Sensor select pin for activation
const int sensor1_select = 10;
const int sensor2_select = 9;
// Required for force calculation based on datasheet
const float output_max = 13107.2;
const float output_min = 3276.8; 

uint16_t sensor1_force_data=0;
uint16_t sensor1_temperature_data=0;
uint16_t sensor1_status = 0;

uint16_t sensor2_force_data=0;
uint16_t sensor2_temperature_data=0;
uint16_t sensor2_status = 0;

// Global SPI settings
#define DATA_ORDER MSBFIRST
#define SPI_MODE   SPI_MODE0
#define SPI_SPEED  800000
// SPI Settings 
SPISettings MySPISettings(SPI_SPEED, DATA_ORDER, SPI_MODE);

void setup() {
  // Serial communiation initialization
  Serial.begin(9600);
  // set the Slave Select Pins as outputs:
  pinMode(sensor1_select, OUTPUT);
  pinMode(sensor2_select, OUTPUT);
  digitalWrite(sensor1_select, HIGH);
  digitalWrite(sensor2_select, HIGH);

  // SPI communication initialization 
  SPI.begin();
 }
float temperature_val=0;
float force_val = 0 ;
float force_val2 = 0 ;
float temperature_val2 = 0;
void loop() {
  // SPI start transaction
  SPI.beginTransaction(MySPISettings);
  // Activate chip
  digitalWrite(sensor1_select, LOW);
  // Read two byte data for force and 2 byte data for temperature.
  sensor1_force_data = SPI.transfer16(0);
  sensor1_temperature_data = SPI.transfer16(0);
  temperature_val = (sensor1_temperature_data & 0b1111111111100000) >> 5;
  temperature_val = (((temperature_val/2047.0)*200)-50);
  // Deactivate sensor
  digitalWrite(sensor1_select, HIGH);
  // End transaction for first sensor
  SPI.endTransaction();
  // First 2 bit of 2 byte data is status bit and the rest 14 bits are representing force values.
  sensor1_status = (sensor1_force_data & 0b1100000000000000) >> 14;
  // if status of sensor is valid print data.
  if(!sensor1_status){
    sensor1_force_data = sensor1_force_data & 0b11111111111111;
    // Conversion of 14 bit force readout to Newton.
    force_val = ((sensor1_force_data-output_min)*10.0)/(output_max-output_min);
    Serial.print("Sensor 1 force value= ");
    Serial.print(force_val);
    Serial.print("  | Temperature value= ");
    Serial.println(temperature_val);
  }
  else if (sensor1_status==0x01)
    Serial.println("Sensor 1 : Device in command mode");
  else if (sensor1_status==0x02)
    Serial.println("Sensor 1 : Stale (duplicate) data, data has been fetched before.");
  else if (sensor1_status==0x03)
    Serial.println("Sensor 1 : Diagnostic Condition");
// ------------------ First sensor reading ended ----------------------------------------
// ------------------ Second sensor reading started----------------------------------------

  SPI.beginTransaction(MySPISettings);
  // Activate chip
  digitalWrite(sensor2_select, LOW);
  // Read two byte data for force and 2 byte data for temperature.
  sensor2_force_data = SPI.transfer16(0);
  sensor2_temperature_data = SPI.transfer16(0);
  temperature_val2 = (sensor2_temperature_data & 0b1111111111100000) >> 5;
  temperature_val2 = (((temperature_val2/2047.0)*200)-50);
  // Deactivate sensor
  digitalWrite(sensor2_select, HIGH);
  // End transaction for second sensor
  SPI.endTransaction();
  // First 2 bit of 2 byte data is status bit and the rest 14 bits are representing force values.
  sensor2_status = (sensor2_force_data & 0b1100000000000000) >> 14;
  // if status of sensor is valid print data.
  if(!sensor2_status){
    sensor2_force_data = sensor2_force_data & 0b11111111111111;
    // Conversion of 14 bit force readout to Newton.
    force_val2 = ((sensor2_force_data-output_min)*10.0)/(output_max-output_min);
    Serial.print("Sensor 2 force value= ");
    Serial.print(force_val2);
    Serial.print("  | Temperature value= ");
    Serial.println(temperature_val2);
  }
  else if (sensor1_status==0x01)
    Serial.println("Sensor 2 : Device in command mode");
  else if (sensor1_status==0x02)
    Serial.println("Sensor 2 : Stale (duplicate) data, data has been fetched before.");
  else if (sensor1_status==0x03)
    Serial.println("Sensor 2 : Diagnostic Condition");
// ---------------------------------- Second sensor reading ended ------------------------------
  delayMicroseconds(1000);

  temperature_val=0;
  sensor1_force_data=0;
  force_val=0;
}
