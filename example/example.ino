#include <Arduino.h>

#include "pin_defs.h"

// FLASH SPI
#include <SPI.h>
#include "FlashMemory.h"
FlashMemory flash;

#define CS_M 10 // Chip Select pin for the flash memory

void setup()
{
	Serial.begin(115200); // Consola Serial
	while (!Serial);

	Serial.println("Starting Flash Memory Test...");
	delay(500);
	
	if(flash.begin(CS_M, 32000000ul)) {
		Serial.println("Flash Memory initialized successfully.");
	} else {
		Serial.println("Failed to initialize Flash Memory.");
	}
	
	Serial.println("Press 'e' to erase the first 4KB block...");
	Serial.println("Press 'd' to dump the first page...");
	Serial.println("Press 'w' to write \"Hello World\", somewhere to the first page...");
	Serial.println("Press 'r' to read the written data...");
}

void loop()
{
	while(!Serial.available()) { ; } // Wait for user input
	char input = Serial.read();

	switch(input) {
		case 'e':
			if(flash.erase_4KB(0x0000)) { // Erase first 4KB block
				Serial.println("Erasing first 4KB block...");
				while(flash.busy()) {
					Serial.print(".");
					delay(1);
				}
				Serial.println(" Erase complete.");
			} else {
				Serial.println("Failed to erase first 4KB block.");
			}
			break;
		case 'd':
			flash.dump_page(0x0000, Serial, HEX); // Dump first page to Serial
			flash.dump_page(0x0000, Serial, ASCII); // Dump first page to Serial
			break;
		case 'w':
			if(flash.write_data(0x0000, 0x0A, (uint8_t *)"Hello World", 12)) {
				Serial.println("Data written successfully.");
			} else {
				Serial.println("Failed to write data.");
			}
			break;
		case 'r':
			uint8_t data[256];
			flash.read_data(0x0000, 0x0A, data, 12); // Read the first 12 bytes
			Serial.print("Read data: ");
			for (size_t i = 0; i < 12; i++) {
				Serial.print((char)data[i]);
			}
			Serial.println();
			break;
		default:
			break;
	}
}