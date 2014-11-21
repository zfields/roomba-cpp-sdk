/* Created and copyrighted by Zachary J. Fields. All rights reserved. */

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "TESTSensors.h"

//TODO: Consider method to return multiple sensor values (std::tuple<uint8_t packet_id_, uint16_t value_, bool signed_>)
//TODO: When data is out of sync, then it should pause data stream, then resume to sync.

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
			[] (uint8_t * const buffer_, const size_t buffer_length_) {
				uint8_t serial_stream[] = "Hello, World!";
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
			[] (uint8_t * const buffer_, const size_t buffer_length_) {
				uint8_t serial_stream[] = { 0x02, 0x19, 0x00 };
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
			[] (uint8_t * const buffer_, const size_t buffer_length_) {
				uint8_t serial_stream[] = { 0x02, 0x19, 0x00 };
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
			[] (uint8_t * const buffer_, const size_t buffer_length_) {
				uint8_t serial_stream[] = { 0x13, 0x05, 0x1D, 0x02, 0x19, 0x0D, 0x00, 0xB6 };
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
			[] (uint8_t * const buffer_, const size_t buffer_length_) {
				uint8_t serial_stream[] = { 0x13, 0x05, 0x1D, 0x02, 0x19, 0x0D, 0x00, 0xBE };
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
			[] (uint8_t * const buffer_, const size_t buffer_length_) {
				uint8_t serial_stream[] = { 0x19, 0x0D, 0x00, 0xB6, 0x13, 0x05, 0x1D, 0x02, 0x19, 0x0D, 0x00, 0xB6, 0x13, 0x05, 0x1D };
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
	sensors::begin([](uint8_t * const, const size_t){ return 7; });
	ASSERT_EQ(7, sensors::testing::fnSerialRead(NULL, 0));
}
/*
TEST_F(QueriedData, sensors$WHENBeforeCallTHENParseKeyIsNotSet) {
	ASSERT_EQ(0, *reinterpret_cast<uint8_t *>(sensors::testing::getParseKey()));
}

TEST_F(QueriedData, sensors$WHENCalledTHENParseKeyIsSet) {
	ASSERT_EQ(1, sensors::test::getParseKey());
}
*/
TEST_F(BeginNotCalled, valueOfSensor$WHENBeginHasNotBeenCalledTHENReturnsError) {
	uint16_t value;
	bool is_signed;
	ASSERT_EQ(sensors::SERIAL_TRANSFER_FAILURE, sensors::valueOfSensor(sensors::OI_MODE, &value, &is_signed));
}

} // namespace

/* Created and copyrighted by Zachary J. Fields. All rights reserved. */
