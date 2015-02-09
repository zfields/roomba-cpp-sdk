/* Created and copyrighted by Zachary J. Fields. Offered as open source under the MIT License (MIT). */

#include "state.h"
#include "serial.h"

#include <chrono>
#include <mutex>

namespace roomba {
namespace state {

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
	
	/// \brief Indicates the validity of the sensor packet ids
	/// \details The index of each bit is tied to the corresponding
	/// packet id. If a sensor value returned from the Roomba has
	/// stale or invalid data (as indicated by the checksum) then
	/// the dirty bit will be flagged.
	uint_opt64_t _flag_mask_dirty;
	
	/// \brief The mode associated with the state of the open interface
	/// \details This variable is used to gate function calls.
	OIMode _oi_mode(OFF);
	
	/// \brief Key to decode the Roomba's serial stream
	/// \details The Roomba returns a blob of data representing the
	/// preceding sensor request. The key is a 1-base indexed array
	/// of packet ids ordered the exact same sequence as the request.
	/// \note The zero index is used to store the size.
	/// \note The maximum size allowed is 64, which provides room for
	/// the count and one of each sensor.
	/// \see OICommand::sensors
	/// \see OICommand::queryList
	sensor::PacketId _parse_key[64] = { static_cast<sensor::PacketId>(0) };
	
	/// \brief Status messages resulting from parsing
	/// \details The parsing function is asynchronous, and therefore
	/// cannot return a status code directly.
	ReturnCode _parse_status(SUCCESS);
	
	/// \brief Raw sensor data
	/// \details The data blob used to store sensor data returned from
	/// the iRobot Roomba in big endian format.
	uint_opt8_t _raw_data[80];
	
	/// \brief Time point when all sensor data should be returned
	/// \details Time required for the Roomba to process the query,
	/// then return the requested data at the current baud rate.
	std::chrono::time_point<std::chrono::steady_clock, std::chrono::milliseconds> _serial_read_next_available_ms;
	
	/// \brief Mutex for the shared sensor data
	std::mutex _shared_data;
} // namespace

/// \brief Constant data used to manage data returned from the iRobot® Roomba
/// \details This constant data facilitates inserting and retreiving
/// sensor data from the blob. Some "data" is represented "functionally"
/// to reduce wasted space for sparse data sets. The data functions do
/// not contain error checking, and should only be used internally.
namespace {
	/// \brief Baud rate
	/// \details The baud rate represented as an integer value
	const uint_opt32_t _BAUD_RATE[] = {
		300,     // BAUD_300
		600,     // BAUD_600
		1200,    // BAUD_1200
		2400,    // BAUD_2400
		4800,    // BAUD_4800
		9600,    // BAUD_9600
		14400,   // BAUD_14400
		19200,   // BAUD_19200
		28800,   // BAUD_28800
		38400,   // BAUD_38400
		57600,   // BAUD_57600
		115200,  // BAUD_115200
	};
	
	/// \brief Packet ids associated with signed data
	/// \details A bit mask indicating which packet ids are associated with
	/// signed data.
	const uint_opt64_t _FLAG_MASK_SIGNED = 0x03C0078001980000;

