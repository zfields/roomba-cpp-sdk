/* Created and copyrighted by Zachary J. Fields. All rights reserved. */

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "TESTSensors.h"

//TODO: Consider method to return multiple sensor values (std::tuple<uint_opt8_t packet_id_, uint16_t value_, bool signed_>)
//TODO: When data is out of sync, then it should pause data stream, then resume to sync.
//TODO: Consider merging begin with OICommand::connectToSerialBus()
//TODO: Check HARDWARE_SERIAL_DELAY_MS on scope
//TODO: Make HARDWARE_SERIAL_DELAY_MS a tunable variable

using namespace roomba::series500::oi;

namespace {

  /******************/
 /* MOCK SCENARIOS */
/******************/
class BeginNotCalled : public ::testing::Test {
  protected:
	//Initialization (
	//	void
	//) {}
	
	//virtual ~Initialization() {}
	//virtual void SetUp() {}
	//virtual void TearDown() {}	
};

class BeginCalled : public ::testing::Test {
  protected:
	//Initialization (
	//	void
	//) {}
	
	//virtual ~Initialization() {}
	virtual void SetUp() {
		sensors::begin(
			[] (uint_opt8_t * const buffer_, const size_t buffer_length_) {
				uint_opt8_t serial_stream[] = "Hello, World!";
				memcpy(serial_stream, buffer_, sizeof(serial_stream));
				return (sizeof(serial_stream));
			}
		);
	}
	//virtual void TearDown() {}
};

class EndCalled : public ::testing::Test {
  protected:
	//Initialization (
	//	void
	//) {}
	
	//virtual ~Initialization() {}
	virtual void SetUp() {
		sensors::begin(
			[] (uint_opt8_t * const buffer_, const size_t buffer_length_) {
				uint_opt8_t serial_stream[] = { 0x02, 0x19, 0x00 };
				memcpy(serial_stream, buffer_, sizeof(serial_stream));
				return (sizeof(serial_stream));
			}
		);
	}
	//virtual void TearDown() {}
};

class QueriedData : public ::testing::Test {
  protected:
	//Initialization (
	//	void
	//) {}
	
	//virtual ~Initialization() {}
	virtual void SetUp() {
		sensors::begin(
			[] (uint_opt8_t * const buffer_, const size_t buffer_length_) {
				uint_opt8_t serial_stream[] = { 0x02, 0x19, 0x00 };
				memcpy(serial_stream, buffer_, sizeof(serial_stream));
				return (sizeof(serial_stream));
			}
		);
	}
	//virtual void TearDown() {}
};

class StreamingData : public ::testing::Test {
  protected:
	//Initialization (
	//	void
	//) {}
	
	//virtual ~Initialization() {}
	virtual void SetUp() {
		sensors::begin(
			[] (uint_opt8_t * const buffer_, const size_t buffer_length_) {
				uint_opt8_t serial_stream[] = { 0x13, 0x05, 0x1D, 0x02, 0x19, 0x0D, 0x00, 0xB6 };
				memcpy(serial_stream, buffer_, sizeof(serial_stream));
				return (sizeof(serial_stream));
			}
		);
	}
	//virtual void TearDown() {}
};

class StreamingData$BadCheckSum : public ::testing::Test {
  protected:
	//Initialization (
	//	void
	//) {}
	
	//virtual ~Initialization() {}
	virtual void SetUp() {
		sensors::begin(
			[] (uint_opt8_t * const buffer_, const size_t buffer_length_) {
				uint_opt8_t serial_stream[] = { 0x13, 0x05, 0x1D, 0x02, 0x19, 0x0D, 0x00, 0xBE };
				memcpy(serial_stream, buffer_, sizeof(serial_stream));
				return (sizeof(serial_stream));
			}
		);
	}
	//virtual void TearDown() {}
};

class StreamingData$OutOfSync : public ::testing::Test {
  protected:
	//Initialization (
	//	void
	//) {}
	
