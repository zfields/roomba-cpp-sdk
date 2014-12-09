/* Created and copyrighted by Zachary J. Fields. All rights reserved. */

#ifndef OI_SENSORS_H
#define OI_SENSORS_H

#include <cstdint>
#include <functional>

#include "../OIDefines.h"

//TODO: Consider method to return multiple sensor values (std::tuple<uint_opt8_t packet_id_, uint16_t value_, bool signed_>)

namespace roomba {
namespace series500 {
namespace oi {

/// \brief Data type loaded by sensor methods
/// \details Sensor data is returned as a byte array
/// with checksum. This type reflects the data
/// representation of the returned sensor data,
/// as it strikes the balance between usability,
/// size and speed optimization.
/// see OICommand::sensors
/// see OICommand::queryList
/// see OICommand::stream
/// see OICommand::pauseResumeStream
namespace sensors {
	/// \brief Return codes
	enum ReturnCode : int_opt8_t {
		INVALID_CHECKSUM = -101,
		SERIAL_TRANSFER_FAILURE = -100,
		INVALID_PARAMETER = -10,
		NO_DATA_AVAILABLE = -2,
		FAILURE_TO_SYNC = -1,
		SUCCESS = 0,
	};
	
	/// \brief iRobot® Roomba Open Interface (OI) Sensor Packets
	enum PacketId : uint_opt8_t {
		PACKETS_7_THRU_26 = 0,
		PACKETS_7_THRU_16 = 1,
		PACKETS_17_THRU_20 = 2,
		PACKETS_21_THRU_26 = 3,
		PACKETS_27_THRU_34 = 4,
		PACKETS_35_THRU_42 = 5,
		PACKETS_7_THRU_42 = 6,
		BUMPS_AND_WHEEL_DROPS = 7,
		WALL = 8,
		CLIFF_LEFT = 9,
		CLIFF_FRONT_LEFT = 10,
		CLIFF_FRONT_RIGHT = 11,
		CLIFF_RIGHT = 12,
		VIRTUAL_WALL = 13,
		MOTOR_OVERCURRENTS = 14,
		DIRT_DETECT = 15,
		INFRARED_CHARACTER_OMNI = 17,
		BUTTONS = 18,
		DISTANCE = 19,
		ANGLE = 20,
		CHARGING_STATE = 21,
		VOLTAGE = 22,
		CURRENT = 23,
		TEMPERATURE = 24,
		BATTERY_CHARGE = 25,
		BATTERY_CAPACITY = 26,
		WALL_SIGNAL = 27,
		CLIFF_LEFT_SIGNAL = 28,
		CLIFF_FRONT_LEFT_SIGNAL = 29,
		CLIFF_FRONT_RIGHT_SIGNAL = 30,
		CLIFF_RIGHT_SIGNAL = 31,
		CHARGING_SOURCES_AVAILABLE = 34,
		OI_MODE = 35,
		SONG_NUMBER = 36,
		SONG_PLAYING = 37,
		NUMBER_OF_STREAM_PACKETS = 38,
		REQUESTED_VELOCITY = 39,
		REQUESTED_RADIUS = 40,
		REQUESTED_RIGHT_VELOCITY = 41,
		REQUESTED_LEFT_VELOCITY = 42,
		RIGHT_ENCODER_COUNTS = 43,
		LEFT_ENCODER_COUNTS = 44,
		LIGHT_BUMPER = 45,
		LIGHT_BUMP_LEFT_SIGNAL = 46,
		LIGHT_BUMP_FRONT_LEFT_SIGNAL = 47,
		LIGHT_BUMP_CENTER_LEFT_SIGNAL = 48,
		LIGHT_BUMP_CENTER_RIGHT_SIGNAL = 49,
		LIGHT_BUMP_FRONT_RIGHT_SIGNAL = 50,
		LIGHT_BUMP_RIGHT_SIGNAL = 51,
		INFRARED_CHARACTER_LEFT = 52,
		INFRARED_CHARACTER_RIGHT = 53,
		LEFT_MOTOR_CURRENT = 54,
		RIGHT_MOTOR_CURRENT = 55,
		MAIN_BRUSH_MOTOR_CURRENT = 56,
		SIDE_BRUSH_MOTOR_CURRENT = 57,
		STASIS = 58,
		ALL_SENSOR_DATA = 100,
		PACKETS_7_THRU_58 = 100,
		PACKETS_43_THRU_58 = 101,
		PACKETS_46_THRU_51 = 106,
		PACKETS_54_THRU_58 = 107,
	};
	
	/// \brief Function to initialize sensor
	/// \details Enables the sensor functionality by setting
	/// the pointer to the serial read function; enabling
	/// serial communication.
	/// \param fnSerialRead_ A multi-byte read serial read function
	/// \return SUCCESS
	/// \see end
	ReturnCode
	begin (
		std::function<size_t(uint_opt8_t * const, const size_t)> fnSerialRead_
	);
	
	/// \brief Release resources tied to sensors
	/// \details Restores internal variables to a state ready for
	/// the next call to begin().
	/// \return SUCCESS
	/// \see begin
	ReturnCode
	end (
		void
	);
	
	/// \brief Function to receive serial data
	/// \details Parses data received from Roomba and stores
	/// it in memory accessible by the OICommand object.
	void
	parseSerialData (
		void
	);
	
	/// \brief Stores the baud code
	/// \details The baud code is used when calculating the
	/// time required to execute a sensor query transaction.
	/// \return SUCCESS
	/// \return INVALID_PARAMETER
	/// \see OICommand::sensors
	/// \see OICommand::queryList
	ReturnCode
	setBaudCode (
		const BaudCode baud_code_
	);
	
	/// \brief Function to store parse key
	/// \details A parsing key is generated during the call
	/// to queryList() or sensors() to describe the format
	/// of the data returned by the iRobot® Roomba.
	/// \param [in] parse_key_ An array of bytes describing
	/// the data requested from the iRobot® Roomba.
	/// \n Index 0 contains the length of the array.
	/// \n The remaining values are the packet ids of the
	/// data requested from the iRobot® Roomba.
	/// \return SUCCESS
	/// \return INVALID_PARAMETER
	/// \see OICommand::sensors
	/// \see OICommand::queryList
	ReturnCode
	setParseKey (
		PacketId const * const parse_key_
	);
	
	/// \brief Provides access to sensor data
	/// \param [in] packet_id_ The packet id of the sensor
	/// who's value you want
	/// \param [out] value_ A pointer to the sixteen-bit
	/// field to store the value of the requested sensor
	/// \param [out] is_signed_ A boolean indicating if the
	/// value of the requested sensor is signed or unsigned
	/// \return SUCCESS
	/// \return INVALID_PARAMETER
	/// \return SERIAL_TRANSFER_FAILURE
	ReturnCode
	valueOfSensor (
		const PacketId packet_id_,
		uint_opt16_t * const value_,
		bool * const is_signed_
	);
} // namespace sensor
} // namespace oi
} // namespace series500
} // namespace roomba

#endif

/* Created and copyrighted by Zachary J. Fields. All rights reserved. */
