/* Created and copyrighted by Zachary J. Fields. All rights reserved. */

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "TESTSensors.h"

//TODO: Consider method to return multiple sensor values (std::tuple<uint_opt8_t packet_id_, uint16_t value_, bool signed_>)
//TODO: When data is out of sync, then it should pause data stream, then resume to sync.
//TODO: Check HARDWARE_SERIAL_DELAY_MS on scope
//TODO: Make HARDWARE_SERIAL_DELAY_MS a tunable variable
//TODO: Serial read next available time, needs to be incorporated into the framework

//NOTE: Considered merging begin with OICommand::connectToSerialBus() -> Decided it is better to have a distinct seperation, which will allow completely seperate modules to be written.

using namespace roomba::series500::oi;

namespace {

  /********************/
 /* HELPER FUNCTIONS */
/********************/
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
		sensors::testing::setInternalsToInitialState();
	}
	
	//virtual ~InitialState() {}
	//virtual void SetUp() {}
	//virtual void TearDown() {}
};

class DataParsed : public ::testing::Test {
  protected:
	DataParsed (
		void
	) {
		sensors::testing::setInternalsToInitialState();
	}
	
	//virtual ~DataParsed() {}
	virtual void SetUp() {
		const uint_opt8_t parse_key[1] = { 0 };
		sensors::begin([](uint_opt8_t * const, const size_t){ return 0; });
		sensors::setParseKey(reinterpret_cast<const sensors::PacketId *>(parse_key));
	}
	//virtual void TearDown() {}
};

class QueriedData : public ::testing::Test {
  protected:
	QueriedData (
		void
	) {
		sensors::testing::setInternalsToInitialState();
	}
	
	//virtual ~QueriedData() {}
	virtual void SetUp() {
		const uint_opt8_t parse_key[3] = { sizeof(parse_key), sensors::CLIFF_FRONT_LEFT_SIGNAL, sensors::VIRTUAL_WALL };
		sensors::begin(
			[] (uint_opt8_t * const buffer_, const size_t buffer_length_) {
				static uint_opt8_t serial_stream[] = { 0x02, 0x25, 0x00 };
				memcpy(buffer_, serial_stream, sizeof(buffer_length_));
				for ( uint_opt8_t i = buffer_length_ ; i < sizeof(serial_stream) ; ++i ) {
					serial_stream[(i - buffer_length_)] = serial_stream[i];
				}
				return buffer_length_;
			}
		);
		sensors::setParseKey(reinterpret_cast<const sensors::PacketId *>(parse_key));
	}
	//virtual void TearDown() {}
};

class QueriedData$ByteCountError : public ::testing::Test {
  protected:
	QueriedData$ByteCountError (
		void
	) {
		sensors::testing::setInternalsToInitialState();
	}
	
	//virtual ~QueriedData$ByteCountError() {}
	virtual void SetUp() {
		const uint_opt8_t parse_key[3] = { sizeof(parse_key), sensors::VIRTUAL_WALL, sensors::CLIFF_FRONT_LEFT_SIGNAL };
		sensors::begin(
			[] (uint_opt8_t * const buffer_, const size_t buffer_length_) {
				static uint_opt8_t serial_stream[] = { 0x00, 0x02, 0x25 };
				memcpy(buffer_, serial_stream, sizeof(buffer_length_));
				for ( uint_opt8_t i = buffer_length_ ; i < sizeof(serial_stream) ; ++i ) {
					serial_stream[(i - buffer_length_)] = serial_stream[i];
				}
				return 1;
			}
		);
		sensors::setParseKey(reinterpret_cast<const sensors::PacketId *>(parse_key));
	}
	//virtual void TearDown() {}
};

class StreamingData : public ::testing::Test {
  protected:
	StreamingData (
		void
	) {
		sensors::testing::setInternalsToInitialState();
	}
	
