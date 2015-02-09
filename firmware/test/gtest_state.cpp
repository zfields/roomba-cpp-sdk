/* Created and copyrighted by Zachary J. Fields. Offered as open source under the MIT License (MIT). */

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "TEST_state.h"
#include "MOCK_serial.h"

//TODO: Guarantee queryList() calculates the time required to retrieve the amount of data it is requesting (via setParseKey())
//TODO: Guarantee the stream is paused when queryList() is called
//TODO: See what happens when a request goes out while streaming data is being returned - expecting nothing, as serial is asynchronous
//TODO: Ensure _serial_read_next_available_ms is not updated on fail cases
//TODO: Ensure _parse_key is not updated on fail cases
//TODO: Serial read next available time, needs to be incorporated into the framework

//TODO: Design new thread function used to wait for available data
//TODO: Determine appropriate mutexing of shared data
//TODO: Design a state machine to test and track _oi_mode

//TODO: Consider the dirty mask - should it only be corrupt data or should it include stale data - if only corrupt, then refactor name
//TODO: When data is out of sync, then it should pause data stream, then resume to sync. When unpaused does it resume where it left off, or send a new stream?

//TODO: Check HARDWARE_SERIAL_DELAY_MS on scope
//TODO: Make HARDWARE_SERIAL_DELAY_MS a tunable variable

//NOTE: Considered method to return multiple sensor values (std::tuple<uint_opt8_t packet_id_, uint16_t value_, bool signed_>), decided that is too far up the stack to be in the scope of this SDK. 

using namespace roomba;

namespace {

  /********************/
 /* HELPER FUNCTIONS */
/********************/
inline
uint_opt16_t
convertTwoByteIntegerFromBigToLittleEndian (
	uint_opt16_t value_
) {
	reinterpret_cast<uint_opt8_t *>(&value_)[0] ^= reinterpret_cast<uint_opt8_t *>(&value_)[1];
	reinterpret_cast<uint_opt8_t *>(&value_)[1] ^= reinterpret_cast<uint_opt8_t *>(&value_)[0];
	reinterpret_cast<uint_opt8_t *>(&value_)[0] ^= reinterpret_cast<uint_opt8_t *>(&value_)[1];

	return value_;
}

  /******************/
 /* MOCK SCENARIOS */
/******************/
class InitialState : public ::testing::Test {
  protected:
	InitialState (
		void
	) {
		state::testing::setInternalsToInitialState();
	}
	
	//virtual ~InitialState() {}
	//virtual void SetUp() {}
	//virtual void TearDown() {}
};

class QueryData : public ::testing::Test {
  protected:
	QueryData (
		void
	) :
		parse_key{ sizeof(parse_key), sensor::CLIFF_FRONT_LEFT_SIGNAL, sensor::VIRTUAL_WALL },
		serial_stream{ 0x02, 0x19, 0x00 }
	{
		state::testing::setInternalsToInitialState();
	}
	
	//virtual ~QueryData() {}
	virtual void SetUp() {
		serial::mock::setSerialReadFunc( 
			[&] (uint_opt8_t * const buffer_, const size_t buffer_length_) {
				memcpy(buffer_, serial_stream, buffer_length_);
				for ( uint_opt8_t i = buffer_length_ ; i < sizeof(serial_stream) ; ++i ) {
					serial_stream[(i - buffer_length_)] = serial_stream[i];
				}
				return buffer_length_;
			}
		);
		state::setParseKey(reinterpret_cast<const sensor::PacketId *>(parse_key));
	}
	//virtual void TearDown() {}
	
	const uint_opt8_t parse_key[3];
	uint_opt8_t serial_stream[3];
};

class QueryData$ByteCountError : public ::testing::Test {
  protected:
	QueryData$ByteCountError (
		void
	) :
		parse_key{ sizeof(parse_key), sensor::CLIFF_FRONT_LEFT_SIGNAL, sensor::VIRTUAL_WALL },
		serial_stream{ 0x02, 0x19, 0x00 },
		call_count(0),
		fail_on_call(1)
	{}

