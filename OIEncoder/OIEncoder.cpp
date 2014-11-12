/* Created and copyrighted by Zachary J. Fields. All rights reserved. */

#include "OIEncoder.h"

#include <chrono>
#include <thread>

namespace roomba {
namespace series500 {
namespace oi {

OIEncoder::OIEncoder (
	void
) :
	_fnSerialWrite([](const uint8_t *, const size_t){ return 0; }),
	_oi_mode(OFF),
	_baud_code(BAUD_115200)
{}

OIEncoder::ReturnCode
OIEncoder::operator() (
	const std::vector<uint8_t> & raw_instructions_,
	const OIMode resulting_mode_,
	const BaudCode resulting_baud_
) {
	if ( !_fnSerialWrite(raw_instructions_.data(), raw_instructions_.size()) ) { return SERIAL_TRANSFER_FAILURE; }
	if ( 0xFF != resulting_mode_ ) { _oi_mode = resulting_mode_; }
	if ( 0xFF != resulting_baud_ ) { _baud_code = resulting_baud_; }
	
	return SUCCESS;
}

OIEncoder::ReturnCode
OIEncoder::connectToSerialBus (
	const std::function<size_t(const uint8_t *, const size_t)> fnSerialWrite_,
	const BaudCode baud_code_
) {
	if ( BAUD_115200 != baud_code_ && BAUD_19200 != baud_code_ ) { return INVALID_PARAMETER; }
	
	_fnSerialWrite = fnSerialWrite_;
	_baud_code = baud_code_;
	
	return SUCCESS;
}

OIEncoder::ReturnCode
OIEncoder::start (
	void
) {
	const uint8_t serial_data[1] = { command::START };
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	_oi_mode = PASSIVE;
	
	return SUCCESS;
}

OIEncoder::ReturnCode
OIEncoder::baud (
	const BaudCode baud_code_
) {
	const uint8_t serial_data[2] = { command::BAUD, baud_code_ };
	if ( OFF == _oi_mode ) { return OI_NOT_STARTED; }
	if ( baud_code_ > 11 ) { return INVALID_PARAMETER; }

	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	_baud_code = baud_code_;
	
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	return SUCCESS;
}

OIEncoder::ReturnCode
OIEncoder::safe (
	void
) {
	const uint8_t serial_data[1] = { command::SAFE };
	if ( OFF == _oi_mode ) { return OI_NOT_STARTED; }
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	_oi_mode = SAFE;
	
	return SUCCESS;
}

OIEncoder::ReturnCode
OIEncoder::control (
	void
) {
	return safe();
}

OIEncoder::ReturnCode
OIEncoder::full (
	void
) {
	const uint8_t serial_data[1] = { command::FULL };
	if ( OFF == _oi_mode ) { return OI_NOT_STARTED; }
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	_oi_mode = FULL;
	
	return SUCCESS;
}

OIEncoder::ReturnCode
OIEncoder::clean (
	void
) {
	const uint8_t serial_data[1] = { command::CLEAN };
	if ( OFF == _oi_mode ) { return OI_NOT_STARTED; }
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	_oi_mode = PASSIVE;
	
	return SUCCESS;
}

OIEncoder::ReturnCode
OIEncoder::max (
	void
) {
	const uint8_t serial_data[1] = { command::MAX };
	if ( OFF == _oi_mode ) { return OI_NOT_STARTED; }
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	_oi_mode = PASSIVE;
	
	return SUCCESS;
}

OIEncoder::ReturnCode
OIEncoder::spot (
	void
) {
	const uint8_t serial_data[1] = { command::SPOT };
	if ( OFF == _oi_mode ) { return OI_NOT_STARTED; }
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	_oi_mode = PASSIVE;
	
	return SUCCESS;
}

OIEncoder::ReturnCode
OIEncoder::seekDock (
	void
) {
	const uint8_t serial_data[1] = { command::SEEK_DOCK };
	if ( OFF == _oi_mode ) { return OI_NOT_STARTED; }
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	_oi_mode = PASSIVE;
	
	return SUCCESS;
}

OIEncoder::ReturnCode
OIEncoder::schedule (
	const bitmask::Days day_mask_,
	const clock_time_t * const clock_times_
) const {
	uint8_t serial_data[16] = { command::SCHEDULE };
	if ( OFF == _oi_mode ) { return OI_NOT_STARTED; }
	
	if ( day_mask_ && clock_times_ ) {
		serial_data[1] = static_cast<bitmask::Days>(day_mask_ & 0x7F);
		for (uint8_t day = 0, parameter_index = 0, serial_index = 2 ; day < 7 ; ++day, parameter_index += ((day_mask_ >> day) & 1), serial_index = ((2 * day) + 2)) {
			// Test conditions without branching logic to allow for code pipelining and parallel execution with loop unroll
			const bool valid = (((day_mask_ >> day) & 1) && (clock_times_[parameter_index].hour >= 0 && clock_times_[parameter_index].hour < 23) && (clock_times_[parameter_index].minute >= 0 && clock_times_[parameter_index].minute <= 59));
			*reinterpret_cast<uint16_t *>(&serial_data[serial_index]) = (valid * (*reinterpret_cast<const uint16_t *>(&clock_times_[parameter_index])));
			serial_data[1] &= ~(!valid << day);
		}
	}
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}

OIEncoder::ReturnCode
OIEncoder::setDayTime (
	const Day day_,
	const clock_time_t clock_time_
) const {
	const uint8_t serial_data[4] = { command::SET_DAY_TIME, day_, clock_time_.hour, clock_time_.minute };
	if ( OFF == _oi_mode ) { return OI_NOT_STARTED; }
	if ( clock_time_.hour < 0 || clock_time_.hour > 23 || clock_time_.minute < 0 || clock_time_.minute > 59 ) { return INVALID_PARAMETER; }
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}

OIEncoder::ReturnCode
OIEncoder::power (
	void
) {
	const uint8_t serial_data[1] = { command::POWER };
	if ( OFF == _oi_mode ) { return OI_NOT_STARTED; }
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	_oi_mode = PASSIVE;
	
	return SUCCESS;
}

OIEncoder::ReturnCode
OIEncoder::drive (
	const int16_t velocity_,
	const int16_t radius_
) const {
	const uint8_t serial_data[5] = { command::DRIVE, reinterpret_cast<const uint8_t *>(&velocity_)[1], reinterpret_cast<const uint8_t *>(&velocity_)[0], reinterpret_cast<const uint8_t *>(&radius_)[1], reinterpret_cast<const uint8_t *>(&radius_)[0] };
	if ( OFF == _oi_mode ) { return OI_NOT_STARTED; }
	if ( PASSIVE == _oi_mode ) { return INVALID_MODE_FOR_REQUESTED_OPERATION; }
	if ( velocity_ < -500 || velocity_ > 500 || (radius_ != 32767 && (radius_ < -2000 || radius_ > 2000)) ) { return INVALID_PARAMETER; }
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}

OIEncoder::ReturnCode
OIEncoder::driveDirect (
	const int16_t left_wheel_velocity_,
	const int16_t right_wheel_velocity_
) const {
	const uint8_t serial_data[5] = { command::DRIVE_DIRECT, reinterpret_cast<const uint8_t *>(&right_wheel_velocity_)[1], reinterpret_cast<const uint8_t *>(&right_wheel_velocity_)[0], reinterpret_cast<const uint8_t *>(&left_wheel_velocity_)[1], reinterpret_cast<const uint8_t *>(&left_wheel_velocity_)[0] };
	if ( OFF == _oi_mode ) { return OI_NOT_STARTED; }
	if ( PASSIVE == _oi_mode ) { return INVALID_MODE_FOR_REQUESTED_OPERATION; }
	if ( left_wheel_velocity_ < -500 || left_wheel_velocity_ > 500 || right_wheel_velocity_ < -500 || right_wheel_velocity_ > 500 ) { return INVALID_PARAMETER; }
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}

OIEncoder::ReturnCode
OIEncoder::drivePWM (
	const int16_t left_wheel_pwm_,
	const int16_t right_wheel_pwm_
) const {
	const uint8_t serial_data[5] = { command::DRIVE_PWM, reinterpret_cast<const uint8_t *>(&right_wheel_pwm_)[1], reinterpret_cast<const uint8_t *>(&right_wheel_pwm_)[0], reinterpret_cast<const uint8_t *>(&left_wheel_pwm_)[1], reinterpret_cast<const uint8_t *>(&left_wheel_pwm_)[0] };
	if ( OFF == _oi_mode ) { return OI_NOT_STARTED; }
	if ( PASSIVE == _oi_mode ) { return INVALID_MODE_FOR_REQUESTED_OPERATION; }
	if ( left_wheel_pwm_ < -255 || left_wheel_pwm_ > 255 || right_wheel_pwm_ < -255 || right_wheel_pwm_ > 255 ) { return INVALID_PARAMETER; }
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}

OIEncoder::ReturnCode
OIEncoder::motors (
	const bitmask::MotorStates motor_state_mask_
) const {
	const uint8_t serial_data[2] = { command::MOTORS, static_cast<const uint8_t>(motor_state_mask_ & 0x1F) };
	if ( OFF == _oi_mode ) { return OI_NOT_STARTED; }
	if ( PASSIVE == _oi_mode ) { return INVALID_MODE_FOR_REQUESTED_OPERATION; }
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}

OIEncoder::ReturnCode
OIEncoder::pwmMotors (
	const int8_t main_brush_,
	const int8_t side_brush_,
	const int8_t vacuum_
) const {
	const uint8_t serial_data[4] = { command::PWM_MOTORS, static_cast<const uint8_t>(main_brush_), static_cast<const uint8_t>(side_brush_), static_cast<const uint8_t>(vacuum_) };
	if ( OFF == _oi_mode ) { return OI_NOT_STARTED; }
	if ( PASSIVE == _oi_mode ) { return INVALID_MODE_FOR_REQUESTED_OPERATION; }
	if ( -128 == main_brush_ || -128 == side_brush_ || vacuum_ < 0 ) { return INVALID_PARAMETER; }
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}

OIEncoder::ReturnCode
OIEncoder::leds (
	const bitmask::display::LEDs led_mask_,
	const uint8_t color_,
	const uint8_t intensity_
) const {
	const uint8_t serial_data[4] = { command::LEDS, static_cast<const bitmask::display::LEDs>(led_mask_ & 0x0F), color_, intensity_ };
	if ( OFF == _oi_mode ) { return OI_NOT_STARTED; }
	if ( PASSIVE == _oi_mode ) { return INVALID_MODE_FOR_REQUESTED_OPERATION; }
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}

OIEncoder::ReturnCode
OIEncoder::schedulingLEDs (
	const bitmask::Days day_mask_,
	const bitmask::display::SchedulingLEDs display_mask_
) const {
	const uint8_t serial_data[3] = { command::SCHEDULING_LEDS, static_cast<const bitmask::Days>(day_mask_ & 0x7F), static_cast<const bitmask::display::SchedulingLEDs>(display_mask_ & 0x1F) };
	if ( OFF == _oi_mode ) { return OI_NOT_STARTED; }
	if ( PASSIVE == _oi_mode ) { return INVALID_MODE_FOR_REQUESTED_OPERATION; }
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}

OIEncoder::ReturnCode
OIEncoder::digitLEDsRaw (
	const bitmask::display::DigitN raw_leds_[4]
) const {
	const uint8_t serial_data[5] = { command::DIGIT_LEDS_RAW, static_cast<const bitmask::display::DigitN>(raw_leds_[0] & 0x7F), static_cast<const bitmask::display::DigitN>(raw_leds_[1] & 0x7F), static_cast<const bitmask::display::DigitN>(raw_leds_[2] & 0x7F), static_cast<const bitmask::display::DigitN>(raw_leds_[3] & 0x7F) };
	if ( OFF == _oi_mode ) { return OI_NOT_STARTED; }
	if ( PASSIVE == _oi_mode ) { return INVALID_MODE_FOR_REQUESTED_OPERATION; }
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}

OIEncoder::ReturnCode
OIEncoder::digitLEDsASCII (
	const char ascii_leds_[4]
) const {
	const uint8_t serial_data[5] = { command::DIGIT_LEDS_ASCII, static_cast<const uint8_t>(ascii_leds_[0]), static_cast<const uint8_t>(ascii_leds_[1]), static_cast<const uint8_t>(ascii_leds_[2]), static_cast<const uint8_t>(ascii_leds_[3]) };
	if ( OFF == _oi_mode ) { return OI_NOT_STARTED; }
	if ( PASSIVE == _oi_mode ) { return INVALID_MODE_FOR_REQUESTED_OPERATION; }
	if ( ascii_leds_[0] < 32 || ascii_leds_[0] > 126 || ascii_leds_[1] < 32 || ascii_leds_[1] > 126 || ascii_leds_[2] < 32 || ascii_leds_[2] > 126 || ascii_leds_[3] < 32 || ascii_leds_[3] > 126 ) { return INVALID_PARAMETER; }
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}

OIEncoder::ReturnCode
OIEncoder::buttons (
	const bitmask::Buttons button_mask_
) const {
	const uint8_t serial_data[2] = { command::BUTTONS, button_mask_ };
	if ( OFF == _oi_mode ) { return OI_NOT_STARTED; }
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}

OIEncoder::ReturnCode
OIEncoder::song (
	const uint8_t song_number_,
	const std::vector<note_t> & song_
) const {
	const uint8_t note_count = song_.size();
	uint8_t serial_data[(3 + (note_count * 2))];
	uint8_t data_index = 2;
	if ( OFF == _oi_mode ) { return OI_NOT_STARTED; }
	if ( song_number_ > 4 || !note_count || note_count > 16 ) { return INVALID_PARAMETER; }
	
	serial_data[0] = command::SONG;
	serial_data[1] = song_number_;
	serial_data[2] = note_count;
	
	for (auto &note : song_) {
		serial_data[++data_index] = note.first;
		serial_data[++data_index] = note.second;
	}
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}

OIEncoder::ReturnCode
OIEncoder::play (
	const uint8_t song_number_
) const {
	const uint8_t serial_data[2] = { command::PLAY, song_number_ };
	if ( OFF == _oi_mode ) { return OI_NOT_STARTED; }
	if ( PASSIVE == _oi_mode ) { return INVALID_MODE_FOR_REQUESTED_OPERATION; }
	if ( song_number_ > 4 ) { return INVALID_PARAMETER; }
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}
#ifdef SENSORS_ENABLED
OIEncoder::ReturnCode
OIEncoder::sensors (
	const sensors::PacketId packet_id_
) const {
	const uint8_t serial_data[2] = { command::SENSORS, packet_id_ };
	if ( OFF == _oi_mode ) { return OI_NOT_STARTED; }
	if ( (packet_id_ > 58 && packet_id_ < 100) || packet_id_ > 107 ) { return INVALID_PARAMETER; }
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}

OIEncoder::ReturnCode
OIEncoder::queryList (
	const std::vector<sensors::PacketId> & sensor_list_
) const {
	return pollSensors(command::QUERY_LIST, sensor_list_);
}

OIEncoder::ReturnCode
OIEncoder::stream (
	const std::vector<sensors::PacketId> & sensor_list_
) const {
	return pollSensors(command::STREAM, sensor_list_);
}

OIEncoder::ReturnCode
OIEncoder::pauseResumeStream (
	const bool resume_
) const {
	const uint8_t serial_data[2] = { command::PAUSE_RESUME_STREAM, resume_ };
	if ( OFF == _oi_mode ) { return OI_NOT_STARTED; }

	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}

OIEncoder::ReturnCode
OIEncoder::pollSensors (
	const command::OpCode opcode_,
	const std::vector<sensors::PacketId> & sensor_list_
) const {
	const uint8_t byte_length = sensor_list_.size();
	uint8_t serial_data[(2 + byte_length)];
	uint8_t data_index = 1;
	if ( OFF == _oi_mode ) { return OI_NOT_STARTED; }
	if ( command::QUERY_LIST != opcode_ && command::STREAM != opcode_ ) { return INVALID_PARAMETER; }
	
	serial_data[0] = opcode_;
	serial_data[1] = byte_length;
	
	for (auto &sensor : sensor_list_) {
		if ( (sensor > 58 && sensor < 100) || sensor > 107 ) { continue; }
		serial_data[++data_index] = sensor;
	}
	if ( 1 == data_index ) { return INVALID_PARAMETER; }
	
	if ( !_fnSerialWrite(serial_data, sizeof(serial_data)) ) { return SERIAL_TRANSFER_FAILURE; }
	
	return SUCCESS;
}
#endif
} // namespace oi
} // namespace series500
} // namespace roomba

roomba::series500::oi::OIEncoder Encoder;

/* Created and copyrighted by Zachary J. Fields. All rights reserved. */
