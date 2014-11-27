/* Created and copyrighted by Zachary J. Fields. All rights reserved. */

#include "OISensors.h"

#include <chrono>
#include <mutex>

namespace roomba {
namespace series500 {
namespace oi {
namespace sensors {

/// \brief Format of the data stored in shared memory
/// \details The variable data utilized by both the OICommand and
/// oi:sensors methods. OICommand and the oi::sensors methods execute
/// concurrently, therefore this data will be accessed concurrently.
/// All methods accessing this data will need to take possession
/// of the internal mutex to ensure the integrity of this data.
namespace {
	/// \brief The baud rate associated with the serial bus
	/// \details This variable is used to calculate buffer overrun
	/// protection.
	BaudCode _baud_code(BAUD_115200);
	
	/// \brief Time point when all sensor data should be returned
	/// \details Time required for the Roomba to process the query,
	/// then return the requested data at the current baud rate.
	std::chrono::system_clock::time_point _transfer_complete_time;
	
	/// \brief Indicates the validity of the sensor packet ids
	/// \details The index of each bit is tied to the corresponding
	/// packet id. If a sensor value returned from the Roomba has
	/// stale or invalid data (as indicated by the checksum) then
	/// the dirty bit will be flagged.
	uint64_t _flag_mask_dirty;
	
	/// \brief Multi-byte read access to the serial bus
	/// \detials a user supplied function that provides read access
	/// to the serial bus. This is intended to provide an abstraction
	/// from the particular hardware involved.
	std::function<size_t(uint_opt8_t * const, const size_t)> _fnSerialRead([](uint_opt8_t * const, const size_t){ return 0; });
	
	/// \brief Key to decode the Roomba's serial stream
	/// \details The Roomba returns a blob of data representing the
	/// preceding sensor request. The key is a 1-base indexed array
	/// of packet ids ordered the exact same sequence as the request.
	/// \note The zero index is used to store the size.
	/// \note The maximum size allowed is 64, which provides room for
	/// the count and one of each sensor.
	/// \see OICommand::sensors
	/// \see OICommand::queryList
	PacketId _parse_key[64] = { static_cast<PacketId>(0) };
	
	/// \brief Status messages resulting from parsing
	/// \details The parsing function is asynchronous, and therefore
	/// cannot return a status code directly.
	ReturnCode _parse_status(SUCCESS);
	
	/// \brief Raw sensor data
	/// \details The data blob used to store sensor data returned from
	/// the Roomba.
	uint_opt8_t _raw_data[80];
	
	/// \brief Ready state of oi:sensors methods
	bool _sensors_ready(false);	
	
	/// \brief Mutex for the shared sensor data
	std::mutex _variable_data;
} // namespace

/// \brief Constant data used to manage data returned from the iRobot® Roomba
/// \details This constant data facilitates inserting and retreiving
/// sensor data from the blob. Some "data" is represented "functionally"
/// to reduce wasted space for sparse data sets. The data functions do
/// not contain error checking, and should only be used internally.
namespace {
	/// \brief Packet ids associated with signed data
	/// \details A bit mask indicating which packet ids are
	/// associated with signed data
	const uint64_t _FLAG_MASK_SIGNED = 0x03C0078001980000;