	/// \brief Hardware serial delay
	/// \details The time (in milliseconds) required for the Roomba to receive
	/// a sensor query, collect the requested values, then write them onto the
	/// serial bus.
	/// \note This value is half-adjusted up to enforce rounding.
	const uint_opt8_t HARDWARE_SERIAL_DELAY_MS(4);
	
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
	const sensor::PacketId _PACKET_INFO[] = {
		static_cast<sensor::PacketId>(0x01),  // PACKETS_7_THRU_26
		static_cast<sensor::PacketId>(0x01),  // PACKETS_7_THRU_16
		static_cast<sensor::PacketId>(0x15),  // PACKETS_17_THRU_20
		static_cast<sensor::PacketId>(0x21),  // PACKETS_21_THRU_26
		static_cast<sensor::PacketId>(0x35),  // PACKETS_27_THRU_34
		static_cast<sensor::PacketId>(0x51),  // PACKETS_35_THRU_42
		static_cast<sensor::PacketId>(0x01),  // PACKETS_7_THRU_42
		static_cast<sensor::PacketId>(0x00),  // BUMPS_AND_WHEEL_DROPS
		static_cast<sensor::PacketId>(0x02),  // WALL
		static_cast<sensor::PacketId>(0x04),  // CLIFF_LEFT
		static_cast<sensor::PacketId>(0x06),  // CLIFF_FRONT_LEFT
		static_cast<sensor::PacketId>(0x08),  // CLIFF_FRONT_RIGHT
		static_cast<sensor::PacketId>(0x0A),  // CLIFF_RIGHT
		static_cast<sensor::PacketId>(0x0C),  // VIRTUAL_WALL
		static_cast<sensor::PacketId>(0x0E),  // MOTOR_OVERCURRENTS
		static_cast<sensor::PacketId>(0x10),  // DIRT_DETECT
		static_cast<sensor::PacketId>(0x12),  // RESERVED_1
		static_cast<sensor::PacketId>(0x14),  // INFRARED_CHARACTER_OMNI
		static_cast<sensor::PacketId>(0x16),  // BUTTONS
		static_cast<sensor::PacketId>(0x19),  // DISTANCE
		static_cast<sensor::PacketId>(0x1D),  // ANGLE
		static_cast<sensor::PacketId>(0x20),  // CHARGING_STATE
		static_cast<sensor::PacketId>(0x23),  // VOLTAGE
		static_cast<sensor::PacketId>(0x27),  // CURRENT
		static_cast<sensor::PacketId>(0x2A),  // TEMPERATURE
		static_cast<sensor::PacketId>(0x2D),  // BATTERY_CHARGE
		static_cast<sensor::PacketId>(0x31),  // BATTERY_CAPACITY
		static_cast<sensor::PacketId>(0x35),  // WALL_SIGNAL
		static_cast<sensor::PacketId>(0x39),  // CLIFF_LEFT_SIGNAL
		static_cast<sensor::PacketId>(0x3D),  // CLIFF_FRONT_LEFT_SIGNAL
		static_cast<sensor::PacketId>(0x41),  // CLIFF_FRONT_RIGHT_SIGNAL
		static_cast<sensor::PacketId>(0x45),  // CLIFF_RIGHT_SIGNAL
		static_cast<sensor::PacketId>(0x48),  // RESERVED_2
		static_cast<sensor::PacketId>(0x4B),  // RESERVED_3
		static_cast<sensor::PacketId>(0x4E),  // CHARGING_SOURCES_AVAILABLE
		static_cast<sensor::PacketId>(0x50),  // OI_MODE
		static_cast<sensor::PacketId>(0x52),  // SONG_NUMBER
		static_cast<sensor::PacketId>(0x54),  // SONG_PLAYING
		static_cast<sensor::PacketId>(0x56),  // NUMBER_OF_STREAM_PACKETS
		static_cast<sensor::PacketId>(0x59),  // REQUESTED_VELOCITY
		static_cast<sensor::PacketId>(0x5D),  // REQUESTED_RADIUS
		static_cast<sensor::PacketId>(0x61),  // REQUESTED_RIGHT_VELOCITY
		static_cast<sensor::PacketId>(0x65),  // REQUESTED_LEFT_VELOCITY
		static_cast<sensor::PacketId>(0x69),  // RIGHT_ENCODER_COUNTS
		static_cast<sensor::PacketId>(0x6D),  // LEFT_ENCODER_COUNTS
		static_cast<sensor::PacketId>(0x70),  // LIGHT_BUMPER
		static_cast<sensor::PacketId>(0x73),  // LIGHT_BUMP_LEFT_SIGNAL
		static_cast<sensor::PacketId>(0x77),  // LIGHT_BUMP_FRONT_LEFT_SIGNAL
		static_cast<sensor::PacketId>(0x7B),  // LIGHT_BUMP_CENTER_LEFT_SIGNAL
		static_cast<sensor::PacketId>(0x7F),  // LIGHT_BUMP_CENTER_RIGHT_SIGNAL
		static_cast<sensor::PacketId>(0x83),  // LIGHT_BUMP_FRONT_RIGHT_SIGNAL
		static_cast<sensor::PacketId>(0x87),  // LIGHT_BUMP_RIGHT_SIGNAL
		static_cast<sensor::PacketId>(0x8A),  // INFRARED_CHARACTER_LEFT
		static_cast<sensor::PacketId>(0x8C),  // INFRARED_CHARACTER_RIGHT
		static_cast<sensor::PacketId>(0x8F),  // LEFT_MOTOR_CURRENT
		static_cast<sensor::PacketId>(0x93),  // RIGHT_MOTOR_CURRENT
		static_cast<sensor::PacketId>(0x97),  // MAIN_BRUSH_MOTOR_CURRENT
		static_cast<sensor::PacketId>(0x9B),  // SIDE_BRUSH_MOTOR_CURRENT
		static_cast<sensor::PacketId>(0x9E),  // STASIS
		static_cast<sensor::PacketId>(0x01),  // PACKETS_7_THRU_58
		static_cast<sensor::PacketId>(0x69),  // PACKETS_43_THRU_58
		static_cast<sensor::PacketId>(0x73),  // PACKETS_46_THRU_51
		static_cast<sensor::PacketId>(0x8F)   // PACKETS_54_THRU_58
	};
	
