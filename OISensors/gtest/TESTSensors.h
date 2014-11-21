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
	PacketId * getParseKey (void);
	size_t fnSerialRead (uint8_t * const, const size_t);
} // testing
} // namespace sensor
} // namespace oi
} // namespace series500
} // namespace roomba

#endif

/* Created and copyrighted by Zachary J. Fields. All rights reserved. */