	/// \brief Packet offset and size information
	/// \details A table providing information regarding the pointer offset
	/// where the associated data can be found (high bits 7-1), and a flag
	/// (low bit) indicating whether the size of the data is 1 byte or larger.
	/// \n Format:
	/// \n |7|6|5|4|3|2|1|0|
	/// \n |o|o|o|o|o|o|o|>|
	/// \n Access:
	/// * Offset value: _PACKET_INFO[x] >> 1
	/// * Size boolean: _PACKET_INFO[x] & 0x01
	const PacketId _PACKET_INFO[] = {
		static_cast<PacketId>(0x01),  // PACKETS_7_THRU_26
		static_cast<PacketId>(0x01),  // PACKETS_7_THRU_16
		static_cast<PacketId>(0x15),  // PACKETS_17_THRU_20
		static_cast<PacketId>(0x21),  // PACKETS_21_THRU_26
		static_cast<PacketId>(0x35),  // PACKETS_27_THRU_34
		static_cast<PacketId>(0x51),  // PACKETS_35_THRU_42
		static_cast<PacketId>(0x01),  // PACKETS_7_THRU_42
		static_cast<PacketId>(0x00),  // BUMPS_AND_WHEEL_DROPS
		static_cast<PacketId>(0x02),  // WALL
		static_cast<PacketId>(0x04),  // CLIFF_LEFT
		static_cast<PacketId>(0x06),  // CLIFF_FRONT_LEFT
		static_cast<PacketId>(0x08),  // CLIFF_FRONT_RIGHT
		static_cast<PacketId>(0x0A),  // CLIFF_RIGHT
		static_cast<PacketId>(0x0C),  // VIRTUAL_WALL
		static_cast<PacketId>(0x0E),  // MOTOR_OVERCURRENTS
		static_cast<PacketId>(0x10),  // DIRT_DETECT
		static_cast<PacketId>(0x12),  // RESERVED_1
		static_cast<PacketId>(0x14),  // INFRARED_CHARACTER_OMNI
		static_cast<PacketId>(0x16),  // BUTTONS
		static_cast<PacketId>(0x19),  // DISTANCE
		static_cast<PacketId>(0x1D),  // ANGLE
		static_cast<PacketId>(0x20),  // CHARGING_STATE
		static_cast<PacketId>(0x23),  // VOLTAGE
		static_cast<PacketId>(0x27),  // CURRENT
		static_cast<PacketId>(0x2A),  // TEMPERATURE
		static_cast<PacketId>(0x2D),  // BATTERY_CHARGE
		static_cast<PacketId>(0x31),  // BATTERY_CAPACITY
		static_cast<PacketId>(0x35),  // WALL_SIGNAL
		static_cast<PacketId>(0x39),  // CLIFF_LEFT_SIGNAL
		static_cast<PacketId>(0x3D),  // CLIFF_FRONT_LEFT_SIGNAL
		static_cast<PacketId>(0x41),  // CLIFF_FRONT_RIGHT_SIGNAL
		static_cast<PacketId>(0x45),  // CLIFF_RIGHT_SIGNAL
		static_cast<PacketId>(0x48),  // RESERVED_2
		static_cast<PacketId>(0x4B),  // RESERVED_3
		static_cast<PacketId>(0x4E),  // CHARGING_SOURCES_AVAILABLE
		static_cast<PacketId>(0x50),  // OI_MODE
		static_cast<PacketId>(0x52),  // SONG_NUMBER
		static_cast<PacketId>(0x54),  // SONG_PLAYING
		static_cast<PacketId>(0x56),  // NUMBER_OF_STREAM_PACKETS
		static_cast<PacketId>(0x59),  // REQUESTED_VELOCITY
		static_cast<PacketId>(0x5D),  // REQUESTED_RADIUS
		static_cast<PacketId>(0x61),  // REQUESTED_RIGHT_VELOCITY
		static_cast<PacketId>(0x65),  // REQUESTED_LEFT_VELOCITY
		static_cast<PacketId>(0x69),  // RIGHT_ENCODER_COUNTS
		static_cast<PacketId>(0x6D),  // LEFT_ENCODER_COUNTS
		static_cast<PacketId>(0x70),  // LIGHT_BUMPER
		static_cast<PacketId>(0x73),  // LIGHT_BUMP_LEFT_SIGNAL
		static_cast<PacketId>(0x77),  // LIGHT_BUMP_FRONT_LEFT_SIGNAL
		static_cast<PacketId>(0x7B),  // LIGHT_BUMP_CENTER_LEFT_SIGNAL
		static_cast<PacketId>(0x7F),  // LIGHT_BUMP_CENTER_RIGHT_SIGNAL
		static_cast<PacketId>(0x83),  // LIGHT_BUMP_FRONT_RIGHT_SIGNAL
		static_cast<PacketId>(0x87),  // LIGHT_BUMP_RIGHT_SIGNAL
		static_cast<PacketId>(0x8A),  // INFRARED_CHARACTER_LEFT
		static_cast<PacketId>(0x8C),  // INFRARED_CHARACTER_RIGHT
		static_cast<PacketId>(0x8F),  // LEFT_MOTOR_CURRENT
		static_cast<PacketId>(0x93),  // RIGHT_MOTOR_CURRENT
		static_cast<PacketId>(0x97),  // MAIN_BRUSH_MOTOR_CURRENT
		static_cast<PacketId>(0x9B),  // SIDE_BRUSH_MOTOR_CURRENT
		static_cast<PacketId>(0x9E),  // STASIS
		static_cast<PacketId>(0x01),  // PACKETS_7_THRU_58
		static_cast<PacketId>(0x69),  // PACKETS_43_THRU_58
		static_cast<PacketId>(0x73),  // PACKETS_46_THRU_51
		static_cast<PacketId>(0x8F)   // PACKETS_54_THRU_58
	};
	
	/// \brief Creates bit-mask of individual packet ids associated with
	/// given packet id
	/// \param [in] packet_id_ Packet id for which to generate a
	/// corresponding bitmask
	/// \warning This function does NOT contain error checking, does
	/// NOT return error codes and should only be used internally.
	/// \return A bitmask representing all associated packet ids
	uint64_t
	_flagMaskForPacket (
		const PacketId packet_id_
	) {
		switch (packet_id_) {
		  case PACKETS_7_THRU_26:
			return 0x0000000007FFFF81;
		  case PACKETS_7_THRU_16:
			return 0x000000000001FF82;
		  case PACKETS_17_THRU_20:
			return 0x00000000001E0004;
		  case PACKETS_21_THRU_26:
			return 0x0000000007E00008;
		  case PACKETS_27_THRU_34:
			return 0x00000007F8000010;
		  case PACKETS_35_THRU_42:
			return 0x000007F800000020;
		  case PACKETS_7_THRU_42:
			return 0x000007FFFFFFFFC0;
		  case PACKETS_7_THRU_58:
			return 0x0FFFFFFFFFFFFF80;
		  case PACKETS_43_THRU_58:
			return 0x17FFF80000000000;
		  case PACKETS_46_THRU_51:
			return 0x200FC00000000000;
		  case PACKETS_54_THRU_58:
			return 0x47C0000000000000;
		  default:
		    return (static_cast<uint64_t>(1) << packet_id_);
		}
	}
	
