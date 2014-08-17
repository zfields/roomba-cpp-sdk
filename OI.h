/* Created and copyrighted by Zachary J. Fields. All rights reserved. */

#ifndef OI_H
#define OI_H

#include <cstdint>
#include <functional>
#include <utility>
#include <vector>

#include "OINames.h"

namespace roomba {
namespace series500 {

/// \brief The Roomba Open Interface (OI) class
/// \details The Roomba Open Interface (OI) is a software
/// interface for controlling and manipulating Roomba’s
/// behavior. The software interface lets you manipulate
/// Roomba’s behavior and read its sensors through a series
/// of commands, including mode commands, actuator commands,
/// song commands, and sensor commands that you send to the
/// Roomba’s serial port by way of a PC or microcontroller
/// that is connected to the Mini-DIN connector.
class OpenInterface {
  public:
	/// \brief Return codes
	enum ReturnCode : int8_t {
		SERIAL_TRANSFER_FAILURE = -100,
		INVALID_PARAMETER = -10,
		INVALID_MODE_FOR_REQUESTED_OPERATION = -2,
		OI_NOT_STARTED = -1,
		SUCCESS = 0,
	};
	
	/// \brief Time representation for the scheduling methods
	/// \details This struct represents time in military time
	/// with the two fields hour and minute. The values in the
	/// structure are initialized to zero upon instantiation.
	typedef struct clock_time_t {
		clock_time_t (uint8_t hour_ = 0, uint8_t minute_ = 0) : hour(hour_), minute(minute_) {}
		uint8_t hour; ///< hour (0-23)
		uint8_t minute; ///< minute (0-59)
	} clock_time_t;
	
	OpenInterface (
		void
	);
	
	/// \brief Direct access to the Open Interface
	/// \details Direct access sends bytes directly to the Open Interface.
	/// Direct access is potentially dangerous, because the parameters are
	/// not checked and the device can be left in a "waiting" state.
	/// \param [in] opcode
	/// \param [in] [data] An optional list of parameters for the OpCode
	/// specified.
	/// \warning Use of direct access will invalidate the current state of
	/// the object, and will incur the overhead associated with polling
	/// the state of the device to restore the current state.
	void
	operator() (
		const command::OpCode opcode_,
		const std::vector<uint8_t> & data_
	);
	
	/// \brief Establishes a serial channel with the hardware.
	/// \details This method must be called before any other methods of
	/// this class. It establishes a serial channel between the Open
	/// Interface class and the underlying hardware. The default baud
	/// for communicating with the Roomba outside the Open Interface
	/// is 115200, if the external microcontroller is unable to
	/// communicate at that speed, an alternative baud, 19200,
	/// is available. To enable the slower baud on the Roomba you must
	/// power-on the Roomba by holding down the clean/power button,
	/// or the Roomba can be signaled on the baud rate change line.
	/// \param [in] fnSerialWrite A function that writes to the
	/// serial bus at either 115200 or 19200 baud.
	/// \warning If the baud rate of fnSerialWrite is not synchronized
	/// to the baud rate of the Roomba, then this class will be unable
	/// to communicate with the Roomba's Open Interface.
	void
	connectToSerialBus (
		const std::function<size_t(const uint8_t *, size_t)> fnSerialWrite_
	);
	
	/// \brief Releases control of the Roomba.
	/// \details This method with set the OI Mode to passive and return
	/// the Roomba to its docking station. It will also reinitialize the
	/// class member variables to restore it to a clean state.
	void
	end (
		void
	) const;
	
	/// \brief Starts the OI.
	/// \details You must always send the Start command before sending any
	/// other commands to the OI.
	/// \note Available in modes: Passive, Safe, or Full.
	/// \note Changes mode to: Passive. Roomba beeps once to acknowledge
	/// it is starting from “off” mode.
	/// \retval SUCCESS
	/// \retval SERIAL_TRANSFER_FAILURE
	ReturnCode
	start (
		void
	);
	
