/* Created and copyrighted by Zachary J. Fields. Offered as open source under the MIT License (MIT). */

#include <cstdint>
#include <cstring>

#include "mock.h"

namespace roomba {
namespace serial {
namespace mock {

namespace {
	BaudCode _baud_code;
	fn_serial_read _SerialRead;
	fn_serial_write _SerialWrite;
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
	return _SerialWrite(serial_data_, data_length_);
}

BaudCode
getBaudCode (
	void
) {
	return _baud_code;
}

void
setSerialReadFunc (
	const fn_serial_read SerialRead_
) {
	_SerialRead = SerialRead_;
}

void
setSerialWriteFunc (
	const fn_serial_write SerialWrite_
) {
	_SerialWrite = SerialWrite_;
}

} // namespace mock
} // namespace serial
} // namespace roomba

/* Created and copyrighted by Zachary J. Fields. Offered as open source under the MIT License (MIT). */
