/* Created and copyrighted by Zachary J. Fields. Offered as open source under the MIT License (MIT). */

#if defined(TESTING)

#ifndef TEST_STATE_H
#define TEST_STATE_H

#include <chrono>
#include <cstdint>

#include "../hardware/state.h"

namespace roomba {
namespace state {
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

OIMode
getOIMode (
	void
);

sensor::PacketId *
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

void
setInternalsToInitialState (
	void
);

} // testing
} // namespace state
} // namespace roomba

#endif

#endif

/* Created and copyrighted by Zachary J. Fields. Offered as open source under the MIT License (MIT). */
