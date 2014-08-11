/* Created and copyrighted by Zachary J. Fields. All rights reserved. */

#include "OI.h"

#include <chrono>
#include <thread>

namespace roomba {
namespace series500 {

OpenInterface::OpenInterface (
	void
) :
	_fnSerialWrite([](const uint8_t *, size_t){ return 0; }),
	_mode(OFF)
{}

void
OpenInterface::connectToSerialBus (
	const std::function<size_t(const uint8_t *, size_t)> fnSerialWrite_
) {
	_fnSerialWrite = fnSerialWrite_;
}

OpenInterface::ReturnCode
OpenInterface::start (
	void
) {
	const uint8_t serial_data[1] = { command::START };
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	_mode = PASSIVE;
	
	return SUCCESS;
}

OpenInterface::ReturnCode
OpenInterface::baud (
	const BaudCode baud_code_
) const {
	const uint8_t serial_data[2] = { command::BAUD, baud_code_ };
	if ( OFF == _mode ) { return OI_NOT_STARTED; }
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	return SUCCESS;
}

OpenInterface::ReturnCode
OpenInterface::safe (
	void
) {
	const uint8_t serial_data[1] = { command::SAFE };
	if ( OFF == _mode ) { return OI_NOT_STARTED; }
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	_mode = SAFE;
	
	return SUCCESS;
}

} // namespace series500
} // namespace roomba

roomba::series500::OpenInterface OI;

/* Created and copyrighted by Zachary J. Fields. All rights reserved. */