	//virtual ~QueryData$ByteCountError() {}
	virtual void SetUp() {
		state::testing::setInternalsToInitialState();
		serial::mock::setSerialReadFunc(
			[&] (uint_opt8_t * const buffer_, const size_t buffer_length_) {
				++call_count;
				memcpy(buffer_, serial_stream, buffer_length_);
				for ( uint_opt8_t i = buffer_length_ ; i < sizeof(serial_stream) ; ++i ) {
					serial_stream[(i - buffer_length_)] = serial_stream[i];
				}
				if ( call_count == fail_on_call ) { return (buffer_length_ - 1); }
				return buffer_length_;
			}
		);
		state::setParseKey(reinterpret_cast<const sensor::PacketId *>(parse_key));
	}
	//virtual void TearDown() {}
	
	const uint_opt8_t parse_key[3];
	uint_opt8_t serial_stream[3];
	size_t call_count, fail_on_call;
};

class StreamData : public ::testing::Test {
  protected:
	StreamData (
		void
	) :
		serial_stream{ 0x13, 0x05, 0x1D, 0x02, 0x19, 0x0D, 0x00, 0xB6 }
	{
		state::testing::setInternalsToInitialState();
	}
	
	//virtual ~StreamData() {}
	virtual void SetUp() {
		serial::mock::setSerialReadFunc(
			[&] (uint_opt8_t * const buffer_, const size_t buffer_length_) {
				memcpy(buffer_, serial_stream, buffer_length_);
				for ( uint_opt8_t i = buffer_length_ ; i < sizeof(serial_stream) ; ++i ) {
					serial_stream[(i - buffer_length_)] = serial_stream[i];
				}
				return buffer_length_;
			}
		);
	}
	//virtual void TearDown() {}
	
	uint_opt8_t serial_stream[8];
};

class StreamData$BadCheckSum : public ::testing::Test {
  protected:
	StreamData$BadCheckSum (
		void
	) :
		serial_stream{ 0x13, 0x05, 0x1D, 0x02, 0x19, 0x0D, 0x00, 0xBE }
	{
		state::testing::setInternalsToInitialState();
	}
	
	//virtual ~StreamData$BadCheckSum() {}
	virtual void SetUp() {
		serial::mock::setSerialReadFunc(
			[&] (uint_opt8_t * const buffer_, const size_t buffer_length_) {
				memcpy(buffer_, serial_stream, buffer_length_);
				for ( uint_opt8_t i = buffer_length_ ; i < sizeof(serial_stream) ; ++i ) {
					serial_stream[(i - buffer_length_)] = serial_stream[i];
				}
				return buffer_length_;
			}
		);
	}
	//virtual void TearDown() {}

	uint_opt8_t serial_stream[8];
};

class StreamData$ByteCountError : public ::testing::Test {
  protected:
	StreamData$ByteCountError (
		void
	) :
		serial_stream{ 0x13, 0x05, 0x1D, 0x02, 0x19, 0x0D, 0x00, 0xB6 },
		call_count(0),
		fail_on_call(1)
	{
		state::testing::setInternalsToInitialState();
	}
	
	//virtual ~StreamData$ByteCountError() {}
	virtual void SetUp() {
		serial::mock::setSerialReadFunc(
			[&] (uint_opt8_t * const buffer_, const size_t buffer_length_) {
				++call_count;
				memcpy(buffer_, serial_stream, buffer_length_);
				for ( uint_opt8_t i = buffer_length_ ; i < sizeof(serial_stream) ; ++i ) {
					serial_stream[(i - buffer_length_)] = serial_stream[i];
				}
				if ( call_count == fail_on_call ) { return (buffer_length_ - 1); }
				return buffer_length_;
			}
		);
	}
	//virtual void TearDown() {}
	
