/* Created and copyrighted by Zachary J. Fields. All rights reserved. */

#include "open_interface.h"

#include <chrono>
#include <thread>

namespace roomba {

template<>
ReturnCode
OpenInterface<OI500>::start (
	void
) {
	const uint_opt8_t serial_data[1] = { command::START };
	
	if ( !serial::multiByteSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	state::setOIMode(PASSIVE);
	
	return SUCCESS;
}

template<>
ReturnCode
OpenInterface<OI500>::baud (
	const BaudCode baud_code_
) {
	const uint_opt8_t serial_data[2] = { command::BAUD, baud_code_ };
	//TODO: Incorporate state machine - if ( OFF == _oi_mode ) { return OI_NOT_STARTED; }
	if ( baud_code_ > 11 ) { return INVALID_PARAMETER; }

	if ( !serial::multiByteSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
#ifdef SENSORS_ENABLED
	state::setBaudCode(baud_code_);
#endif

	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	return SUCCESS;
}

template<>
ReturnCode
OpenInterface<OI500>::safe (
	void
) {
	const uint_opt8_t serial_data[1] = { command::SAFE };
	//TODO: Incorporate state machine - if ( OFF == _oi_mode ) { return OI_NOT_STARTED; }
	
	if ( !serial::multiByteSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	state::setOIMode(SAFE);
	
	return SUCCESS;
}

template<>
ReturnCode
OpenInterface<OI500>::control (
	void
) {
	return safe();
}

template<>
ReturnCode
OpenInterface<OI500>::full (
	void
) {
	const uint_opt8_t serial_data[1] = { command::FULL };
	//TODO: Incorporate state machine - if ( OFF == _oi_mode ) { return OI_NOT_STARTED; }
	
	if ( !serial::multiByteSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	state::setOIMode(FULL);
	
	return SUCCESS;
}

template<>
ReturnCode
OpenInterface<OI500>::clean (
	void
) {
	const uint_opt8_t serial_data[1] = { command::CLEAN };
	//TODO: Incorporate state machine - if ( OFF == _oi_mode ) { return OI_NOT_STARTED; }
	
	if ( !serial::multiByteSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	state::setOIMode(PASSIVE);
	
	return SUCCESS;
}

template<>
ReturnCode
OpenInterface<OI500>::max (
	void
) {
	const uint_opt8_t serial_data[1] = { command::MAX };
	//TODO: Incorporate state machine - if ( OFF == _oi_mode ) { return OI_NOT_STARTED; }
	
	if ( !serial::multiByteSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	state::setOIMode(PASSIVE);
	
	return SUCCESS;
}

template<>
ReturnCode
OpenInterface<OI500>::spot (
	void
) {
	const uint_opt8_t serial_data[1] = { command::SPOT };
	//TODO: Incorporate state machine - if ( OFF == _oi_mode ) { return OI_NOT_STARTED; }
	
	if ( !serial::multiByteSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	state::setOIMode(PASSIVE);
	
	return SUCCESS;
}

template<>
ReturnCode
OpenInterface<OI500>::seekDock (
	void
) {
	const uint_opt8_t serial_data[1] = { command::SEEK_DOCK };
	//TODO: Incorporate state machine - if ( OFF == _oi_mode ) { return OI_NOT_STARTED; }
	
	if ( !serial::multiByteSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	state::setOIMode(PASSIVE);
	
	return SUCCESS;
}

template<>
ReturnCode
OpenInterface<OI500>::schedule (
	const bitmask::Days day_mask_,
	const clock_time_t * const clock_times_
) {
	uint_opt8_t serial_data[16] = { command::SCHEDULE };
	//TODO: Incorporate state machine - if ( OFF == _oi_mode ) { return OI_NOT_STARTED; }
	
	if ( day_mask_ && clock_times_ ) {
		serial_data[1] = static_cast<bitmask::Days>(day_mask_ & 0x7F);
		for (uint_opt8_t day = 0, parameter_index = 0, serial_index = 2 ; day < 7 ; ++day, parameter_index += ((day_mask_ >> day) & 1), serial_index = ((2 * day) + 2)) {
			// Test conditions without branching logic to allow for code pipelining and parallel execution with loop unroll
			const bool valid = (((day_mask_ >> day) & 1) && (clock_times_[parameter_index].hour >= 0 && clock_times_[parameter_index].hour < 23) && (clock_times_[parameter_index].minute >= 0 && clock_times_[parameter_index].minute <= 59));
			*reinterpret_cast<uint_opt16_t *>(&serial_data[serial_index]) = (valid * (*reinterpret_cast<const uint_opt16_t *>(&clock_times_[parameter_index])));
			serial_data[1] &= ~(!valid << day);
		}
	}
	
	if ( !serial::multiByteSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}

template<>
ReturnCode
OpenInterface<OI500>::setDayTime (
	const Day day_,
	const clock_time_t clock_time_
) {
	const uint_opt8_t serial_data[4] = { command::SET_DAY_TIME, day_, clock_time_.hour, clock_time_.minute };
	//TODO: Incorporate state machine - if ( OFF == _oi_mode ) { return OI_NOT_STARTED; }
	if ( clock_time_.hour < 0 || clock_time_.hour > 23 || clock_time_.minute < 0 || clock_time_.minute > 59 ) { return INVALID_PARAMETER; }
	
	if ( !serial::multiByteSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}

template<>
ReturnCode
OpenInterface<OI500>::power (
	void
) {
	const uint_opt8_t serial_data[1] = { command::POWER };
	//TODO: Incorporate state machine - if ( OFF == _oi_mode ) { return OI_NOT_STARTED; }
	
	if ( !serial::multiByteSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	state::setOIMode(PASSIVE);
	
	return SUCCESS;
}

template<>
ReturnCode
OpenInterface<OI500>::drive (
	const int_opt16_t velocity_,
	const int_opt16_t radius_
) {
	const uint_opt8_t serial_data[5] = { command::DRIVE, reinterpret_cast<const uint_opt8_t *>(&velocity_)[1], reinterpret_cast<const uint_opt8_t *>(&velocity_)[0], reinterpret_cast<const uint_opt8_t *>(&radius_)[1], reinterpret_cast<const uint_opt8_t *>(&radius_)[0] };
	//TODO: Incorporate state machine - if ( OFF == _oi_mode ) { return OI_NOT_STARTED; }
	//TODO: Incorporate state machine - if ( PASSIVE == _oi_mode ) { return INVALID_MODE_FOR_REQUESTED_OPERATION; }
	if ( velocity_ < -500 || velocity_ > 500 || (radius_ != 32767 && (radius_ < -2000 || radius_ > 2000)) ) { return INVALID_PARAMETER; }
	
	if ( !serial::multiByteSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}

template<>
ReturnCode
OpenInterface<OI500>::driveDirect (
	const int_opt16_t left_wheel_velocity_,
	const int_opt16_t right_wheel_velocity_
) {
	const uint_opt8_t serial_data[5] = { command::DRIVE_DIRECT, reinterpret_cast<const uint_opt8_t *>(&right_wheel_velocity_)[1], reinterpret_cast<const uint_opt8_t *>(&right_wheel_velocity_)[0], reinterpret_cast<const uint_opt8_t *>(&left_wheel_velocity_)[1], reinterpret_cast<const uint_opt8_t *>(&left_wheel_velocity_)[0] };
	//TODO: Incorporate state machine - if ( OFF == _oi_mode ) { return OI_NOT_STARTED; }
	//TODO: Incorporate state machine - if ( PASSIVE == _oi_mode ) { return INVALID_MODE_FOR_REQUESTED_OPERATION; }
	if ( left_wheel_velocity_ < -500 || left_wheel_velocity_ > 500 || right_wheel_velocity_ < -500 || right_wheel_velocity_ > 500 ) { return INVALID_PARAMETER; }
	
	if ( !serial::multiByteSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}

template<>
ReturnCode
OpenInterface<OI500>::drivePWM (
	const int_opt16_t left_wheel_pwm_,
	const int_opt16_t right_wheel_pwm_
) {
	const uint_opt8_t serial_data[5] = { command::DRIVE_PWM, reinterpret_cast<const uint_opt8_t *>(&right_wheel_pwm_)[1], reinterpret_cast<const uint_opt8_t *>(&right_wheel_pwm_)[0], reinterpret_cast<const uint_opt8_t *>(&left_wheel_pwm_)[1], reinterpret_cast<const uint_opt8_t *>(&left_wheel_pwm_)[0] };
	//TODO: Incorporate state machine - if ( OFF == _oi_mode ) { return OI_NOT_STARTED; }
	//TODO: Incorporate state machine - if ( PASSIVE == _oi_mode ) { return INVALID_MODE_FOR_REQUESTED_OPERATION; }
	if ( left_wheel_pwm_ < -255 || left_wheel_pwm_ > 255 || right_wheel_pwm_ < -255 || right_wheel_pwm_ > 255 ) { return INVALID_PARAMETER; }
	
	if ( !serial::multiByteSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}

template<>
ReturnCode
OpenInterface<OI500>::motors (
	const bitmask::MotorStates motor_state_mask_
) {
	const uint_opt8_t serial_data[2] = { command::MOTORS, static_cast<const uint_opt8_t>(motor_state_mask_ & 0x1F) };
	//TODO: Incorporate state machine - if ( OFF == _oi_mode ) { return OI_NOT_STARTED; }
	//TODO: Incorporate state machine - if ( PASSIVE == _oi_mode ) { return INVALID_MODE_FOR_REQUESTED_OPERATION; }
	
	if ( !serial::multiByteSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}

template<>
ReturnCode
OpenInterface<OI500>::pwmMotors (
	const int_opt8_t main_brush_,
	const int_opt8_t side_brush_,
	const int_opt8_t vacuum_
) {
	const uint_opt8_t serial_data[4] = { command::PWM_MOTORS, static_cast<const uint_opt8_t>(main_brush_), static_cast<const uint_opt8_t>(side_brush_), static_cast<const uint_opt8_t>(vacuum_) };
	//TODO: Incorporate state machine - if ( OFF == _oi_mode ) { return OI_NOT_STARTED; }
	//TODO: Incorporate state machine - if ( PASSIVE == _oi_mode ) { return INVALID_MODE_FOR_REQUESTED_OPERATION; }
	if ( -128 == main_brush_ || -128 == side_brush_ || vacuum_ < 0 ) { return INVALID_PARAMETER; }
	
	if ( !serial::multiByteSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}

template<>
ReturnCode
OpenInterface<OI500>::leds (
	const bitmask::display::LEDs led_mask_,
	const uint_opt8_t color_,
	const uint_opt8_t intensity_
) {
	const uint_opt8_t serial_data[4] = { command::LEDS, static_cast<const bitmask::display::LEDs>(led_mask_ & 0x0F), color_, intensity_ };
	//TODO: Incorporate state machine - if ( OFF == _oi_mode ) { return OI_NOT_STARTED; }
	//TODO: Incorporate state machine - if ( PASSIVE == _oi_mode ) { return INVALID_MODE_FOR_REQUESTED_OPERATION; }
	
	if ( !serial::multiByteSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}

template<>
ReturnCode
OpenInterface<OI500>::schedulingLEDs (
	const bitmask::Days day_mask_,
	const bitmask::display::SchedulingLEDs display_mask_
) {
	const uint_opt8_t serial_data[3] = { command::SCHEDULING_LEDS, static_cast<const bitmask::Days>(day_mask_ & 0x7F), static_cast<const bitmask::display::SchedulingLEDs>(display_mask_ & 0x1F) };
	//TODO: Incorporate state machine - if ( OFF == _oi_mode ) { return OI_NOT_STARTED; }
	//TODO: Incorporate state machine - if ( PASSIVE == _oi_mode ) { return INVALID_MODE_FOR_REQUESTED_OPERATION; }
	
	if ( !serial::multiByteSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}

template<>
ReturnCode
OpenInterface<OI500>::digitLEDsRaw (
	const bitmask::display::DigitN raw_leds_[4]
) {
	const uint_opt8_t serial_data[5] = { command::DIGIT_LEDS_RAW, static_cast<const bitmask::display::DigitN>(raw_leds_[0] & 0x7F), static_cast<const bitmask::display::DigitN>(raw_leds_[1] & 0x7F), static_cast<const bitmask::display::DigitN>(raw_leds_[2] & 0x7F), static_cast<const bitmask::display::DigitN>(raw_leds_[3] & 0x7F) };
	//TODO: Incorporate state machine - if ( OFF == _oi_mode ) { return OI_NOT_STARTED; }
	//TODO: Incorporate state machine - if ( PASSIVE == _oi_mode ) { return INVALID_MODE_FOR_REQUESTED_OPERATION; }
	
	if ( !serial::multiByteSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}

template<>
ReturnCode
OpenInterface<OI500>::digitLEDsASCII (
	const char ascii_leds_[4]
) {
	const uint_opt8_t serial_data[5] = { command::DIGIT_LEDS_ASCII, static_cast<const uint_opt8_t>(ascii_leds_[0]), static_cast<const uint_opt8_t>(ascii_leds_[1]), static_cast<const uint_opt8_t>(ascii_leds_[2]), static_cast<const uint_opt8_t>(ascii_leds_[3]) };
	//TODO: Incorporate state machine - if ( OFF == _oi_mode ) { return OI_NOT_STARTED; }
	//TODO: Incorporate state machine - if ( PASSIVE == _oi_mode ) { return INVALID_MODE_FOR_REQUESTED_OPERATION; }
	if ( ascii_leds_[0] < 32 || ascii_leds_[0] > 126 || ascii_leds_[1] < 32 || ascii_leds_[1] > 126 || ascii_leds_[2] < 32 || ascii_leds_[2] > 126 || ascii_leds_[3] < 32 || ascii_leds_[3] > 126 ) { return INVALID_PARAMETER; }
	
	if ( !serial::multiByteSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}

template<>
ReturnCode
OpenInterface<OI500>::buttons (
	const bitmask::Buttons button_mask_
) {
	const uint_opt8_t serial_data[2] = { command::BUTTONS, button_mask_ };
	//TODO: Incorporate state machine - if ( OFF == _oi_mode ) { return OI_NOT_STARTED; }
	
	if ( !serial::multiByteSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}

template<>
ReturnCode
OpenInterface<OI500>::song (
	const uint_opt8_t song_number_,
	const std::vector<note_t> & song_
) {
	const uint_opt8_t note_count = song_.size();
	uint_opt8_t serial_data[(3 + (note_count * 2))];
	uint_opt8_t data_index = 2;
	//TODO: Incorporate state machine - if ( OFF == _oi_mode ) { return OI_NOT_STARTED; }
	if ( song_number_ > 4 || !note_count || note_count > 16 ) { return INVALID_PARAMETER; }
	
	serial_data[0] = command::SONG;
	serial_data[1] = song_number_;
	serial_data[2] = note_count;
	
	for (auto &note : song_) {
		serial_data[++data_index] = note.first;
		serial_data[++data_index] = note.second;
	}
	
	if ( !serial::multiByteSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}

template<>
ReturnCode
OpenInterface<OI500>::play (
	const uint_opt8_t song_number_
) {
	const uint_opt8_t serial_data[2] = { command::PLAY, song_number_ };
	//TODO: Incorporate state machine - if ( OFF == _oi_mode ) { return OI_NOT_STARTED; }
	//TODO: Incorporate state machine - if ( PASSIVE == _oi_mode ) { return INVALID_MODE_FOR_REQUESTED_OPERATION; }
	if ( song_number_ > 4 ) { return INVALID_PARAMETER; }
	
	if ( !serial::multiByteSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}

template<>
ReturnCode
OpenInterface<OI500>::sensors (
	const sensor::PacketId packet_id_
) {
	const uint_opt8_t serial_data[2] = { command::SENSORS, packet_id_ };
	// Ensure this is called after serial::multiByteSerialWrite() and SUCCESS is returned
	//const uint_opt8_t parse_key[2] = { sizeof(parse_key), packet_id_ };
	//state::setParseKey(parse_key);
	//TODO: Incorporate state machine - if ( OFF == _oi_mode ) { return OI_NOT_STARTED; }
	if ( (packet_id_ > 58 && packet_id_ < 100) || packet_id_ > 107 ) { return INVALID_PARAMETER; }
	
	if ( !serial::multiByteSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}

template<>
ReturnCode
OpenInterface<OI500>::pollSensors (
	const command::OpCode opcode_,
	const std::vector<sensor::PacketId> & sensor_list_
) {
	const uint_opt8_t byte_length = sensor_list_.size();
	uint_opt8_t serial_data[(2 + byte_length)];
	uint_opt8_t data_index = 1;
	//TODO: Incorporate state machine - if ( OFF == _oi_mode ) { return OI_NOT_STARTED; }
	if ( command::QUERY_LIST != opcode_ && command::STREAM != opcode_ ) { return INVALID_PARAMETER; }
	
	serial_data[0] = opcode_;
	serial_data[1] = byte_length;
	
	for (auto &sensor : sensor_list_) {
		if ( (sensor > 58 && sensor < 100) || sensor > 107 ) { continue; }
		serial_data[++data_index] = sensor;
	}
	if ( 1 == data_index ) { return INVALID_PARAMETER; }
	
	if ( !serial::multiByteSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}

template<>
ReturnCode
OpenInterface<OI500>::queryList (
	const std::vector<sensor::PacketId> & sensor_list_
) {
	return OpenInterface<OI500>::pollSensors(command::QUERY_LIST, sensor_list_);
}

template<>
ReturnCode
OpenInterface<OI500>::stream (
	const std::vector<sensor::PacketId> & sensor_list_
) {
	return OpenInterface<OI500>::pollSensors(command::STREAM, sensor_list_);
}

template<>
ReturnCode
OpenInterface<OI500>::pauseResumeStream (
	const bool resume_
) {
	const uint_opt8_t serial_data[2] = { command::PAUSE_RESUME_STREAM, resume_ };
	//TODO: Incorporate state machine - if ( OFF == _oi_mode ) { return OI_NOT_STARTED; }

	if ( !serial::multiByteSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}

} // namespace roomba

/* Created and copyrighted by Zachary J. Fields. All rights reserved. */
