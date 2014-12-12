/* Created and copyrighted by Zachary J. Fields. All rights reserved. */

#ifndef TEST_SENSORS_H
#define TEST_SENSORS_H

#include <chrono>
#include <cstdint>

#include "../OISensors.h"

namespace roomba {
namespace series500 {
namespace oi {
namespace sensors {
namespace testing {
	BaudCode
	getBaudCode (
		void
	);
	
	uint_opt64_t
	getFlagMaskDirty (
		void
	);
	
	uint_opt64_t
	getFlagMaskSigned (
		void
	);
	
	PacketId *
	getParseKey (
		void
	);
	
	uint_opt8_t *
	getRawData (
		void
	);
	
	std::chrono::time_point<std::chrono::steady_clock, std::chrono::milliseconds>
	getSerialReadNextAvailableMs (
		void
	);
	
	size_t
	fnSerialRead (
		uint_opt8_t * const,
		const size_t
	);

	void
	setInternalsToInitialState (
		void
	);
} // testing
} // namespace sensor
} // namespace oi
} // namespace series500
} // namespace roomba

#endif

/* Created and copyrighted by Zachary J. Fields. All rights reserved. */
