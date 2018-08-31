#include "storage.h"

void i2c_eeprom_write_page(int deviceaddress,unsigned int eeaddresspage,byte* data,byte length){
  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddresspage >> 8)); // MSB
  Wire.write((int)(eeaddresspage & 0xFF)); // LSB
  byte c;
  for ( c = 0; c < length; c++) Wire.write(data[c]);
  Wire.endTransmission();
}

void i2c_eeprom_read_buffer(int deviceaddress,unsigned int eeaddress,byte *buffer,int length){
  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddress >> 8)); // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.endTransmission();
  Wire.requestFrom(deviceaddress,length);
  int c = 0;
  for ( c = 0; c < length; c++ ) if (Wire.available()) buffer[c] = Wire.read();
}

Storage::Storage(bool debug){
    this->_debug=debug;
}


void Storage::setup(){
    //REPLACE WITH READ FROM EPPROM
    this->index = this->INIT_INDEX;

}

void Storage::store(t_measure measure){
    byte data = 20;



    i2c_eeprom_write_page(this->DEVICE,this->index,&data,4);
        
}

void Storage::retriveLast(){
    byte *buffer;
   // i2c_eeprom_read_buffer(this->DEVICE,this->index-8,buffer,4);
        
}

void Storage::removeLast(){
        
}

void Storage::resetStorage(){
    //SAVE INIT INDEX FROM EPPROM
    this->index = this->INIT_INDEX;
}