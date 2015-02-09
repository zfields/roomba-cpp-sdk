/* Created and copyrighted by Zachary J. Fields. Offered as open source under the MIT License (MIT). */

#ifndef ROOMBA_CPP_SDK_H
#define ROOMBA_CPP_SDK_H

#include "defines.h"
#include "hardware/state.h"
#include "open_interface/open_interface.h"
#include "platform/serial.h"

template <enum roomba::OISeries OI_SERIES>
struct Roomba {
	static roomba::OpenInterface<OI_SERIES> oi;
};

#endif

/* Created and copyrighted by Zachary J. Fields. Offered as open source under the MIT License (MIT). */
