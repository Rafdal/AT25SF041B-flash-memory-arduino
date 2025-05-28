#include <Arduino.h>
#include <mbed.h>
#include <pinDefinitions.h>
#include "AT25SF041B_defs.h"
#include "FlashMemory.h"

bool FlashMemory::begin(uint8_t CS_pin, uint32_t spi_freq, SPIClass &spiPort)
{
    mbed::DigitalInOut* tmpgpio = digitalPinToGpio(CS_pin);
	if (tmpgpio != NULL)
		delete tmpgpio;
	cs_pin = new mbed::DigitalOut(digitalPinToPinName(CS_pin), HIGH);
    cs_pin->write(1);

	spiPort.begin(); // Initialize the SPI port
    _spiPort = &spiPort;

    settings = SPISettings(spi_freq, MSBFIRST, SPI_MODE0);

	buf[0] = GET_FLASH_ID; // Read Manufacturer ID
	buf[1] = 0x00;
	buf[2] = 0x00;
	buf[3] = 0x00;
    _spiPort->beginTransaction(settings);
	cs_pin->write(0);
	_spiPort->transfer(buf, 4);
	cs_pin->write(1);
	_spiPort->endTransaction();
    uint8_t tmp = buf[1] ^ MANUFACTURER_ID;
    tmp |= (buf[2] ^ DEVICE_ID1);
    tmp |= (buf[3] ^ DEVICE_ID2);
    return (tmp == 0); // Check if the device is the expected one
}

uint8_t FlashMemory::readStatus1()
{
	buf[0] = READ_STATUS_REG1; // Read Status Register
	buf[1] = 0x00;
	_spiPort->beginTransaction(settings);
	cs_pin->write(0);
	_spiPort->transfer(buf, 2);
	cs_pin->write(1);
	_spiPort->endTransaction();
	return buf[1];
}

bool FlashMemory::busy()
{
	return (readStatus1() & BUSY_STATUS_FLAG);
}

bool FlashMemory::blockErase(uint16_t page, uint8_t block_size_opcode)
{
	if (busy() || page > (uint16_t)0x7FF)
		return false; // Device is busy
	_spiPort->beginTransaction(settings);
	cs_pin->write(0);
	_spiPort->transfer(WRITE_ENABLE); // Write Enable
	cs_pin->write(1);
	_spiPort->endTransaction();

	buf[0] = block_size_opcode; // Block Erase command
	buf[1] = (page >> 8) & 0xFF;	// Address byte MSB
	buf[2] = page & 0xFF;	// Address byte LSB
	buf[3] = 0x00;	// Address byte LSB
	_spiPort->beginTransaction(settings);
	cs_pin->write(0);
	_spiPort->transfer(buf, 4);
	cs_pin->write(1);
	_spiPort->endTransaction();
	return true;
}

bool FlashMemory::erase_4KB(uint16_t page)
{
	return blockErase(page, ERASE_4KB);
}
bool FlashMemory::erase_32KB(uint16_t page)
{
	return blockErase(page, ERASE_32KB);
}
bool FlashMemory::erase_64KB(uint16_t page)
{
	return blockErase(page, ERASE_64KB);
}
bool FlashMemory::erase_full()
{
	if (busy())
		return false; 
	_spiPort->beginTransaction(settings);
	cs_pin->write(0);
	_spiPort->transfer(WRITE_ENABLE); // Write Enable
	cs_pin->write(1);
	_spiPort->endTransaction();

	buf[0] = CHIP_ERASE; // Chip Erase command
	_spiPort->beginTransaction(settings);
	cs_pin->write(0);
	_spiPort->transfer(buf, 1);
	cs_pin->write(1);
	_spiPort->endTransaction();
	return true;
}

bool FlashMemory::write_data(uint16_t page, uint8_t low, uint8_t *data, size_t size)
{
	if (busy() || page > (uint16_t)0x7FF)
		return false; // Device is busy
	_spiPort->beginTransaction(settings);
	cs_pin->write(0);
	_spiPort->transfer(WRITE_ENABLE); // Write Enable
	cs_pin->write(1);
	_spiPort->endTransaction();

	buf[0] = BYTE_PAGE_PROGRAM;   // Byte/Page program
	buf[1] = (page >> 8) & 0xFF;	// Address byte MSB
	buf[2] = page & 0xFF;	        // Address byte LSB
	buf[3] = low;	                // Address byte LSB
	memcpy(&buf[4], data, size);
	_spiPort->beginTransaction(settings);
	cs_pin->write(0);
	_spiPort->transfer(buf, size + 4);
	cs_pin->write(1);
	_spiPort->endTransaction();
	return true;
}

void FlashMemory::read_data(uint16_t page, uint8_t low, uint8_t *data, size_t size)
{
	buf[0] = READ_ARRAY; 			// Read Array
	buf[1] = (page >> 8) & 0xFF;	// Address byte MSB
	buf[2] = page & 0xFF;			// Address byte LSB
	buf[3] = low;					// Address byte LSB
	_spiPort->beginTransaction(settings);
	cs_pin->write(0);
	_spiPort->transfer(buf, size + 4);
	cs_pin->write(1);
	_spiPort->endTransaction();
	memcpy(data, &buf[4], size);
}

void FlashMemory::dump_page(uint16_t page, Stream &stream, uint8_t base)
{
	uint8_t data[256];
	read_data(page, 0, data, sizeof(data));
	for (size_t i = 0; i < sizeof(data); i++)
	{
		switch (base)
		{
			case ASCII:
				stream.print((char)data[i]);
				break;
			default:
				stream.print(data[i], base);
				stream.print(" ");
				break;
		}
	}
	stream.println();
}