	/// \brief Sets the baud rate in bits per second (bps).
	/// \details This command sets the baud rate in bits per second (bps)
	/// at which OI commands and data are sent according to the baud code
	/// sent in the data byte.
	/// \param [in] baud_code
	/// \note The default baud rate at power up is 115200 bps.
	/// \note Available in modes: Passive, Safe, or Full.
	/// \retval SUCCESS
	/// \retval OI_NOT_STARTED
	/// \retval SERIAL_TRANSFER_FAILURE
	ReturnCode
	baud (
		const BaudCode baud_code_
	) const;
	
	/// \brief The effect and usage of the Control command are identical to
	/// the Safe command.
	/// \see OpenInterface::safe
	ReturnCode
	control (
		void
	);
	
	/// \brief Puts the OI into Safe mode.
	/// \details This command puts the OI into Safe mode, enabling user
	/// control of Roomba. It turns off all LEDs. The OI can be in Passive,
	/// Safe, or Full mode to accept this command.
	/// \note If a safety condition occurs (see above) Roomba reverts
	/// automatically to Passive mode.
	/// \note Available in modes: Passive, Safe, or Full.
	/// \note Changes mode to: Safe.
	/// \retval SUCCESS
	/// \retval OI_NOT_STARTED
	/// \retval SERIAL_TRANSFER_FAILURE
	ReturnCode
	safe (
		void
	);
	
	/// \brief Puts the OI into Full mode.
	/// \details This command gives you complete control over Roomba by
	/// putting the OI into Full mode, and turning off the cliff,
	/// wheel-drop and internal charger safety features.
	/// \note Available in modes: Passive, Safe, or Full.
	/// \note Changes mode to: Full.
	/// \retval SUCCESS
	/// \retval OI_NOT_STARTED
	/// \retval SERIAL_TRANSFER_FAILURE
	ReturnCode
	full (
		void
	);
	
	/// \brief Starts the default cleaning mode.
	/// \note Available in modes: Passive, Safe, or Full.
	/// \note Changes mode to: Passive.
	/// \retval SUCCESS
	/// \retval OI_NOT_STARTED
	/// \retval SERIAL_TRANSFER_FAILURE
	ReturnCode
	clean (
		void
	);
	
	/// \brief Starts the Max cleaning mode.
	/// \note Available in modes: Passive, Safe, or Full.
	/// \note Changes mode to: Passive.
	/// \retval SUCCESS
	/// \retval OI_NOT_STARTED
	/// \retval SERIAL_TRANSFER_FAILURE
	ReturnCode
	max (
		void
	);
	
	/// \brief Starts the Spot cleaning mode.
	/// \note Available in modes: Passive, Safe, or Full.
	/// \note Changes mode to: Passive.
	/// \retval SUCCESS
	/// \retval OI_NOT_STARTED
	/// \retval SERIAL_TRANSFER_FAILURE
	ReturnCode
	spot (
		void
	);
	
	/// \brief Sends Roomba to the dock.
	/// \note Available in modes: Passive, Safe, or Full.
	/// \note Changes mode to: Passive.
	/// \retval SUCCESS
	/// \retval OI_NOT_STARTED
	/// \retval SERIAL_TRANSFER_FAILURE
	ReturnCode
	seekDock (
		void
	);
	
	/// \brief Sends Roomba a new schedule.
	/// \details This command sends Roomba a new schedule. To disable
	/// scheduled cleaning, send all 0s.
	/// \param [in] day_mask A bitmask representing the days of the week
	/// \param [in] clock_times_ A sparse array of clock_time_t variables
	/// following { Sun time, Mon time, ..., Sat time }.
	/// \note The day_mask variable will determine which days the array
	/// elements will be applied to, disabled days will be zero filled.
	/// \note If a clock_time_t variable contains any values out of range,
	/// then the corresponding day will be ignored.
	/// \note If Roomba’s schedule or clock button is pressed, this
	/// command will be ignored.
	/// \note Available in modes: Passive, Safe, or Full.
	/// \retval SUCCESS
	/// \retval OI_NOT_STARTED
	/// \retval SERIAL_TRANSFER_FAILURE
	ReturnCode
	schedule (
		const bitmask::Days day_mask_,
		const clock_time_t * const clock_times_
	) const;
	