	uint_opt8_t serial_stream[8];
	size_t call_count, fail_on_call;
};

class StreamData$Paused : public ::testing::Test {
  protected:
	StreamData$Paused (
		void
	) :
		serial_stream{ 0x13, 0x05, 0x1D, 0x02, 0x19, 0x0D }
	{
		state::testing::setInternalsToInitialState();
	}
	
	//virtual ~StreamData$Paused() {}
	virtual void SetUp() {
		serial::mock::setSerialReadFunc(
			[&] (uint_opt8_t * const buffer_, const size_t buffer_length_) {
				memcpy(buffer_, serial_stream, buffer_length_);
				for ( uint_opt8_t i = buffer_length_ ; i < sizeof(serial_stream) ; ++i ) {
					serial_stream[(i - buffer_length_)] = serial_stream[i];
				}
				return buffer_length_;
			}
		);
	}
	//virtual void TearDown() {}

	uint_opt8_t serial_stream[6];
};

class StreamData$OutOfSync : public ::testing::Test {
  protected:
	StreamData$OutOfSync (
		void
	) :
		serial_stream{ 0x19, 0x0D, 0x00, 0xB6, 0x13, 0x05, 0x1D, 0x02, 0x19, 0x0D, 0x00, 0xB6, 0x13, 0x05, 0x1D }
	{
		state::testing::setInternalsToInitialState();
	}
	
	//virtual ~StreamData$OutOfSync() {}
	virtual void SetUp() {
		serial::mock::setSerialReadFunc(
			[&] (uint_opt8_t * const buffer_, const size_t buffer_length_) {
				memcpy(buffer_, serial_stream, buffer_length_);
				for ( uint_opt8_t i = buffer_length_ ; i < sizeof(serial_stream) ; ++i ) {
					serial_stream[(i - buffer_length_)] = serial_stream[i];
				}
				return buffer_length_;
			}
		);
	}
	//virtual void TearDown() {}

