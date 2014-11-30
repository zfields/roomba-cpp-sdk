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
	uint_opt64_t getFlagMaskDirty (void);
	PacketId * getParseKey (void);
	std::chrono::time_point<std::chrono::steady_clock, std::chrono::milliseconds> getTransferCompletionTimeMs (void);
	size_t fnSerialRead (uint_opt8_t * const, const size_t);
} // testing
} // namespace sensor
} // namespace oi
} // namespace series500
} // namespace roomba

#endif

/* Created and copyrighted by Zachary J. Fields. All rights reserved. */