	/// \brief Creates bit-mask of individual packet ids associated with
	/// given packet id
	/// \param [in] packet_id_ Packet id for which to generate a
	/// corresponding bitmask
	/// \warning This function does NOT contain error checking, does
	/// NOT return error codes and should only be used internally.
	/// \return A bitmask representing all associated packet ids
	inline
	uint_opt64_t
	_flagMaskForPacket (
		const sensor::PacketId packet_id_
	) {
		switch (packet_id_) {
		  case sensor::PACKETS_7_THRU_26:
			return 0x0000000007FFFF81;
		  case sensor::PACKETS_7_THRU_16:
			return 0x000000000001FF82;
		  case sensor::PACKETS_17_THRU_20:
			return 0x00000000001E0004;
		  case sensor::PACKETS_21_THRU_26:
			return 0x0000000007E00008;
		  case sensor::PACKETS_27_THRU_34:
			return 0x00000007F8000010;
		  case sensor::PACKETS_35_THRU_42:
			return 0x000007F800000020;
		  case sensor::PACKETS_7_THRU_42:
			return 0x000007FFFFFFFFC0;
		  case sensor::PACKETS_7_THRU_58:
			return 0x0FFFFFFFFFFFFF80;
		  case sensor::PACKETS_43_THRU_58:
			return 0x17FFF80000000000;
		  case sensor::PACKETS_46_THRU_51:
			return 0x200FC00000000000;
		  case sensor::PACKETS_54_THRU_58:
			return 0x47C0000000000000;
		  default:
		    return (static_cast<uint_opt64_t>(1) << packet_id_);
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
		const sensor::PacketId packet_id_
	) {
		switch (packet_id_) {
		  case sensor::PACKETS_7_THRU_58:
			return 59;
		  case sensor::PACKETS_43_THRU_58:
			return 60;
		  case sensor::PACKETS_46_THRU_51:
			return 61;
		  case sensor::PACKETS_54_THRU_58:
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
		const sensor::PacketId packet_id_
	) {
		switch (packet_id_) {
		  case sensor::PACKETS_7_THRU_26:
			return 26;
		  case sensor::PACKETS_7_THRU_16:
			return 10;
		  case sensor::PACKETS_17_THRU_20:
			return 6;
		  case sensor::PACKETS_21_THRU_26:
			return 10;
		  case sensor::PACKETS_27_THRU_34:
			return 14;
		  case sensor::PACKETS_35_THRU_42:
			return 12;
		  case sensor::PACKETS_7_THRU_42:
			return 52;
		  case sensor::PACKETS_7_THRU_58:
			return 80;
		  case sensor::PACKETS_43_THRU_58:
			return 28;
		  case sensor::PACKETS_46_THRU_51:
			return 12;
		  case sensor::PACKETS_54_THRU_58:
			return 9;
		  default:
		    return 2;
		}
	}
} // namespace

/// \brief Internal helper functions
/// \details Functions used for common calculations. These functions do not
/// supply return codes and are considered to be unsafe.
namespace {
	/// \brief Calculate bytes required by packet id list
	/// \param [in] query_list_ List used to count bytes
	/// \n Index 0 contains the length of the array.
	/// \n The remaining values are the packet ids of the
	/// data requested from the iRobot® Roomba.
	/// \return The byte count of the packets represented by the parse key
	/// \see state::setParseKey
	inline
	uint_opt16_t
	_bytesInQueryList (
		sensor::PacketId const * const query_list_
	) {
		uint_opt16_t byte_count(0);
		
		for ( uint_opt8_t i = 1 ; i < *query_list_ ; ++i ) {
			const uint_opt8_t pi = _packetIndex(query_list_[i]);
			if ( _PACKET_INFO[pi] & 0x01 ) {
				byte_count += _packetSize(query_list_[i]);
			} else {
				++byte_count;
			}
		}
		
		return byte_count;
	}

	/// \brief Store a packet in shared memory
	/// \details The data value of the packet id will be read from the
	/// serial bus and stored into shared memory.
	/// \param [in] packet_id_ Packet Id associated with the data
	/// \param [out] packet_size_ Returns the size of the packet stored
	/// \param [out] check_sum_ Tracks the check sum of the data being transferred
	/// \return SUCCESS
	/// \return SERIAL_TRANSFER_FAILURE
	/// \see state::parseQueryData
	/// \see state::parseStreamData
	inline
	ReturnCode
	_readPacketValueIntoRawDataBlob (
		const sensor::PacketId packet_id_,
		uint_opt8_t * const packet_size_,
		uint_opt8_t * const check_sum_
	) {
		const uint_opt8_t pi = _packetIndex(packet_id_);
		uint_opt8_t bytes_read = 0;
		bool size_greater_than_one = ( _PACKET_INFO[pi] & 0x01 );
		*packet_size_ = (size_greater_than_one * _packetSize(packet_id_)) + !size_greater_than_one;
		bytes_read = serial::multiByteSerialRead((_raw_data + (_PACKET_INFO[pi] >> 1)), *packet_size_);
		if ( bytes_read != *packet_size_ ) {
			return SERIAL_TRANSFER_FAILURE;
		}
		for ( uint_opt8_t i = 0 ; i < *packet_size_ ; ++i ) {
			*check_sum_ += (_raw_data + (_PACKET_INFO[pi] >> 1))[i];
		}
		return SUCCESS;
	}
} // namespace

ReturnCode
getParseError (
	void
) {
	return _parse_status;
}

ReturnCode
parseQueryData (
	void
) {
	uint_opt64_t flag_mask_received(0);
	const uint_opt8_t packet_count = *_parse_key;
	uint_opt8_t unused;
	*_parse_key = static_cast<sensor::PacketId>(0);
	for ( uint_opt8_t i = 1 ; i < packet_count ; ++i ) {
		const ReturnCode rc = _readPacketValueIntoRawDataBlob(_parse_key[i], &unused, &unused);
		if ( SUCCESS != rc ) { return rc; }
		flag_mask_received |= (static_cast<uint_opt64_t>(1) << _packetIndex(_parse_key[i]));
	}
	_flag_mask_dirty &= ~flag_mask_received;
	return SUCCESS;
}

ReturnCode
parseStreamData (
	void
) {
	uint_opt64_t flag_mask_received(0);
	uint_opt8_t bytes_read, byte_sum, check_sum, header[2];
	
	// Get header information
	bytes_read = serial::multiByteSerialRead(header, sizeof(header));
	if ( bytes_read != sizeof(header) ) { return SERIAL_TRANSFER_FAILURE; }
	//TODO: Attempt to recover from unsynchronized stream
	if ( 19 != header[0] ) { return FAILURE_TO_SYNC; }
	byte_sum = header[1];
	
	// Parse stream
	for ( uint_opt8_t i = 1 ; i < header[1] ; ++i ) {
		uint_opt8_t packet_id, packet_size;
		
		// Get packet id
		bytes_read = serial::multiByteSerialRead(&packet_id, sizeof(packet_id));
		if ( bytes_read != sizeof(packet_id) ) { return SERIAL_TRANSFER_FAILURE; }
		byte_sum += packet_id;
		
		// Insert packet data into blob
		const ReturnCode rc = _readPacketValueIntoRawDataBlob(static_cast<sensor::PacketId>(packet_id), &packet_size, &byte_sum);
		if ( SUCCESS != rc ) { return rc; }
		
		// Flag packet as received
		flag_mask_received |= (static_cast<uint_opt64_t>(1) << _packetIndex(static_cast<sensor::PacketId>(packet_id)));
		i += packet_size;
	}
	
	// Test the check sum
	bytes_read = serial::multiByteSerialRead(&check_sum, sizeof(check_sum));
	if ( bytes_read != sizeof(check_sum) ) { return SERIAL_TRANSFER_FAILURE; }
	if ( static_cast<uint_opt8_t>(check_sum + byte_sum) ) { return INVALID_CHECKSUM; }
	
	// Clear the dirty flag for received packets
	_flag_mask_dirty &= ~flag_mask_received;
	return SUCCESS;
}

ReturnCode
setBaudCode (
	const BaudCode baud_code_
) {
	if ( baud_code_ > 11 ) { return INVALID_PARAMETER; }
	serial::beginAtBaudCode(baud_code_);
	_baud_code = baud_code_;
	return SUCCESS;
}

ReturnCode
setOIMode (
	const OIMode oi_mode_
) {
	if ( oi_mode_ > 3 ) { return INVALID_PARAMETER; }
	_oi_mode = oi_mode_;
	return SUCCESS;
}

ReturnCode
setParseKey (
	sensor::PacketId const * const parse_key_
) {
	if ( !parse_key_ ) { return INVALID_PARAMETER; }
	if ( !(*parse_key_) ) { return INVALID_PARAMETER; }
	
	// Calculate completion time (including Roomba signal processing time)
	std::chrono::milliseconds transfer_time_ms(HARDWARE_SERIAL_DELAY_MS + ((_bytesInQueryList(parse_key_) * 10000) / _BAUD_RATE[_baud_code]));
	std::chrono::time_point<std::chrono::steady_clock, std::chrono::milliseconds> serial_read_next_available_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()) + transfer_time_ms;
	
	{  // Critical section: Update shared memory
		_shared_data.lock();
		
		memcpy(_parse_key, parse_key_, *reinterpret_cast<const uint_opt8_t *>(parse_key_));
		_flag_mask_dirty = static_cast<uint_opt64_t>(-1);
		_serial_read_next_available_ms = serial_read_next_available_ms;
		
		_shared_data.unlock();
	}
	
	return SUCCESS;
}

#ifdef TESTING
namespace testing {
	BaudCode
	getBaudCode (
		void
	) {
		return _baud_code;
	}
	