	uint_opt8_t serial_stream[15];
};

/*
The first argument is the name of the test case (or fixture), and the
second argument is the test's name within the test case. Both names must
be valid C++ identifiers, and they should not contain underscore (_). A
test's full name consists of its containing test case and its individual
name. Tests from different test cases can have the same individual name.
(e.g. ASSERT_EQ(_EXPECTED_, _ACTUAL_))
*/
TEST_F(InitialState, setBaudCode$WHENCalledTHENBaudCodeIsSet) {
	ASSERT_EQ(BAUD_115200, state::testing::getBaudCode());
	ASSERT_EQ(SUCCESS, state::setBaudCode(BAUD_57600));
	EXPECT_EQ(BAUD_57600, state::testing::getBaudCode());
}

TEST_F(InitialState, setBaudCode$WHENCalledTHENBeginAtBaudCodeIsCalled) {
	ASSERT_EQ(BAUD_115200, state::testing::getBaudCode());
	ASSERT_EQ(BAUD_115200, serial::mock::getBaudCode());
	ASSERT_EQ(SUCCESS, state::setBaudCode(BAUD_57600));
	EXPECT_EQ(BAUD_57600, serial::mock::getBaudCode());
}

TEST_F(InitialState, setBaudCode$WHENBaudCodeIsGreaterThan11THENParameterIsInvalid) {
	for ( int i = 12 ; i <= 255 ; ++i ) {
		EXPECT_EQ(INVALID_PARAMETER, state::setBaudCode(static_cast<BaudCode>(i))) << "Accepted value <" << static_cast<unsigned int>(i) << ">!";
	}
}

TEST_F(InitialState, setBaudCode$WHENParameterIsInvalidTHENBaudCodeIsNotSet) {
	for ( int i = 12 ; i <= 255 ; ++i ) {
		EXPECT_EQ(INVALID_PARAMETER, state::setBaudCode(static_cast<BaudCode>(i)));
		EXPECT_EQ(BAUD_115200, state::testing::getBaudCode());
	}
}

TEST_F(InitialState, setOIMode$WHENCalledTHENOIModeIsSet) {
	ASSERT_EQ(OFF, state::testing::getOIMode());
	ASSERT_EQ(SUCCESS, state::setOIMode(PASSIVE));
	EXPECT_EQ(PASSIVE, state::testing::getOIMode());
}

TEST_F(InitialState, setOIMode$WHENOIModeIsGreaterThan3THENParameterIsInvalid) {
	for ( int i = 4 ; i <= 255 ; ++i ) {
		EXPECT_EQ(INVALID_PARAMETER, state::setOIMode(static_cast<OIMode>(i))) << "Accepted value <" << static_cast<unsigned int>(i) << ">!";
	}
}

TEST_F(InitialState, setOIMode$WHENParameterIsInvalidTHENOIModeIsNotSet) {
	for ( int i = 4 ; i <= 255 ; ++i ) {
		EXPECT_EQ(INVALID_PARAMETER, state::setOIMode(static_cast<OIMode>(i)));
		EXPECT_EQ(OFF, state::testing::getOIMode());
	}
}

TEST_F(InitialState, setParseKey$WHENBeforeCallTHENParseKeyIsNotSet) {
	ASSERT_EQ(0, *reinterpret_cast<uint_opt8_t *>(state::testing::getParseKey()));
}

TEST_F(InitialState, setParseKey$WHENCalledTHENParseKeyIsSet) {
	const uint_opt8_t parse_key[2] = { sizeof(parse_key), sensor::BUTTONS };
	state::setParseKey(reinterpret_cast<const sensor::PacketId *>(parse_key));
	ASSERT_EQ(2, *reinterpret_cast<uint_opt8_t *>(state::testing::getParseKey()));
	ASSERT_EQ(sensor::BUTTONS, *reinterpret_cast<uint_opt8_t *>(state::testing::getParseKey() + 1));
}

TEST_F(InitialState, setParseKey$WHENCalledWithNULLTHENErrorIsReturned) {
	ASSERT_EQ(INVALID_PARAMETER, state::setParseKey(NULL));
}

TEST_F(InitialState, setParseKey$WHENCalledWithZeroSizeTHENErrorIsReturned) {
	const uint_opt8_t parse_key[2] = { 0, sensor::BUTTONS };
	ASSERT_EQ(INVALID_PARAMETER, state::setParseKey(reinterpret_cast<const sensor::PacketId *>(parse_key)));
}

TEST_F(InitialState, setParseKey$WHENCalledForSingleByteDataTHENSerialReadNextAvailableMsIsCalculatedAsHardwareDelayAndTransferTimeThenStored) {
	const uint_opt8_t parse_key[2] = { sizeof(parse_key), sensor::BUTTONS };
	const uint_opt8_t HARDWARE_SERIAL_DELAY_MS = 4;
	const uint_opt8_t TRANSFER_TIME_MS = 0;
	const uint_opt8_t EXPECTED_COMPLETION_TIME_MS = (HARDWARE_SERIAL_DELAY_MS + TRANSFER_TIME_MS);
	state::setParseKey(reinterpret_cast<const sensor::PacketId *>(parse_key));
	ASSERT_EQ(EXPECTED_COMPLETION_TIME_MS, std::chrono::duration_cast<std::chrono::milliseconds>(state::testing::getSerialReadNextAvailableMs() - std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now())).count());
}

TEST_F(InitialState, setParseKey$WHENCalledForSingleByteDataTHENTransferTimeIsCalculatedAccordingToBaudRateThenStored) {
	const uint_opt8_t parse_key[2] = { sizeof(parse_key), sensor::BUTTONS };
	const uint_opt8_t HARDWARE_SERIAL_DELAY_MS = 4;
	const uint_opt8_t TRANSFER_TIME_MS = 33;
	const uint_opt8_t EXPECTED_COMPLETION_TIME_MS = (HARDWARE_SERIAL_DELAY_MS + TRANSFER_TIME_MS);
	state::setBaudCode(BAUD_300);
	state::setParseKey(reinterpret_cast<const sensor::PacketId *>(parse_key));
	ASSERT_EQ(EXPECTED_COMPLETION_TIME_MS, std::chrono::duration_cast<std::chrono::milliseconds>(state::testing::getSerialReadNextAvailableMs() - std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now())).count());
}

