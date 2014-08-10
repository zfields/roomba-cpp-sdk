/* Created and copyrighted by Zachary J. Fields. All rights reserved. */

#include "OI.h"

namespace roomba {
namespace series500 {

OpenInterface::OpenInterface (
	void
) :
	_fnSerialWrite([](const uint8_t *, size_t){ return 0; }),
	_baud_code(BAUD_115200),
	_mode(OFF)
{}

OpenInterface::ReturnCode
OpenInterface::begin (
	const std::function<size_t(const uint8_t *, size_t)> fnSerialWrite_,
	const BaudCode baud_code_
) {
	if ( baud_code_ != BAUD_19200 && baud_code_ != BAUD_115200 ) { return INVALID_NON_OI_BAUD_RATE; }
	
	_fnSerialWrite = fnSerialWrite_;
	_baud_code = baud_code_;
	
	return SUCCESS;
}

OpenInterface::ReturnCode
OpenInterface::start (
	void
) {
	const uint8_t opcode(128);
	
	_fnSerialWrite(&opcode, 1);
	_mode = PASSIVE;
	
	return SUCCESS;
}

} // namespace series500
} // namespace roomba

roomba::series500::OpenInterface OI;

/* Created and copyrighted by Zachary J. Fields. All rights reserved. */