	/// \brief Array index for packet id
	/// \details Maps packet ids into indices between 0-62 which enables
	/// the ability to use 64-bit bitmask to represent flags associated
	/// with each packet, as well as the ability to create compact (non-
	/// spare) arrays of informational data associated with each packet.
	/// \param [in] packet_id_ Packet id for which to provide the
	/// corresponding index
	/// \warning This function does NOT contain error checking, does
	/// NOT return error codes and should only be used internally.
	/// \return Index associated with the packet id provided
	inline
	uint_opt8_t
	_packetIndex (
		const PacketId packet_id_
	) {
		switch (packet_id_) {
		  case PACKETS_7_THRU_58:
			return 59;
		  case PACKETS_43_THRU_58:
			return 60;
		  case PACKETS_46_THRU_51:
			return 61;
		  case PACKETS_54_THRU_58:
			return 62;
		  default:
		    return packet_id_;
		}
	}
	
	/// \brief Provides the size of the packet (in bytes)
	/// \details Provides the size of the packet (in bytes) when the
	/// value is greater than one. Packet data is stored in a single
	/// memory location as a data blob. Packet groups are a subset of
	/// packets which reside in contiguous memory locations. Therefore,
	/// the data for a packet group can be written or read as a single
	/// operation when the size is provided.
	/// \param [in] packet_id_ Packet id for which to provide the
	/// corresponding size
	/// \note The use of this function is only necessary when the
	/// low-bit of the _PACKET_INFO table is set ON (true).
	/// \note Values located in iRobot® Roomba Open Interface (OI)
	/// Specification (page 19)
	/// \warning This function does NOT contain error checking, does
	/// NOT return error codes and should only be used internally.
	/// \return The size (in bytes) of the packet id provided
	/// \see _PACKET_INFO[]
	inline
	uint_opt8_t
	_packetSize (
		const PacketId packet_id_
	) {
		switch (packet_id_) {
		  case PACKETS_7_THRU_26:
			return 26;
		  case PACKETS_7_THRU_16:
			return 10;
		  case PACKETS_17_THRU_20:
			return 6;
		  case PACKETS_21_THRU_26:
			return 10;
		  case PACKETS_27_THRU_34:
			return 14;
		  case PACKETS_35_THRU_42:
			return 12;
		  case PACKETS_7_THRU_42:
			return 52;
		  case PACKETS_7_THRU_58:
			return 80;
		  case PACKETS_43_THRU_58:
			return 28;
		  case PACKETS_46_THRU_51:
			return 12;
		  case PACKETS_54_THRU_58:
			return 9;
		  default:
		    return 2;
		}
	}
} // namespace

ReturnCode
begin (
	std::function<size_t(uint_opt8_t * const, const size_t)> fnSerialRead_
) {
	_fnSerialRead = fnSerialRead_;
	return SUCCESS;
}

ReturnCode
end (
	void
) {
	_fnSerialRead = ([](uint_opt8_t * const, const size_t){ return 0; });
	return SUCCESS;
}

ReturnCode
setBaudCode (
	const BaudCode baud_code_
) {
	_baud_code = baud_code_;
	return SUCCESS;
}

ReturnCode
setParseKey (
	PacketId const * const parse_key_
) {
	uint_opt8_t byte_count(0), serial_bit_count(0);
	
	if ( !parse_key_ ) { return INVALID_PARAMETER; }
	for ( uint_opt8_t i = 1 ; i < *parse_key_ ; ++i ) {
//		uint_opt8_t pi = _packetIndex(parse_key_[i]);
//		if ( _PACKET_INFO[pi] & 0x01 ) {
//			size += _packetSize(pi);
//		} else {
			++byte_count;
//		}
	}
	serial_bit_count = 10 * byte_count;
	
	memcpy(_parse_key, parse_key_, *reinterpret_cast<const uint_opt8_t *>(parse_key_));
	return SUCCESS;
}

ReturnCode
valueOfSensor (
	const PacketId packet_id_,
	uint16_t * const value_,
	bool * const is_signed_
) {
	return SERIAL_TRANSFER_FAILURE;
}

#ifdef TESTING
namespace testing {
	size_t
	fnSerialRead (
		uint_opt8_t * const buffer_,
		const size_t buffer_length_
	) {
		return _fnSerialRead(buffer_, buffer_length_);
	}
	
	BaudCode
	getBaudCode (
		void
	) {
		return _baud_code;
	}

	PacketId *
	getParseKey (
		void
	) {
		return _parse_key;
	}
} // namespace testing
#endif

} // namespace sensors
} // namespace oi
} // namespace series500
} // namespace roomba

/* Created and copyrighted by Zachary J. Fields. All rights reserved. */