	//virtual ~StreamingData() {}
	virtual void SetUp() {
		sensors::begin(
			[] (uint_opt8_t * const buffer_, const size_t buffer_length_) {
				static uint_opt8_t serial_stream[] = { 0x13, 0x05, 0x1D, 0x02, 0x25, 0x0D, 0x00, 0xB6 };
				memcpy(buffer_, serial_stream, sizeof(buffer_length_));
				for ( uint_opt8_t i = buffer_length_ ; i < sizeof(serial_stream) ; ++i ) {
					serial_stream[(i - buffer_length_)] = serial_stream[i];
				}
				return buffer_length_;
			}
		);
	}
	//virtual void TearDown() {}
};

class StreamingData$BadCheckSum : public ::testing::Test {
  protected:
	StreamingData$BadCheckSum (
		void
	) {
		sensors::testing::setInternalsToInitialState();
	}
	
	//virtual ~StreamingData$BadCheckSum() {}
	virtual void SetUp() {
		sensors::begin(
			[] (uint_opt8_t * const buffer_, const size_t buffer_length_) {
				static uint_opt8_t serial_stream[] = { 0x13, 0x05, 0x1D, 0x02, 0x25, 0x0D, 0x00, 0xBE };
				memcpy(buffer_, serial_stream, sizeof(buffer_length_));
				for ( uint_opt8_t i = buffer_length_ ; i < sizeof(serial_stream) ; ++i ) {
					serial_stream[(i - buffer_length_)] = serial_stream[i];
				}
				return buffer_length_;
			}
		);
	}
	//virtual void TearDown() {}
};

class StreamingData$Paused : public ::testing::Test {
  protected:
	StreamingData$Paused (
		void
	) {
		sensors::testing::setInternalsToInitialState();
	}
	
	//virtual ~StreamingData$Paused() {}
	virtual void SetUp() {
		sensors::begin(
			[] (uint_opt8_t * const buffer_, const size_t buffer_length_) {
				static uint_opt8_t serial_stream[] = { 0x13, 0x05, 0x1D, 0x02, 0x25, 0x0D };
				memcpy(buffer_, serial_stream, sizeof(buffer_length_));
				for ( uint_opt8_t i = buffer_length_ ; i < sizeof(serial_stream) ; ++i ) {
					serial_stream[(i - buffer_length_)] = serial_stream[i];
				}
				return buffer_length_;
			}
		);
	}
	//virtual void TearDown() {}
};

class StreamingData$OutOfSync : public ::testing::Test {
  protected:
	StreamingData$OutOfSync (
		void
	) {
		sensors::testing::setInternalsToInitialState();
	}
	
	//virtual ~StreamingData$OutOfSync() {}
	virtual void SetUp() {
		sensors::begin(
			[] (uint_opt8_t * const buffer_, const size_t buffer_length_) {
				static uint_opt8_t serial_stream[] = { 0x25, 0x0D, 0x00, 0xB6, 0x13, 0x05, 0x1D, 0x02, 0x25, 0x0D, 0x00, 0xB6, 0x13, 0x05, 0x1D };
				memcpy(buffer_, serial_stream, sizeof(buffer_length_));
				for ( uint_opt8_t i = buffer_length_ ; i < sizeof(serial_stream) ; ++i ) {
					serial_stream[(i - buffer_length_)] = serial_stream[i];
				}
				return buffer_length_;
			}
		);
	}
	//virtual void TearDown() {}
};

/*
The first argument is the name of the test case (or fixture), and the
second argument is the test's name within the test case. Both names must
be valid C++ identifiers, and they should not contain underscore (_). A
test's full name consists of its containing test case and its individual
name. Tests from different test cases can have the same individual name.
(e.g. ASSERT_EQ(_EXPECTED_, _ACTUAL_))
*/

TEST_F(InitialState, begin$WHENBeginHasNotBeenCalledTHENfnSerialReadReturnsZero) {
	ASSERT_EQ(0, sensors::testing::fnSerialRead(NULL, 0));
}