	//virtual ~Initialization() {}
	virtual void SetUp() {
		sensors::begin(
			[] (uint_opt8_t * const buffer_, const size_t buffer_length_) {
				uint_opt8_t serial_stream[] = { 0x19, 0x0D, 0x00, 0xB6, 0x13, 0x05, 0x1D, 0x02, 0x19, 0x0D, 0x00, 0xB6, 0x13, 0x05, 0x1D };
				memcpy(serial_stream, buffer_, sizeof(serial_stream));
				return (sizeof(serial_stream));
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

TEST_F(BeginNotCalled, begin$WHENBeginHasNotBeenCalledTHENfnSerialReadReturnsZero) {
	ASSERT_EQ(0, sensors::testing::fnSerialRead(NULL, 0));
}

TEST_F(BeginNotCalled, begin$WHENBeginIsCalledTHENfnSerialReadIsStored) {
	ASSERT_EQ(0, sensors::testing::fnSerialRead(NULL, 0));
	sensors::begin([](uint_opt8_t * const, const size_t){ return 7; });
	ASSERT_EQ(7, sensors::testing::fnSerialRead(NULL, 0));
}

TEST_F(StreamingData, end$WHENEndIsCalledTHENfnSerialReadReturnsZero) {
	uint_opt8_t buffer[32] = { 0 };
	ASSERT_EQ(8, sensors::testing::fnSerialRead(buffer, sizeof(buffer)));
	sensors::end();
	ASSERT_EQ(0, sensors::testing::fnSerialRead(buffer, sizeof(buffer)));
}

TEST_F(QueriedData, setBaudCode$WHENCalledTHENBaudCodeIsSet) {
	ASSERT_EQ(BAUD_115200, sensors::testing::getBaudCode());
	sensors::setBaudCode(BAUD_57600);
	ASSERT_EQ(BAUD_57600, sensors::testing::getBaudCode());
}

TEST_F(QueriedData, setParseKey$WHENBeforeCallTHENParseKeyIsNotSet) {
	ASSERT_EQ(0, *reinterpret_cast<uint_opt8_t *>(sensors::testing::getParseKey()));
}

TEST_F(QueriedData, setParseKey$WHENCalledTHENParseKeyIsSet) {
	const uint_opt8_t parse_key[2] = { sizeof(parse_key), sensors::BUTTONS };
	sensors::setParseKey(reinterpret_cast<const sensors::PacketId *>(parse_key));
	ASSERT_EQ(2, *reinterpret_cast<uint_opt8_t *>(sensors::testing::getParseKey()));
	ASSERT_EQ(sensors::BUTTONS, *reinterpret_cast<uint_opt8_t *>(sensors::testing::getParseKey() + 1));
}

TEST_F(QueriedData, setParseKey$WHENCalledWithNULLTHENErrorIsReturned) {
	ASSERT_EQ(sensors::INVALID_PARAMETER, sensors::setParseKey(NULL));
}

TEST_F(QueriedData, setParseKey$WHENCalledForSingleByteDataTHENTransferCompletionTimeMsIsCalculatedAsHardwareDelayAndTransferTimeThenStored) {
	const uint_opt8_t parse_key[2] = { sizeof(parse_key), sensors::BUTTONS };
	const uint_opt8_t HARDWARE_SERIAL_DELAY_MS = 4;
	const uint_opt8_t TRANSFER_TIME_MS = 0;
	const uint_opt8_t EXPECTED_COMPLETION_TIME_MS = (HARDWARE_SERIAL_DELAY_MS + TRANSFER_TIME_MS);
	sensors::setParseKey(reinterpret_cast<const sensors::PacketId *>(parse_key));
	ASSERT_EQ(EXPECTED_COMPLETION_TIME_MS, std::chrono::duration_cast<std::chrono::milliseconds>(sensors::testing::getTransferCompletionTimeMs() - std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now())).count());
}

TEST_F(QueriedData, setParseKey$WHENCalledForSingleByteDataTHENTransferTimeIsCalculatedAccordingToBaudRateThenStored) {
	const uint_opt8_t parse_key[2] = { sizeof(parse_key), sensors::BUTTONS };
	const uint_opt8_t HARDWARE_SERIAL_DELAY_MS = 4;
	const uint_opt8_t TRANSFER_TIME_MS = 33;
	const uint_opt8_t EXPECTED_COMPLETION_TIME_MS = (HARDWARE_SERIAL_DELAY_MS + TRANSFER_TIME_MS);
	sensors::setBaudCode(BAUD_300);
	sensors::setParseKey(reinterpret_cast<const sensors::PacketId *>(parse_key));
	ASSERT_EQ(EXPECTED_COMPLETION_TIME_MS, std::chrono::duration_cast<std::chrono::milliseconds>(sensors::testing::getTransferCompletionTimeMs() - std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now())).count());
}

TEST_F(QueriedData, setParseKey$WHENCalledForMultiByteDataTHENTransferTimeIsCalculatedUsingTheResultingByteSizeThenStored) {
	const uint_opt8_t parse_key[2] = { sizeof(parse_key), sensors::DISTANCE };
	const uint_opt8_t HARDWARE_SERIAL_DELAY_MS = 4;
	const uint_opt8_t TRANSFER_TIME_MS = 66;
	const uint_opt8_t EXPECTED_COMPLETION_TIME_MS = (HARDWARE_SERIAL_DELAY_MS + TRANSFER_TIME_MS);
	sensors::setBaudCode(BAUD_300);
	sensors::setParseKey(reinterpret_cast<const sensors::PacketId *>(parse_key));
	ASSERT_EQ(EXPECTED_COMPLETION_TIME_MS, std::chrono::duration_cast<std::chrono::milliseconds>(sensors::testing::getTransferCompletionTimeMs() - std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now())).count());
}

TEST_F(QueriedData, setParseKey$WHENCalledForGroupDataTHENTransferTimeIsCalculatedUsingTheResultingByteSizeThenStored) {
	const uint_opt8_t parse_key[2] = { sizeof(parse_key), sensors::PACKETS_17_THRU_20 };
	const uint_opt8_t HARDWARE_SERIAL_DELAY_MS = 4;
	const uint_opt8_t TRANSFER_TIME_MS = 200;
	const uint_opt8_t EXPECTED_COMPLETION_TIME_MS = (HARDWARE_SERIAL_DELAY_MS + TRANSFER_TIME_MS);
	sensors::setBaudCode(BAUD_300);
	sensors::setParseKey(reinterpret_cast<const sensors::PacketId *>(parse_key));
	ASSERT_EQ(EXPECTED_COMPLETION_TIME_MS, std::chrono::duration_cast<std::chrono::milliseconds>(sensors::testing::getTransferCompletionTimeMs() - std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now())).count());
}

TEST_F(QueriedData, setParseKey$WHENCalledForMultiplePacketsTHENTransferTimeIsCalculatedUsingTheResultingByteSizeThenStored) {
	const uint_opt8_t parse_key[4] = { sizeof(parse_key), sensors::BUTTONS, sensors::DISTANCE, sensors::PACKETS_17_THRU_20 };
	const uint_opt8_t HARDWARE_SERIAL_DELAY_MS = 4;
	const uint_opt16_t TRANSFER_TIME_MS = 300;
	const uint_opt16_t EXPECTED_COMPLETION_TIME_MS = (HARDWARE_SERIAL_DELAY_MS + TRANSFER_TIME_MS);
	sensors::setBaudCode(BAUD_300);
	sensors::setParseKey(reinterpret_cast<const sensors::PacketId *>(parse_key));
	ASSERT_EQ(EXPECTED_COMPLETION_TIME_MS, std::chrono::duration_cast<std::chrono::milliseconds>(sensors::testing::getTransferCompletionTimeMs() - std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now())).count());
}

TEST_F(BeginNotCalled, valueOfSensor$WHENBeginHasNotBeenCalledTHENReturnsError) {
	uint16_t value;
	bool is_signed;
	ASSERT_EQ(sensors::SERIAL_TRANSFER_FAILURE, sensors::valueOfSensor(sensors::OI_MODE, &value, &is_signed));
}

} // namespace

/* Created and copyrighted by Zachary J. Fields. All rights reserved. */