	/// \brief Sets Roomba’s clock.
	/// \param [in] day
	/// \param [in] clock time (hour [0-23] and minute [0-59])
	/// \note If Roomba’s schedule or clock button is pressed,
	/// this command will be ignored.
	/// \note Available in modes: Passive, Safe, or Full.
	/// \retval SUCCESS
	/// \retval OI_NOT_STARTED
	/// \retval INVALID_PARAMETER
	/// \retval SERIAL_TRANSFER_FAILURE
	ReturnCode
	setDayTime (
		const Day day_,
		const clock_time_t clock_time_
	) const;
	
	/// \brief Powers down Roomba.
	/// \details This command powers down Roomba. The OI can be in Passive,
	/// Safe, or Full mode to accept this command.
	/// \note Available in modes: Passive, Safe, or Full.
	/// \note Changes mode to: Passive.
	ReturnCode
	power (
		void
	) const;
	
	/// \brief Controls Roomba’s drive wheels.
	/// \details This command controls Roomba’s drive wheels.
	/// A Drive command with a positive velocity and a positive radius
	/// makes Roomba drive forward while turning toward the left. A
	/// negative radius makes Roomba turn toward the right. Special
	/// cases for the radius make Roomba turn in place or drive straight,
	/// as specified below. A negative velocity makes Roomba drive backward.
	/// \param [in] velocity (-500 – 500) The average velocity of the drive
	/// wheels in millimeters per second (mm/s).
	/// \param [in] radius (-2000 – 2000) The radius in millimeters at which
	/// Roomba will turn. The radius is measured from the center of the
	/// turning circle to the center of Roomba.
	/// \par Special cases
	/// \par
	/// Straight = 32768 (0x8000) or 32767 (0x7FFF)
	/// \par
	/// Turn in place clockwise = -1
	/// \par
	/// Turn in place counter-clockwise = 1
	/// \note Available in modes: Safe or Full.
	/// \warning Internal and environmental restrictions may prevent Roomba
	/// from accurately carrying out some drive commands.
	ReturnCode
	drive (
		const int16_t velocity_,
		const int16_t radius_
	) const;
	
	/// \brief Controls the forward and backward motion of Roomba’s drive
	/// wheels independently.
	/// \details This command lets you control the forward and backward
	/// motion of Roomba’s drive wheels independently. A positive velocity
	/// makes that wheel drive forward, while a negative velocity makes it
	/// drive backward.
	/// \param [in] right_wheel_velocity (-500 – 500) The velocity of the
	/// right wheel in millimeters per second (mm/s).
	/// \param [in] left_wheel_velocity (-500 – 500) The velocity of the
	/// left wheel in millimeters per second (mm/s).
	/// \note Available in modes: Safe or Full.
	ReturnCode
	driveDirect (
		const int16_t right_wheel_velocity_,
		const int16_t left_wheel_velocity_
	) const;
	
	/// \brief Controls the raw forward and backward motion of Roomba’s drive
	/// wheels independently.
	/// \details This command lets you control the raw forward and backward
	/// motion of Roomba’s drive wheels independently. A positive PWM makes
	/// that wheel drive forward, while a negative PWM makes it drive backward.
	/// \param [in] right_wheel_pwm (-255 – 255) The PWM of the right wheel.
	/// \param [in] left_wheel_pwm (-255 – 255) The PWM of the left wheel.
	/// \note Available in modes: Safe or Full.
	ReturnCode
	drivePWM (
		const int16_t right_wheel_pwm_,
		const int16_t left_wheel_pwm_
	) const;
	
