/* Created and copyrighted by Zachary J. Fields. All rights reserved. */

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../OISensors.h"

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

class BeginCalled_QueriedData : public ::testing::Test {
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

class BeginCalled_StreamingData : public ::testing::Test {
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

/*
The first argument is the name of the test case (or fixture), and the
second argument is the test's name within the test case. Both names must
be valid C++ identifiers, and they should not contain underscore (_). A
test's full name consists of its containing test case and its individual
name. Tests from different test cases can have the same individual name.
(e.g. ASSERT_EQ(_EXPECTED_, _ACTUAL_))
*/

TEST_F(BeginNotCalled, begin$WHENBeginHasNotBeenCalledTHENParseSerialDataReturnsError) {
	ASSERT_EQ(sensors::SERIAL_TRANSFER_FAILURE, sensors::parseSerialData());
}

TEST_F(BeginNotCalled, begin$WHENBeginHasNotBeenCalledTHENValueOfSensorReturnsError) {
	uint16_t value;
	bool is_signed;
	ASSERT_EQ(sensors::SERIAL_TRANSFER_FAILURE, sensors::valueOfSensor(sensors::OI_MODE, &value, &is_signed));
}


} // namespace

/* Created and copyrighted by Zachary J. Fields. All rights reserved. */
