/* Created and copyrighted by Zachary J. Fields. All rights reserved. */

#include "OI.h"

#include <chrono>
#include <thread>

namespace roomba {
namespace series500 {

OpenInterface::OpenInterface (
	void
) :
	_fnSerialWrite([](const uint8_t *, size_t){ return 0; }),
	_mode(OFF)
{}

void
OpenInterface::connectToSerialBus (
	const std::function<size_t(const uint8_t *, size_t)> fnSerialWrite_
) {
	_fnSerialWrite = fnSerialWrite_;
}

OpenInterface::ReturnCode
OpenInterface::start (
	void
) {
	const uint8_t serial_data[1] = { command::START };
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	_mode = PASSIVE;
	
	return SUCCESS;
}

OpenInterface::ReturnCode
OpenInterface::baud (
	const BaudCode baud_code_
) const {
	const uint8_t serial_data[2] = { command::BAUD, baud_code_ };
	if ( OFF == _mode ) { return OI_NOT_STARTED; }
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	return SUCCESS;
}

OpenInterface::ReturnCode
OpenInterface::safe (
	void
) {
	const uint8_t serial_data[1] = { command::SAFE };
	if ( OFF == _mode ) { return OI_NOT_STARTED; }
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	_mode = SAFE;
	
	return SUCCESS;
}

OpenInterface::ReturnCode
OpenInterface::control (
	void
) {
	return safe();
}

OpenInterface::ReturnCode
OpenInterface::full (
	void
) {
	const uint8_t serial_data[1] = { command::FULL };
	if ( OFF == _mode ) { return OI_NOT_STARTED; }
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	_mode = FULL;
	
	return SUCCESS;
}

OpenInterface::ReturnCode
OpenInterface::clean (
	void
) {
	const uint8_t serial_data[1] = { command::CLEAN };
	if ( OFF == _mode ) { return OI_NOT_STARTED; }
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	_mode = PASSIVE;
	
	return SUCCESS;
}

OpenInterface::ReturnCode
OpenInterface::max (
	void
) {
	const uint8_t serial_data[1] = { command::MAX };
	if ( OFF == _mode ) { return OI_NOT_STARTED; }
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	_mode = PASSIVE;
	
	return SUCCESS;
}

OpenInterface::ReturnCode
OpenInterface::spot (
	void
) {
	const uint8_t serial_data[1] = { command::SPOT };
	if ( OFF == _mode ) { return OI_NOT_STARTED; }
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	_mode = PASSIVE;
	
	return SUCCESS;
}

OpenInterface::ReturnCode
OpenInterface::seekDock (
	void
) {
	const uint8_t serial_data[1] = { command::SEEK_DOCK };
	if ( OFF == _mode ) { return OI_NOT_STARTED; }
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	_mode = PASSIVE;
	
	return SUCCESS;
}

OpenInterface::ReturnCode
OpenInterface::schedule (
	const bitmask::Days day_mask_,
	const clock_time_t * const clock_times_
) const {
	uint8_t serial_data[16] = { command::SCHEDULE };
	if ( OFF == _mode ) { return OI_NOT_STARTED; }
	
	if ( day_mask_ && clock_times_ ) {
		serial_data[1] = day_mask_;
		for (uint8_t day = 0, parameter_index = 0, serial_index = 2 ; day < 7 ; ++day, parameter_index += !!(day_mask_ & (1 << day)), serial_index = ((2 * day) + 2)) {
			// Test conditions without branching logic to allow for code pipelining and parallel execution with loop unroll
			*reinterpret_cast<uint16_t *>(&serial_data[serial_index]) = ((!!(day_mask_ & (1 << day)) && (clock_times_[parameter_index].hour >= 0 && clock_times_[parameter_index].hour < 23) && (clock_times_[parameter_index].minute >= 0 && clock_times_[parameter_index].minute <= 59)) * (*reinterpret_cast<const uint16_t *>(&clock_times_[parameter_index])));
		}
	}
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}

OpenInterface::ReturnCode
OpenInterface::setDayTime (
	const Day day_,
	const clock_time_t clock_time_
) const {
	uint8_t serial_data[4] = { command::SET_DAY_TIME };
	if ( OFF == _mode ) { return OI_NOT_STARTED; }
	if ( clock_time_.hour < 0 || clock_time_.hour > 23 || clock_time_.minute < 0 || clock_time_.minute > 59 ) { return INVALID_PARAMETER; }
	
	serial_data[1] = day_;
	*reinterpret_cast<uint16_t *>(&serial_data[2]) = (*reinterpret_cast<const uint16_t *>(&clock_time_));
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}

OpenInterface::ReturnCode
OpenInterface::power (
	void
) {
	uint8_t serial_data[1] = { command::POWER };
	if ( OFF == _mode ) { return OI_NOT_STARTED; }
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	_mode = PASSIVE;
	
	return SUCCESS;
}

OpenInterface::ReturnCode
OpenInterface::drive (
	const int16_t velocity_,
	const int16_t radius_
) const {
	uint8_t serial_data[5] = { command::DRIVE };
	if ( OFF == _mode ) { return OI_NOT_STARTED; }
	if ( PASSIVE == _mode ) { return INVALID_MODE_FOR_REQUESTED_OPERATION; }
	if ( velocity_ < -500 || velocity_ > 500 || radius_ != 32767 && (radius_ < -2000 || radius_ > 2000) ) { return INVALID_PARAMETER; }
	
	*reinterpret_cast<int16_t *>(&serial_data[1]) = velocity_;
	*reinterpret_cast<int16_t *>(&serial_data[3]) = radius_;
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}

} // namespace series500
} // namespace roomba

roomba::series500::OpenInterface OI;

/* Created and copyrighted by Zachary J. Fields. All rights reserved. */