TEST_F(InitialState, setParseKey$WHENCalledForMultiByteDataTHENTransferTimeIsCalculatedUsingTheResultingByteSizeThenStored) {
	const uint_opt8_t parse_key[2] = { sizeof(parse_key), sensor::DISTANCE };
	const uint_opt8_t HARDWARE_SERIAL_DELAY_MS = 4;
	const uint_opt8_t TRANSFER_TIME_MS = 66;
	const uint_opt8_t EXPECTED_COMPLETION_TIME_MS = (HARDWARE_SERIAL_DELAY_MS + TRANSFER_TIME_MS);
	state::setBaudCode(BAUD_300);
	state::setParseKey(reinterpret_cast<const sensor::PacketId *>(parse_key));
	ASSERT_EQ(EXPECTED_COMPLETION_TIME_MS, std::chrono::duration_cast<std::chrono::milliseconds>(state::testing::getSerialReadNextAvailableMs() - std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now())).count());
}

TEST_F(InitialState, setParseKey$WHENCalledForGroupDataTHENTransferTimeIsCalculatedUsingTheResultingByteSizeThenStored) {
	const uint_opt8_t parse_key[2] = { sizeof(parse_key), sensor::PACKETS_17_THRU_20 };
	const uint_opt8_t HARDWARE_SERIAL_DELAY_MS = 4;
	const uint_opt8_t TRANSFER_TIME_MS = 200;
	const uint_opt8_t EXPECTED_COMPLETION_TIME_MS = (HARDWARE_SERIAL_DELAY_MS + TRANSFER_TIME_MS);
	state::setBaudCode(BAUD_300);
	state::setParseKey(reinterpret_cast<const sensor::PacketId *>(parse_key));
	ASSERT_EQ(EXPECTED_COMPLETION_TIME_MS, std::chrono::duration_cast<std::chrono::milliseconds>(state::testing::getSerialReadNextAvailableMs() - std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now())).count());
}

TEST_F(InitialState, setParseKey$WHENCalledForMultiplePacketsTHENTransferTimeIsCalculatedUsingTheResultingByteSizeThenStored) {
	const uint_opt8_t parse_key[4] = { sizeof(parse_key), sensor::BUTTONS, sensor::DISTANCE, sensor::PACKETS_17_THRU_20 };
	const uint_opt8_t HARDWARE_SERIAL_DELAY_MS = 4;
	const uint_opt16_t TRANSFER_TIME_MS = 300;
	const uint_opt16_t EXPECTED_COMPLETION_TIME_MS = (HARDWARE_SERIAL_DELAY_MS + TRANSFER_TIME_MS);
	state::setBaudCode(BAUD_300);
	state::setParseKey(reinterpret_cast<const sensor::PacketId *>(parse_key));
	ASSERT_EQ(EXPECTED_COMPLETION_TIME_MS, std::chrono::duration_cast<std::chrono::milliseconds>(state::testing::getSerialReadNextAvailableMs() - std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now())).count());
}

TEST_F(InitialState, setParseKey$WHENCalledTHENAllValuesAreConsideredDirty) {
	const uint_opt8_t parse_key[4] = { sizeof(parse_key), sensor::BUTTONS, sensor::DISTANCE, sensor::PACKETS_17_THRU_20 };
	state::setParseKey(reinterpret_cast<const sensor::PacketId *>(parse_key));
	ASSERT_EQ(static_cast<uint_opt64_t>(-1), state::testing::getFlagMaskDirty());
}