	uint_opt64_t
	getFlagMaskDirty (
		void
	) {
		return _flag_mask_dirty;
	}
	
	uint_opt64_t
	getFlagMaskSigned (
		void
	) {
		return _FLAG_MASK_SIGNED;
	}
	
	OIMode
	getOIMode (
		void
	) {
		return _oi_mode;
	}
	
	sensor::PacketId *
	getParseKey (
		void
	) {
		return _parse_key;
	}
	
	uint_opt8_t *
	getRawData (
		void
	) {
		return _raw_data;
	}
	
	std::chrono::time_point<std::chrono::steady_clock, std::chrono::milliseconds>
	getSerialReadNextAvailableMs (
		void
	) {
		return _serial_read_next_available_ms;
	}
	
	void
	setInternalsToInitialState (
		void
	) {
		_baud_code = BAUD_115200;
		_oi_mode = OFF;
		serial::beginAtBaudCode(_baud_code);
		_flag_mask_dirty = static_cast<uint_opt64_t>(-1);
		*_parse_key = static_cast<sensor::PacketId>(0);
		_parse_status = SUCCESS;
	}
} // namespace testing
#endif

} // namespace state
} // namespace roomba

/* Created and copyrighted by Zachary J. Fields. Offered as open source under the MIT License (MIT). */
