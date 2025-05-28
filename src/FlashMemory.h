#ifndef FLASH_MEMORY_H
#define FLASH_MEMORY_H

#include <Arduino.h>
#include <stdint.h>
#include <mbed.h>
#include <pinDefinitions.h>
#include <SPI.h>

#define ASCII 0x41

class FlashMemory
{
public:
    FlashMemory(){};

    bool begin(uint8_t CS_pin, uint32_t spi_freq=32000000ul, SPIClass &spiPort = SPI);

	// 90 ms	(A11 - A0 ignored)
	bool erase_4KB(uint16_t page);
	// 210 ms	(A14 - A0 ignored)
	bool erase_32KB(uint16_t page);
	// 360 ms	(A15 - A0 ignored)
	bool erase_64KB(uint16_t page);
	// 3 s
	bool erase_full();

	bool write_data(uint16_t page, uint8_t low, uint8_t *data, size_t size);
	void read_data(uint16_t page, uint8_t low, uint8_t *data, size_t size);

	void dump_page(uint16_t page, Stream &stream, uint8_t base=HEX);

	// true if the device is busy
	bool busy();

private:

// SRP0 | BP4 | BP3 | BP2 | BP1 | BP0 | WEL (Write Enabled) | BUSY
	uint8_t readStatus1();
	bool blockErase(uint16_t page, uint8_t block_size_opcode);

    uint8_t buf[255+4]; // Buffer for SPI transfer (255 bytes + 4 for address and command)

    mbed::DigitalOut *cs_pin = nullptr;   // gpio pin for time measurement purposes
	SPIClass *_spiPort = nullptr; // Pointer to the SPI port
	SPISettings settings;
};

#endif // FLASH_MEMORY_H