/* Created and copyrighted by Zachary J. Fields. All rights reserved. */

#ifndef TEST_SENSORS_H
#define TEST_SENSORS_H

#include <cstdint>

#include "../OISensors.h"

namespace roomba {
namespace series500 {
namespace oi {
namespace sensors {
namespace testing {
	BaudCode getBaudCode (void);
	PacketId * getParseKey (void);
	std::chrono::system_clock::time_point getTransferCompleteTime (void);
	size_t fnSerialRead (uint_opt8_t * const, const size_t);
} // testing
} // namespace sensor
} // namespace oi
} // namespace series500
} // namespace roomba

#endif

/* Created and copyrighted by Zachary J. Fields. All rights reserved. */