	/// \brief Controls the forward and backward motion of Roomba’s main brush,
	/// side brush, and vacuum independently.
	/// \details This command lets you control the forward and backward motion
	/// of Roomba’s main brush, side brush, and vacuum independently. Motor
	/// velocity cannot be controlled with this command, all motors will run at
	/// maximum speed when enabled. The main brush and side brush can be run in
	/// either direction. The vacuum only runs forward.
	/// \param [in] motor_state_mask 
	/// \note Available in modes: Safe or Full.
	ReturnCode
	motors (
		const bitmask::MotorStates motor_state_mask_
	) const;
	
	/// \brief Controls the speed of Roomba’s main brush, side brush, and
	/// vacuum independently.
	/// \details This command lets you control the speed of Roomba’s main brush,
	/// side brush, and vacuum independently. With each parameter, you specify
	/// the duty cycle for the low side driver (max 127). Positive speeds turn
	/// the motor in its default (cleaning) direction. For example, if you want
	/// to control a motor with 25% of battery voltage, choose a duty cycle of
	/// 127 * 25% ~ 32.
	/// \param [in] main_brush (-127 – 127)
	/// \param [in] side_brush (-127 – 127)
	/// \param [in] vacuum (0 – 127)
	/// \note The vacuum only runs forward.
	/// \note The main brush and side brush can be run in either direction.
	/// \note Default direction for the side brush is counter-clockwise.
	/// \note Default direction for the main brush/flapper is inward.
	/// \note Available in modes: Safe or Full.
	ReturnCode
	pwmMotors (
		const int8_t main_brush_,
		const int8_t side_brush_,
		const int8_t vacuum_
	) const;
	
	/// \brief Controls the LEDs
	/// \details This command controls the LEDs common to all models of
	/// Roomba 500. Clean/Power uses a bicolor (red/green) LED. The intensity
	/// and color of this LED can be controlled with 8-bit resolution.
	/// \param [in] led_mask
	/// \param [in] color 0 = green, 255 = red. Intermediate values are
	/// intermediate colors (orange, yellow, etc).
	/// \param [in] intensity 0 = off, 255 = full intensity. Intermediate
	/// values are intermediate intensities.
	/// \note Home and Spot use green LEDs: 0 = off, 1 = on.
	/// \note Check Robot uses an orange LED.
	/// \note Debris uses a blue LED.
	/// \note Available in modes: Safe or Full.
	ReturnCode
	leds (
		const bitmask::display::LEDs led_mask_,
		const uint8_t color_,
		const uint8_t intensity_
	) const;
	
	/// \brief Controls the state of the scheduling LEDs present on the Roomba 560 and 570.
	/// \param [in] day_mask
	/// \param [in] led_mask
	/// \note All use red LEDs
	/// \note Available in modes: Safe or Full.
	ReturnCode
	schedulingLEDs (
		const bitmask::Days day_mask_,
		const bitmask::display::SchedulingLEDs led_mask_
	) const;
	
	/// \brief Controls the 7 segment displays.
	/// \details This command controls the four 7 segment displays on
	/// the Roomba 560 and 570.
	/// \param [in] raw_leds An array representing the four 7 segment display values.
	/// \note All use red LEDs
	/// \note Available in modes: Safe or Full.
	ReturnCode
	digitLEDsRaw (
		const bitmask::display::DigitN raw_leds_[4]
	) const;
	
	/// \brief Controls the 7 segment displays using ASCII character codes.
	/// \details This command controls the four 7 segment displays on
	/// the Roomba 560 and 570 using ASCII character codes. Because a
	/// 7 segment display is not sufficient to display alphabetic
	/// characters properly, all characters are an approximation
	/// \param [in] ascii_leds An ASCII char array (valid values range
	/// from (32-126).
	/// \note Available in modes: Safe or Full
	/// \warning Not all ASCII codes are implemented.
	ReturnCode
	digitLEDsASCII (
		const char ascii_leds_[4]
	) const;
	