TEST_F(InitialState, begin$WHENBeginIsCalledTHENfnSerialReadIsStored) {
	ASSERT_EQ(0, sensors::testing::fnSerialRead(NULL, 0));
	sensors::begin([](uint_opt8_t * const, const size_t){ return 7; });
	ASSERT_EQ(7, sensors::testing::fnSerialRead(NULL, 0));
}

TEST_F(InitialState, begin$WHENBeginIsCalledAgainWithoutCallingEndTHENErrorIsReturned) {
	ASSERT_EQ(0, sensors::testing::fnSerialRead(NULL, 0));
	sensors::begin([](uint_opt8_t * const, const size_t){ return 7; });
	ASSERT_EQ(7, sensors::testing::fnSerialRead(NULL, 0));
	ASSERT_EQ(sensors::INVALID_MODE_FOR_REQUESTED_OPERATION, sensors::begin([](uint_opt8_t * const, const size_t){ return 8; }));
}

TEST_F(InitialState, end$WHENEndIsCalledTHENfnSerialReadReturnsZero) {
	sensors::begin([](uint_opt8_t * const, const size_t){ return 8; });
	ASSERT_EQ(8, sensors::testing::fnSerialRead(NULL, 0));
	sensors::end();
	ASSERT_EQ(0, sensors::testing::fnSerialRead(NULL, 0));
}

TEST_F(InitialState, end$WHENEndIsCalledTHENBeginCanBeCalledAgain) {
	sensors::begin([](uint_opt8_t * const, const size_t){ return 7; });
	ASSERT_EQ(7, sensors::testing::fnSerialRead(NULL, 0));
	sensors::end();
	ASSERT_EQ(sensors::SUCCESS, sensors::begin([](uint_opt8_t * const, const size_t){ return 8; }));
}

TEST_F(InitialState, setBaudCode$WHENCalledTHENBaudCodeIsSet) {
	ASSERT_EQ(BAUD_115200, sensors::testing::getBaudCode());
	sensors::setBaudCode(BAUD_57600);
	ASSERT_EQ(BAUD_57600, sensors::testing::getBaudCode());
}

TEST_F(InitialState, setBaudCode$WHENBaudCodeIsGreaterThan11THENParameterIsInvalid) {
	for ( int i = 12 ; i <= 255 ; ++i ) {
		EXPECT_EQ(sensors::INVALID_PARAMETER, sensors::setBaudCode(static_cast<BaudCode>(i))) << "Accepted value <" << static_cast<unsigned int>(i) << ">!";
	}
}

TEST_F(InitialState, setBaudCode$WHENParameterIsInvalidTHENBaudCodeIsNotSet) {
	for ( int i = 12 ; i <= 255 ; ++i ) {
		EXPECT_EQ(sensors::INVALID_PARAMETER, sensors::setBaudCode(static_cast<BaudCode>(i)));
		ASSERT_EQ(BAUD_115200, sensors::testing::getBaudCode());
	}
}

TEST_F(InitialState, setParseKey$WHENBeforeCallTHENParseKeyIsNotSet) {
	ASSERT_EQ(0, *reinterpret_cast<uint_opt8_t *>(sensors::testing::getParseKey()));
}

TEST_F(InitialState, setParseKey$WHENCalledTHENParseKeyIsSet) {
	const uint_opt8_t parse_key[2] = { sizeof(parse_key), sensors::BUTTONS };
	sensors::setParseKey(reinterpret_cast<const sensors::PacketId *>(parse_key));
	ASSERT_EQ(2, *reinterpret_cast<uint_opt8_t *>(sensors::testing::getParseKey()));
	ASSERT_EQ(sensors::BUTTONS, *reinterpret_cast<uint_opt8_t *>(sensors::testing::getParseKey() + 1));
}

TEST_F(InitialState, setParseKey$WHENCalledWithNULLTHENErrorIsReturned) {
	ASSERT_EQ(sensors::INVALID_PARAMETER, sensors::setParseKey(NULL));
}

