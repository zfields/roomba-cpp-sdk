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
	if ( day_mask_ > 127 ) { return INVALID_PARAMETER; }
	
	if ( day_mask_ && clock_times_ ) {
		serial_data[1] = day_mask_;
		for (uint8_t day = 0, parameter_index = 0, serial_index = 2 ; day < 7 ; ++day, parameter_index += ((day_mask_ >> day) & 1), serial_index = ((2 * day) + 2)) {
			// Test conditions without branching logic to allow for code pipelining and parallel execution with loop unroll
			*reinterpret_cast<uint16_t *>(&serial_data[serial_index]) = ((((day_mask_ >> day) & 1) && (clock_times_[parameter_index].hour >= 0 && clock_times_[parameter_index].hour < 23) && (clock_times_[parameter_index].minute >= 0 && clock_times_[parameter_index].minute <= 59)) * (*reinterpret_cast<const uint16_t *>(&clock_times_[parameter_index])));
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
	if ( velocity_ < -500 || velocity_ > 500 || (radius_ != 32767 && (radius_ < -2000 || radius_ > 2000)) ) { return INVALID_PARAMETER; }
	
	serial_data[1] = reinterpret_cast<const uint8_t *>(&velocity_)[1];
	serial_data[2] = reinterpret_cast<const uint8_t *>(&velocity_)[0];
	serial_data[3] = reinterpret_cast<const uint8_t *>(&radius_)[1];
	serial_data[4] = reinterpret_cast<const uint8_t *>(&radius_)[0];
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}

OpenInterface::ReturnCode
OpenInterface::driveDirect (
	const int16_t left_wheel_velocity_,
	const int16_t right_wheel_velocity_
) const {
	uint8_t serial_data[5] = { command::DRIVE_DIRECT };
	if ( OFF == _mode ) { return OI_NOT_STARTED; }
	if ( PASSIVE == _mode ) { return INVALID_MODE_FOR_REQUESTED_OPERATION; }
	if ( left_wheel_velocity_ < -500 || left_wheel_velocity_ > 500 || right_wheel_velocity_ < -500 || right_wheel_velocity_ > 500 ) { return INVALID_PARAMETER; }
	
	serial_data[1] = reinterpret_cast<const uint8_t *>(&right_wheel_velocity_)[1];
	serial_data[2] = reinterpret_cast<const uint8_t *>(&right_wheel_velocity_)[0];
	serial_data[3] = reinterpret_cast<const uint8_t *>(&left_wheel_velocity_)[1];
	serial_data[4] = reinterpret_cast<const uint8_t *>(&left_wheel_velocity_)[0];
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}

OpenInterface::ReturnCode
OpenInterface::drivePWM (
	const int16_t left_wheel_pwm_,
	const int16_t right_wheel_pwm_
) const {
	uint8_t serial_data[5] = { command::DRIVE_PWM };
	if ( OFF == _mode ) { return OI_NOT_STARTED; }
	if ( PASSIVE == _mode ) { return INVALID_MODE_FOR_REQUESTED_OPERATION; }
	if ( left_wheel_pwm_ < -255 || left_wheel_pwm_ > 255 || right_wheel_pwm_ < -255 || right_wheel_pwm_ > 255 ) { return INVALID_PARAMETER; }
	
	serial_data[1] = reinterpret_cast<const uint8_t *>(&right_wheel_pwm_)[1];
	serial_data[2] = reinterpret_cast<const uint8_t *>(&right_wheel_pwm_)[0];
	serial_data[3] = reinterpret_cast<const uint8_t *>(&left_wheel_pwm_)[1];
	serial_data[4] = reinterpret_cast<const uint8_t *>(&left_wheel_pwm_)[0];
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}

OpenInterface::ReturnCode
OpenInterface::motors (
	const bitmask::MotorStates motor_state_mask_
) const {
	uint8_t serial_data[2] = { command::MOTORS };
	if ( OFF == _mode ) { return OI_NOT_STARTED; }
	if ( PASSIVE == _mode ) { return INVALID_MODE_FOR_REQUESTED_OPERATION; }
	if ( motor_state_mask_ > 31 ) { return INVALID_PARAMETER; }
	
	serial_data[1] = motor_state_mask_;
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}

OpenInterface::ReturnCode
OpenInterface::pwmMotors (
	const int8_t main_brush_,
	const int8_t side_brush_,
	const int8_t vacuum_
) const {
	uint8_t serial_data[4] = { command::PWM_MOTORS };
	if ( OFF == _mode ) { return OI_NOT_STARTED; }
	if ( PASSIVE == _mode ) { return INVALID_MODE_FOR_REQUESTED_OPERATION; }
	if ( -128 == main_brush_ || -128 == side_brush_ || vacuum_ < 0 ) { return INVALID_PARAMETER; }
	
	serial_data[1] = main_brush_;
	serial_data[2] = side_brush_;
	serial_data[3] = vacuum_;
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}

OpenInterface::ReturnCode
OpenInterface::leds (
	const bitmask::display::LEDs led_mask_,
	const uint8_t color_,
	const uint8_t intensity_
) const {
	uint8_t serial_data[4] = { command::LEDS };
	if ( OFF == _mode ) { return OI_NOT_STARTED; }
	if ( PASSIVE == _mode ) { return INVALID_MODE_FOR_REQUESTED_OPERATION; }
	if ( led_mask_ > 15 ) { return INVALID_PARAMETER; }

	serial_data[1] = led_mask_;
	serial_data[2] = color_;
	serial_data[3] = intensity_;
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}

OpenInterface::ReturnCode
OpenInterface::schedulingLEDs (
	const bitmask::Days day_mask_,
	const bitmask::display::SchedulingLEDs display_mask_
) const {
	uint8_t serial_data[3] = { command::SCHEDULING_LEDS };
	if ( OFF == _mode ) { return OI_NOT_STARTED; }
	if ( PASSIVE == _mode ) { return INVALID_MODE_FOR_REQUESTED_OPERATION; }
	if ( day_mask_ > 127 || display_mask_ > 31 ) { return INVALID_PARAMETER; }
	
	serial_data[1] = day_mask_;
	serial_data[2] = display_mask_;
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}

OpenInterface::ReturnCode
OpenInterface::digitLEDsRaw (
	const bitmask::display::DigitN raw_leds_[4]
) const {
	uint8_t serial_data[5] = { command::DIGIT_LEDS_RAW };
	if ( OFF == _mode ) { return OI_NOT_STARTED; }
	if ( PASSIVE == _mode ) { return INVALID_MODE_FOR_REQUESTED_OPERATION; }
	if ( raw_leds_[0] > 127 || raw_leds_[1] > 127 || raw_leds_[2] > 127 || raw_leds_[3] > 127 ) { return INVALID_PARAMETER; }
	
	*reinterpret_cast<uint32_t *>(&serial_data[1]) = *reinterpret_cast<const uint32_t *>(raw_leds_);
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}

OpenInterface::ReturnCode
OpenInterface::digitLEDsASCII (
	const char ascii_leds_[4]
) const {
	uint8_t serial_data[5] = { command::DIGIT_LEDS_ASCII };
	if ( OFF == _mode ) { return OI_NOT_STARTED; }
	if ( PASSIVE == _mode ) { return INVALID_MODE_FOR_REQUESTED_OPERATION; }
	if ( ascii_leds_[0] < 32 || ascii_leds_[0] > 126 || ascii_leds_[1] < 32 || ascii_leds_[1] > 126 || ascii_leds_[2] < 32 || ascii_leds_[2] > 126 || ascii_leds_[3] < 32 || ascii_leds_[3] > 126 ) { return INVALID_PARAMETER; }
	
	*reinterpret_cast<uint32_t *>(&serial_data[1]) = *reinterpret_cast<const uint32_t *>(ascii_leds_);
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}

OpenInterface::ReturnCode
OpenInterface::buttons (
	const bitmask::Buttons button_mask_
) const {
	uint8_t serial_data[2] = { command::BUTTONS };
	if ( OFF == _mode ) { return OI_NOT_STARTED; }
	
	serial_data[1] = button_mask_;
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}

OpenInterface::ReturnCode
OpenInterface::song (
	const uint8_t song_number_,
	const std::vector<note_t> & notes_
) const {
	uint8_t serial_data[35] = { command::SONG };
	uint8_t data_index = 2;
	if ( OFF == _mode ) { return OI_NOT_STARTED; }
	if ( song_number_ > 4 || !notes_.size() || notes_.size() > 16 ) { return INVALID_PARAMETER; }
	
	serial_data[1] = song_number_;
	serial_data[2] = notes_.size();
	
	for (auto &note : notes_) {
		serial_data[++data_index] = note.first;
		serial_data[++data_index] = note.second;
	}
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}

OpenInterface::ReturnCode
OpenInterface::play (
	const uint8_t song_number_
) const {
	uint8_t serial_data[2] = { command::PLAY };
	if ( OFF == _mode ) { return OI_NOT_STARTED; }
	if ( PASSIVE == _mode ) { return INVALID_MODE_FOR_REQUESTED_OPERATION; }
	if ( song_number_ > 4 ) { return INVALID_PARAMETER; }
	
	serial_data[1] = song_number_;
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}

OpenInterface::ReturnCode
OpenInterface::sensors (
	const sensor::PacketId packet_id_
) const {
	uint8_t serial_data[2] = { command::SENSORS };
	if ( OFF == _mode ) { return OI_NOT_STARTED; }
	if ( (packet_id_ > 58 && packet_id_ < 100) || packet_id_ > 107 ) { return INVALID_PARAMETER; }

	serial_data[1] = packet_id_;
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}

OpenInterface::ReturnCode
OpenInterface::queryList (
	const std::vector<sensor::PacketId> & sensor_list_
) const {
	const uint8_t bytes = sensor_list_.size();
	uint8_t serial_data[(2 + bytes)];
	uint8_t data_index = 1;
	if ( OFF == _mode ) { return OI_NOT_STARTED; }
	
	serial_data[0] = command::QUERY_LIST;
	serial_data[1] = bytes;
	
	for (auto &sensor : sensor_list_) {
		if ( (sensor > 58 && sensor < 100) || sensor > 107 ) { continue; }
		serial_data[++data_index] = sensor;
	}
	if ( 1 == data_index ) { return INVALID_PARAMETER; }
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}

} // namespace series500
} // namespace roomba

roomba::series500::OpenInterface OI;

/* Created and copyrighted by Zachary J. Fields. All rights reserved. */