	/// \brief Push Roomba’s buttons.
	/// \details This command lets you push Roomba’s buttons. The buttons
	/// will automatically release after 1/6th of a second.
	/// \param [in] button_mask
	/// \note Available in modes: Passive, Safe, or Full.
	ReturnCode
	buttons (
		const bitmask::Buttons button_mask_
	) const;
	
	/// \brief Specify songs to be played at a later time.
	/// \details This command lets you specify up to four
	/// songs to the OI that you can play at a later time.
	/// Each song is associated with a song number. The Play
	/// command uses the song number to identify your song
	/// selection. Each song can contain up to sixteen notes.
	/// Each note is associated with a note number that uses
	/// MIDI note definitions and a duration that is specified
	/// in increments of 1/64th of a second.
	/// \param [in] song_number (0-4) The song number
	/// associated with the specific song.
	/// \param [in] song A vector of std::pairs composed
	/// of Note and duration.
	/// \note If you send a second Song command, using the
	/// same song number, the old song is overwritten.
	/// \note Available in modes: Passive, Safe, or Full.
	/// \see OpenInterface::play
	ReturnCode
	song (
		const uint8_t song_number_,
		const std::vector<std::pair<Note, uint8_t> > & notes_
	) const;
	
	/// \brief Select a song to play.
	/// \details This command lets you select a song to
	/// play from the songs added to Roomba using the Song
	/// command. You must add one or more songs to Roomba
	/// using the Song command in order for the Play command
	/// to work.
	/// \param [in] song_number (0-4) The number of the
	/// song Roomba is to play.
	/// \note Available in modes: Safe or Full
	/// \see OpenInterface::song
	ReturnCode
	play (
		const uint8_t song_number_
	) const;
	
	/// \brief Request sensor data.
	/// \details This command requests the OI to send a
	/// packet of sensor data bytes. There are 58 different
	/// sensor data packets. Each provides a value of a
	/// specific sensor or group of sensors.
	/// \param [in] packet_id Identifies which of the 58
	/// sensor data packets should be sent back by the OI.
	/// \note A value of 100 indicates a packet with all
	/// of the sensor data. Values of 0 through 6 and 101
	/// through 107 indicate specific subgroups of the sensor
	/// data.
	/// \note Available in modes: Passive, Safe, or Full.
	ReturnCode
	sensors (
		const sensor::PacketId packet_id_
	) const;
	
	/// \brief Request list of sensor packets
	/// \details This command lets you ask for a list of
	/// sensor packets. The result is returned once, as
	/// in the Sensors command. The robot returns the
	/// packets in the order you specify.
	/// \param packet_ids A vector of packet ids
	/// \note Available in modes: Passive, Safe, or Full.
	ReturnCode
	queryList (
		const std::vector<sensor::PacketId> & packet_ids_
	) const;
	
	/// \brief Start a data stream based on a query list.
	/// \details This command starts a stream of data packets.
	/// The list of packets requested is sent every 15 ms,
	/// which is the rate Roomba uses to update data.
	/// \param 
	/// \note This method of requesting sensor data is best
	/// if you are controlling Roomba over a wireless network
	/// (which has poor real-time characteristics) with
	/// software running on a desktop computer.
	/// \note Available in modes: Passive, Safe, or Full.
	ReturnCode
	stream (
		const std::vector<sensor::PacketId> & packet_ids_
	) const;
	
	/// \brief Stop and restart the stream.
	/// \details This command lets you stop and restart the
	/// steam without clearing the list of requested packets.
	/// \note Available in modes: Passive, Safe, or Full.
	ReturnCode
	pauseResumeStream (
		void
	) const;
	
  protected:
	std::function<size_t(const uint8_t *, size_t)> _fnSerialWrite;
	OIMode _mode;
};

} // namespace series500
} // namespace roomba

extern roomba::series500::OpenInterface OI;

#endif

/* Created and copyrighted by Zachary J. Fields. All rights reserved. */
