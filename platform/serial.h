/* Created and copyrighted by Zachary J. Fields. Offered as open source under the MIT License (MIT). */

#ifndef SERIAL_H
#define SERIAL_H

#include "../OIDefines.h"

#if defined(TESTING)
	#include "mock.h"
#elif defined(ARDUINO) || defined(SPARK)
	#include "wiring.h"
#endif

namespace roomba {

/// \brief Serial Platform Abstraction Layer
/// \details This class serves as a serial platform abstraction layer, which
/// lists the basic services to be provided by a serial platform. An
/// explicit serial protocol will be instantiated at compilation.
/// (i.e. Wiring, Linux, Windows_NT, Windows_UAP, etc...).
namespace serial {

/// \brief Begin the serial connection
/// \details A serial connection will be started at the rate
/// represented by the baud code.
/// \param [in] baud_code_ The code indicating a specific rate
inline
void
beginAtBaudCode (
	roomba::BaudCode baud_code_
) {
#if defined(TESTING)
	return mock::beginAtBaudCode(baud_code_);
#elif defined(ARDUINO) || defined(SPARK)
	return wiring::beginAtBaudCode(baud_code_);
#else
	return;
#endif
}

/// \brief Delay in milliseconds
/// \param [in] desired_ms_ The number of milliseconds you wish to wait
/// \return The actual duration of the delay in milliseconds
inline
size_t
delayMs (
	const size_t desired_ms_
) {
#if defined(TESTING)
	return mock::delayMs(desired_ms_);
#elif defined(ARDUINO) || defined(SPARK)
	return wiring::delayMs(desired_ms_);
#else
	return 0;
#endif
}

/// \brief Delay in microseconds
/// \param [in] desired_us_ The number of microseconds you wish to wait
/// \return The actual duration of the delay in microseconds
inline
size_t
delayUs (
	const size_t desired_us_
) {
#if defined(TESTING)
	return mock::delayUs(desired_us_);
#elif defined(ARDUINO) || defined(SPARK)
	return wiring::delayUs(desired_us_);
#else
	return 0;
#endif
}

/// \brief A function supplying multi-byte read access to the serial bus
/// \param [out] data_buffer_ A buffer used for transfering the contents
/// of the serial bus
/// \param [in] buffer_length_ The length of the buffer
/// \param [in] timeout_ms_ The number of milliseconds to wait before
/// returning with a timeout error. [default value: 1000]
/// \return The actual number of bytes placed in the buffer provided
inline
size_t
multiByteSerialRead (
	uint_opt8_t * const data_buffer_,
	const size_t buffer_length_,
	const uint_opt32_t timeout_ms_ = 1000
) {
#if defined(TESTING)
	return mock::multiByteSerialRead(data_buffer_, buffer_length_, timeout_ms_);
#elif defined(ARDUINO) || defined(SPARK)
	return wiring::multiByteSerialRead(data_buffer_, buffer_length_, timeout_ms_);
#else
	return 0;
#endif
}

/// \brief A function supplying multi-byte write access to the serial bus
/// \param [in] serial_data_ A serialized data string to be sent on the
/// serial bus
/// \param [in] data_length_ The length of the data at the pointer passed
/// in the first parameter
/// \return The actual number of bytes written to the serial bus
inline
size_t
multiByteSerialWrite (
	const uint_opt8_t * const serial_data_,
	const size_t data_length_
) {
#if defined(TESTING)
	return mock::multiByteSerialWrite(serial_data_, data_length_);
#elif defined(ARDUINO) || defined(SPARK)
	return wiring::multiByteSerialWrite(serial_data_, data_length_);
#else
	return 0;
#endif
}

} // namespace serial
} // namespace roomba

#endif

/* Created and copyrighted by Zachary J. Fields. Offered as open source under the MIT License (MIT). */