TEST_F(InitialState, setParseKey$WHENCalledWithZeroSizeTHENErrorIsReturned) {
	const uint_opt8_t parse_key[2] = { 0, sensors::BUTTONS };
	ASSERT_EQ(sensors::INVALID_PARAMETER, sensors::setParseKey(reinterpret_cast<const sensors::PacketId *>(parse_key)));
}

TEST_F(InitialState, setParseKey$WHENCalledForSingleByteDataTHENSerialReadNextAvailableMsIsCalculatedAsHardwareDelayAndTransferTimeThenStored) {
	const uint_opt8_t parse_key[2] = { sizeof(parse_key), sensors::BUTTONS };
	const uint_opt8_t HARDWARE_SERIAL_DELAY_MS = 4;
	const uint_opt8_t TRANSFER_TIME_MS = 0;
	const uint_opt8_t EXPECTED_COMPLETION_TIME_MS = (HARDWARE_SERIAL_DELAY_MS + TRANSFER_TIME_MS);
	sensors::setParseKey(reinterpret_cast<const sensors::PacketId *>(parse_key));
	ASSERT_EQ(EXPECTED_COMPLETION_TIME_MS, std::chrono::duration_cast<std::chrono::milliseconds>(sensors::testing::getSerialReadNextAvailableMs() - std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now())).count());
}

TEST_F(InitialState, setParseKey$WHENCalledForSingleByteDataTHENTransferTimeIsCalculatedAccordingToBaudRateThenStored) {
	const uint_opt8_t parse_key[2] = { sizeof(parse_key), sensors::BUTTONS };
	const uint_opt8_t HARDWARE_SERIAL_DELAY_MS = 4;
	const uint_opt8_t TRANSFER_TIME_MS = 33;
	const uint_opt8_t EXPECTED_COMPLETION_TIME_MS = (HARDWARE_SERIAL_DELAY_MS + TRANSFER_TIME_MS);
	sensors::setBaudCode(BAUD_300);
	sensors::setParseKey(reinterpret_cast<const sensors::PacketId *>(parse_key));
	ASSERT_EQ(EXPECTED_COMPLETION_TIME_MS, std::chrono::duration_cast<std::chrono::milliseconds>(sensors::testing::getSerialReadNextAvailableMs() - std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now())).count());
}

TEST_F(InitialState, setParseKey$WHENCalledForMultiByteDataTHENTransferTimeIsCalculatedUsingTheResultingByteSizeThenStored) {
	const uint_opt8_t parse_key[2] = { sizeof(parse_key), sensors::DISTANCE };
	const uint_opt8_t HARDWARE_SERIAL_DELAY_MS = 4;
	const uint_opt8_t TRANSFER_TIME_MS = 66;
	const uint_opt8_t EXPECTED_COMPLETION_TIME_MS = (HARDWARE_SERIAL_DELAY_MS + TRANSFER_TIME_MS);
	sensors::setBaudCode(BAUD_300);
	sensors::setParseKey(reinterpret_cast<const sensors::PacketId *>(parse_key));
	ASSERT_EQ(EXPECTED_COMPLETION_TIME_MS, std::chrono::duration_cast<std::chrono::milliseconds>(sensors::testing::getSerialReadNextAvailableMs() - std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now())).count());
}

TEST_F(InitialState, setParseKey$WHENCalledForGroupDataTHENTransferTimeIsCalculatedUsingTheResultingByteSizeThenStored) {
	const uint_opt8_t parse_key[2] = { sizeof(parse_key), sensors::PACKETS_17_THRU_20 };
	const uint_opt8_t HARDWARE_SERIAL_DELAY_MS = 4;
	const uint_opt8_t TRANSFER_TIME_MS = 200;
	const uint_opt8_t EXPECTED_COMPLETION_TIME_MS = (HARDWARE_SERIAL_DELAY_MS + TRANSFER_TIME_MS);
	sensors::setBaudCode(BAUD_300);
	sensors::setParseKey(reinterpret_cast<const sensors::PacketId *>(parse_key));
	ASSERT_EQ(EXPECTED_COMPLETION_TIME_MS, std::chrono::duration_cast<std::chrono::milliseconds>(sensors::testing::getSerialReadNextAvailableMs() - std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now())).count());
}

