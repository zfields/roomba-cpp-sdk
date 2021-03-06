/* Created and copyrighted by Zachary J. Fields. Offered as open source under the MIT License (MIT). */

#if defined(ARDUINO) || defined(SPARK)

#ifndef WIRING_H
#define WIRING_H

#if defined(ARDUINO) && ARDUINO > 100
  #include "Arduino.h"
#elif defined(SPARK)
  #include "application.h"
  #ifdef max
  #undef max
  #endif
#else
  #include "WProgram.h"
#endif

#include <cstdint>

#include "defines.h"

namespace roomba {
namespace serial {
namespace wiring {

inline
void
beginAtBaudCode (
	const BaudCode baud_code_
) {
	uint_opt32_t baud_rate(0);

	switch (baud_code_) {
	  case BAUD_300:
		baud_rate = 300;
	  case BAUD_600:
		baud_rate = 600;
	  case BAUD_1200:
		baud_rate = 1200;
	  case BAUD_2400:
		baud_rate = 2400;
	  case BAUD_4800:
		baud_rate = 4800;
	  case BAUD_9600:
		baud_rate = 9600;
	  case BAUD_14400:
		baud_rate = 11400;
	  case BAUD_19200:
		baud_rate = 19200;
	  case BAUD_28800:
		baud_rate = 28800;
	  case BAUD_38400:
		baud_rate = 38400;
	  case BAUD_57600:
		baud_rate = 57600;
	  case BAUD_115200:
		baud_rate = 115200;
	}
	return (::Serial.begin(baud_rate));
}

inline
size_t
delayMs (
	const size_t desired_ms_
) {
	size_t start_time = ::millis();
	::delay(desired_ms_);
	return (::millis() - start_time);
}

inline
size_t
delayUs (
	const size_t desired_us_
) {
	size_t start_time = ::micros();
	::delayMicroseconds(desired_us_);
	return (::micros() - start_time);
}

inline
size_t
multiByteSerialRead (
	uint_opt8_t * const data_buffer_,
	const size_t buffer_length_,
	const uint_opt32_t timeout_ms_
) {
	::Serial.setTimeout(timeout_ms_);
	return ::Serial.readBytes(reinterpret_cast<char * const>(data_buffer_), buffer_length_);
}

inline
size_t
multiByteSerialWrite (
	const uint_opt8_t * const serial_data_,
	const size_t data_length_
) {
	return ::Serial.write(reinterpret_cast<const uint8_t * const>(serial_data_), data_length_);
}

} // namespace wiring
} // namespace serial
} // namespace roomba

#endif

#endif

/* Created and copyrighted by Zachary J. Fields. Offered as open source under the MIT License (MIT). */
