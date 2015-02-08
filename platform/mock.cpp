/* Created and copyrighted by Zachary J. Fields. All rights reserved. */

#include <cstdint>
#include <cstring>

#include "mock.h"

namespace roomba {
namespace serial {
namespace mock {

namespace {
	BaudCode _baud_code;
	uint_opt8_t _serial_bus[256] = { 0 };
	fn_serial_read _SerialRead;
} // namespace

void
beginAtBaudCode (
	const BaudCode baud_code_
) {
	_baud_code = baud_code_;
	return;
}

size_t
delayMs (
	const size_t desired_milliseconds_
) {
	return desired_milliseconds_;
}

size_t
delayUs (
	const size_t desired_microseconds_
) {
	return desired_microseconds_;
}

size_t
multiByteSerialRead (
	uint_opt8_t * const data_buffer_,
	const size_t buffer_length_,
	const uint_opt32_t timeout_ms_
) {
	return _SerialRead(data_buffer_, buffer_length_);
}

size_t
multiByteSerialWrite (
	const uint_opt8_t * const serial_data_,
	const size_t data_length_
) {
	memcpy(_serial_bus, serial_data_, data_length_);
	return strlen(reinterpret_cast<char *>(_serial_bus));
}

BaudCode
getBaudCode (
	void
) {
	return _baud_code;
}

uint_opt8_t const * const
getSerialBus (
	void
) {
	return _serial_bus;
}

void
setSerialReadFunc (
	const fn_serial_read SerialRead_
) {
	_SerialRead = SerialRead_;
}

} // namespace mock
} // namespace serial
} // namespace roomba

/* Created and copyrighted by Zachary J. Fields. All rights reserved. */