TEST_F(InitialState, setParseKey$WHENCalledForMultiplePacketsTHENTransferTimeIsCalculatedUsingTheResultingByteSizeThenStored) {
	const uint_opt8_t parse_key[4] = { sizeof(parse_key), sensors::BUTTONS, sensors::DISTANCE, sensors::PACKETS_17_THRU_20 };
	const uint_opt8_t HARDWARE_SERIAL_DELAY_MS = 4;
	const uint_opt16_t TRANSFER_TIME_MS = 300;
	const uint_opt16_t EXPECTED_COMPLETION_TIME_MS = (HARDWARE_SERIAL_DELAY_MS + TRANSFER_TIME_MS);
	sensors::setBaudCode(BAUD_300);
	sensors::setParseKey(reinterpret_cast<const sensors::PacketId *>(parse_key));
	ASSERT_EQ(EXPECTED_COMPLETION_TIME_MS, std::chrono::duration_cast<std::chrono::milliseconds>(sensors::testing::getSerialReadNextAvailableMs() - std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now())).count());
}

TEST_F(InitialState, setParseKey$WHENCalledTHENAllValuesAreConsideredDirty) {
	const uint_opt8_t parse_key[4] = { sizeof(parse_key), sensors::BUTTONS, sensors::DISTANCE, sensors::PACKETS_17_THRU_20 };
	sensors::setParseKey(reinterpret_cast<const sensors::PacketId *>(parse_key));
	ASSERT_EQ(static_cast<uint_opt64_t>(-1), sensors::testing::getFlagMaskDirty());
}

TEST_F(QueriedData, parseQueriedData$WHENCalledTHENValuesAreStoredInTheirRespectiveLocations) {
	ASSERT_EQ(sensors::SUCCESS, sensors::parseQueriedData());
	const uint_opt16_t expected_cliff_front_left_signal = 0x0225;
	const uint_opt8_t expected_virtual_wall = 0x00;
	const uint_opt16_t actual_cliff_front_left_signal = convertTwoByteIntegerFromBigToLittleEndian(*reinterpret_cast<uint_opt16_t *>(sensors::testing::getRawData() + 30));
	const uint_opt8_t actual_virtual_wall = sensors::testing::getRawData()[6];
	EXPECT_EQ(expected_cliff_front_left_signal, actual_cliff_front_left_signal);
	EXPECT_EQ(expected_virtual_wall, actual_virtual_wall);
}

TEST_F(QueriedData, parseQueriedData$WHENCalledTHENTheParseKeyIsEmptied) {
	ASSERT_EQ(sensors::SUCCESS, sensors::parseQueriedData());
	ASSERT_EQ(0, *sensors::testing::getParseKey());
}

TEST_F(QueriedData, parseQueriedData$WHENCalledTHENTheDirtyFlagIsUnset) {
	ASSERT_EQ(sensors::SUCCESS, sensors::parseQueriedData());
	const uint_opt64_t flag_mask_dirty = sensors::testing::getFlagMaskDirty();
	EXPECT_FALSE((flag_mask_dirty >> 29 ) & 0x01 );
	EXPECT_FALSE((flag_mask_dirty >> 13 ) & 0x01 );
}

TEST_F(QueriedData$ByteCountError, parseQueriedData$WHENBytesReadDoNotMatchBytesRequestedTHENErrorIsReturned) {
	ASSERT_EQ(sensors::SERIAL_TRANSFER_FAILURE, sensors::parseQueriedData());
}

