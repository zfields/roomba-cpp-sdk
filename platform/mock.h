/* Created and copyrighted by Zachary J. Fields. All rights reserved. */

#if defined(TESTING)

#ifndef MOCK_H
#define MOCK_H

#include <cstdint>
#include <functional>

#include "../OIDefines.h"

namespace roomba {
namespace serial {
namespace mock {

typedef std::function<size_t(uint_opt8_t * const data_buffer_, const size_t buffer_length_)> fn_serial_read;
typedef std::function<size_t(const uint_opt8_t * const serial_data_, const size_t data_length_)> fn_serial_write;

void
beginAtBaudCode (
	const roomba::BaudCode baud_code_
);

size_t
delayMs (
	const size_t desired_milliseconds_
);

size_t
delayUs (
	const size_t desired_microseconds_
);

size_t
multiByteSerialRead (
	uint_opt8_t * const data_buffer_,
	const size_t buffer_length_,
	const uint_opt32_t timeout_ms_
);

size_t
multiByteSerialWrite (
	const uint_opt8_t * const serial_data_,
	const size_t data_length_
);

  /*************************/
 /* MOCK HELPER FUNCTIONS */
/*************************/

BaudCode
getBaudCode (
	void
);

void
setSerialReadFunc (
	const fn_serial_read SerialRead_
);

void
setSerialWriteFunc (
	const fn_serial_write SerialWrite_
);

} // namespace mock
} // namespace serial
} // namespace roomba

#endif

#endif

/* Created and copyrighted by Zachary J. Fields. All rights reserved. */