TEST_F(QueryData, parseQueryData$WHENCalledTHENValuesAreStoredInTheirRespectiveLocations) {
	ASSERT_EQ(SUCCESS, state::parseQueryData());
	const uint_opt16_t expected_cliff_front_left_signal = 0x0219;
	const uint_opt8_t expected_virtual_wall = 0x00;
	const uint_opt16_t actual_cliff_front_left_signal = convertTwoByteIntegerFromBigToLittleEndian(*reinterpret_cast<uint_opt16_t *>(state::testing::getRawData() + 30));
	const uint_opt8_t actual_virtual_wall = state::testing::getRawData()[6];
	EXPECT_EQ(expected_cliff_front_left_signal, actual_cliff_front_left_signal);
	EXPECT_EQ(expected_virtual_wall, actual_virtual_wall);
}

TEST_F(QueryData, parseQueryData$WHENCalledTHENTheParseKeyIsEmptied) {
	ASSERT_EQ(SUCCESS, state::parseQueryData());
	ASSERT_EQ(0, *state::testing::getParseKey());
}

TEST_F(QueryData, parseQueryData$WHENCalledTHENTheDirtyFlagIsUnset) {
	ASSERT_EQ(SUCCESS, state::parseQueryData());
	const uint_opt64_t flag_mask_dirty = state::testing::getFlagMaskDirty();
	EXPECT_FALSE((flag_mask_dirty >> 29 ) & 0x01 );
	EXPECT_FALSE((flag_mask_dirty >> 13 ) & 0x01 );
}

TEST_F(QueryData$ByteCountError, parseQueryData$WHENBytesReadDoNotMatchBytesRequestedTHENErrorIsReturned) {
	ASSERT_EQ(SERIAL_TRANSFER_FAILURE, state::parseQueryData());
}

TEST_F(QueryData$ByteCountError, parseQueryData$WHENBytesReadDoNotMatchBytesRequestedTHENTheParseKeyIsEmptied) {
	ASSERT_EQ(SERIAL_TRANSFER_FAILURE, state::parseQueryData());
	ASSERT_EQ(0, *state::testing::getParseKey());
}

TEST_F(QueryData$ByteCountError, parseQueryData$WHENBytesReadDoNotMatchBytesRequestedTHENTheDirtyFlagIsSet) {
	ASSERT_EQ(SERIAL_TRANSFER_FAILURE, state::parseQueryData());
	const uint_opt64_t flag_mask_dirty = state::testing::getFlagMaskDirty();
	EXPECT_TRUE((flag_mask_dirty >> 13 ) & 0x01 );
	EXPECT_TRUE((flag_mask_dirty >> 29 ) & 0x01 );
}

TEST_F(StreamData$OutOfSync, parseStreamData$WHENFirstValueIsNot19THENFailureToSyncErrorIsReturned) {
	ASSERT_EQ(FAILURE_TO_SYNC, state::parseStreamData());
}

TEST_F(StreamData, parseStreamData$WHENCalledTHENValuesAreStoredInTheirRespectiveLocations) {
	ASSERT_EQ(SUCCESS, state::parseStreamData());
	const uint_opt16_t expected_cliff_front_left_signal = 0x0219;
	const uint_opt8_t expected_virtual_wall = 0x00;
	const uint_opt16_t actual_cliff_front_left_signal = convertTwoByteIntegerFromBigToLittleEndian(*reinterpret_cast<uint_opt16_t *>(state::testing::getRawData() + 30));
	const uint_opt8_t actual_virtual_wall = state::testing::getRawData()[6];
	EXPECT_EQ(expected_cliff_front_left_signal, actual_cliff_front_left_signal);
	EXPECT_EQ(expected_virtual_wall, actual_virtual_wall);
}

TEST_F(StreamData, parseStreamData$WHENCalledTHENTheDirtyFlagIsUnset) {
	ASSERT_EQ(SUCCESS, state::parseStreamData());
	const uint_opt64_t flag_mask_dirty = state::testing::getFlagMaskDirty();
	EXPECT_FALSE((flag_mask_dirty >> 29 ) & 0x01 );
	EXPECT_FALSE((flag_mask_dirty >> 13 ) & 0x01 );
}