TEST_F(QueriedData$ByteCountError, parseQueriedData$WHENBytesReadDoNotMatchBytesRequestedTHENTheParseKeyIsEmptied) {
	ASSERT_EQ(sensors::SERIAL_TRANSFER_FAILURE, sensors::parseQueriedData());
	ASSERT_EQ(0, *sensors::testing::getParseKey());
}

TEST_F(QueriedData$ByteCountError, parseQueriedData$WHENBytesReadDoNotMatchBytesRequestedTHENTheDirtyFlagIsSet) {
	ASSERT_EQ(sensors::SERIAL_TRANSFER_FAILURE, sensors::parseQueriedData());
	const uint_opt64_t flag_mask_dirty = sensors::testing::getFlagMaskDirty();
	EXPECT_TRUE((flag_mask_dirty >> 13 ) & 0x01 );
	EXPECT_TRUE((flag_mask_dirty >> 29 ) & 0x01 );
}

TEST_F(InitialState, valueOfSensor$WHENBeginHasNotBeenCalledTHENReturnsError) {
	uint_opt16_t value;
	bool is_signed;
	ASSERT_EQ(sensors::SERIAL_TRANSFER_FAILURE, sensors::valueOfSensor(sensors::OI_MODE, &value, &is_signed));
}

TEST_F(QueriedData, valueOfSensor$WHENCalledWithNullValueParameterTHENReturnsError) {
	uint_opt16_t *value(NULL);
	bool is_signed;
	ASSERT_EQ(sensors::INVALID_PARAMETER, sensors::valueOfSensor(sensors::OI_MODE, value, &is_signed));
}

TEST_F(QueriedData, valueOfSensor$WHENCalledWithNullIsSignedParameterTHENReturnsError) {
	uint_opt16_t value;
	bool *is_signed(NULL);
	ASSERT_EQ(sensors::INVALID_PARAMETER, sensors::valueOfSensor(sensors::OI_MODE, &value, is_signed));
}

TEST_F(QueriedData, valueOfSensor$WHENCalledWithInvalidSensorNameTHENReturnsError) {
	uint_opt16_t value;
	bool is_signed;
	
	for ( uint_opt8_t i = 0 ; i < 7 ; ++i ) {
		EXPECT_EQ(sensors::INVALID_PARAMETER, sensors::valueOfSensor(static_cast<sensors::PacketId>(i), &value, &is_signed)) << "Accepted value <" << static_cast<unsigned int>(i) << ">!";
	}
	for ( int i = 59 ; i <= 255 ; ++i ) {
		EXPECT_EQ(sensors::INVALID_PARAMETER, sensors::valueOfSensor(static_cast<sensors::PacketId>(i), &value, &is_signed)) << "Accepted value <" << static_cast<unsigned int>(i) << ">!";
	}
}

TEST_F(QueriedData, valueOfSensor$WHENCalledTHENSignedParameterReturnsCorrectValue) {
	uint_opt64_t FLAG_MASK_SIGNED = sensors::testing::getFlagMaskSigned();
	uint_opt16_t value;
	bool is_signed;
	
	for ( int i = 7 ; i < 59 ; ++i ) {
		sensors::valueOfSensor(static_cast<sensors::PacketId>(i), &value, &is_signed);
		EXPECT_EQ(((FLAG_MASK_SIGNED >> i) & 0x01), is_signed) << "Tested value <" << static_cast<unsigned int>(i) << ">!";
	}
}
/* FINISH PARSING TEST FIRST
TEST_F(QueriedData, valueOfSensor$WHENCalledForEightBitDataTHENReturnsCorrectValue) {
	uint_opt16_t value;
	bool is_signed;
	
	for ( uint_opt8_t i = 7 ; i < 59 ; ++i ) {
		sensors::valueOfSensor(static_cast<sensors::PacketId>(i), &value, &is_signed);
		EXPECT_EQ(, value);
	}
}
*/
} // namespace

/* Created and copyrighted by Zachary J. Fields. All rights reserved. */
