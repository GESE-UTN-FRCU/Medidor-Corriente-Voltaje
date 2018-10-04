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
	index = INIT_INDEX;

}

void Storage::store(t_measure measure){
	
    byte *measurebytes = new byte[struct_size];
    char charbuffer[struct_size];

    memcpy(charbuffer,&measure,struct_size);

    measurebytes = (byte*) charbuffer;

    i2c_eeprom_write_page(DEVICE,index,measurebytes,(byte) struct_size);

	index = index + struct_size;
	
}

void Storage::retriveLast(){
	
    byte *measurebytes = new byte[struct_size];
    char charbuffer[struct_size];
    
    i2c_eeprom_read_buffer(DEVICE,index,measurebytes,struct_size);

    for(int i=0; i<=(struct_size-1);i++){
        charbuffer[i] = (char) measurebytes[i];
    }
    
	memcpy(&lastReadedMeasure,charbuffer,struct_size); 
        
}

void Storage::removeLast(){
	
	index = index - struct_size;
        
}

void Storage::resetStorage(){
    //SAVE INIT INDEX FROM EPPROM
	index = INIT_INDEX;
}