TEST_F(StreamData$ByteCountError, parseStreamData$WHENHeaderBytesReadDoNotMatchBytesRequestedTHENSerialTransferFailureErrorIsReturned) {
	ASSERT_EQ(SERIAL_TRANSFER_FAILURE, state::parseStreamData());
}

TEST_F(StreamData$ByteCountError, parseStreamData$WHENPacketIdBytesReadDoNotMatchBytesRequestedTHENSerialTransferFailureErrorIsReturned) {
	fail_on_call = 2;
	ASSERT_EQ(SERIAL_TRANSFER_FAILURE, state::parseStreamData());
}

TEST_F(StreamData$ByteCountError, parseStreamData$WHENDataBytesReadDoNotMatchBytesRequestedTHENSerialTransferFailureErrorIsReturned) {
	fail_on_call = 3;
	ASSERT_EQ(SERIAL_TRANSFER_FAILURE, state::parseStreamData());
}

TEST_F(StreamData$ByteCountError, parseStreamData$WHENCheckSumBytesReadDoNotMatchBytesRequestedTHENSerialTransferFailureErrorIsReturned) {
	fail_on_call = 6;
	ASSERT_EQ(SERIAL_TRANSFER_FAILURE, state::parseStreamData());
}

TEST_F(StreamData$BadCheckSum, parseStreamData$WHENCheckSumDoesNotMatchTHENInvalidChecksumErrorIsReturned) {
	ASSERT_EQ(INVALID_CHECKSUM, state::parseStreamData());
}

TEST_F(StreamData$BadCheckSum, parseStreamData$WHENCheckSumDoesNotMatchTHENTheDirtyFlagIsSetForAllBytesRead) {
	ASSERT_EQ(INVALID_CHECKSUM, state::parseStreamData());
	const uint_opt64_t flag_mask_dirty = state::testing::getFlagMaskDirty();
	EXPECT_TRUE((flag_mask_dirty >> 13 ) & 0x01 );
	EXPECT_TRUE((flag_mask_dirty >> 29 ) & 0x01 );
}

TEST_F(StreamData$ByteCountError, parseStreamData$WHENPacketIdBytesReadDoNotMatchBytesRequestedTHENTheDirtyFlagIsSetForAllBytesRead) {
	fail_on_call = 4;
	ASSERT_EQ(SERIAL_TRANSFER_FAILURE, state::parseStreamData());
	const uint_opt64_t flag_mask_dirty = state::testing::getFlagMaskDirty();
	EXPECT_TRUE((flag_mask_dirty >> 13 ) & 0x01 );
	EXPECT_TRUE((flag_mask_dirty >> 29 ) & 0x01 );
}

TEST_F(StreamData$ByteCountError, parseStreamData$WHENDataBytesReadDoNotMatchBytesRequestedTHENTheDirtyFlagIsSetForAllBytesRead) {
	fail_on_call = 5;
	ASSERT_EQ(SERIAL_TRANSFER_FAILURE, state::parseStreamData());
	const uint_opt64_t flag_mask_dirty = state::testing::getFlagMaskDirty();
	EXPECT_TRUE((flag_mask_dirty >> 13 ) & 0x01 );
	EXPECT_TRUE((flag_mask_dirty >> 29 ) & 0x01 );
}

TEST_F(StreamData$ByteCountError, parseStreamData$WHENCheckSumBytesReadDoNotMatchBytesRequestedTHENTheDirtyFlagIsSetForAllBytesRead) {
	fail_on_call = 6;
	ASSERT_EQ(SERIAL_TRANSFER_FAILURE, state::parseStreamData());
	const uint_opt64_t flag_mask_dirty = state::testing::getFlagMaskDirty();
	EXPECT_TRUE((flag_mask_dirty >> 13 ) & 0x01 );
	EXPECT_TRUE((flag_mask_dirty >> 29 ) & 0x01 );
}

} // namespace

/* Created and copyrighted by Zachary J. Fields. Offered as open source under the MIT License (MIT). */
