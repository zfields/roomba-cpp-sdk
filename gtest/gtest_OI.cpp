/* Created and copyrighted by Zachary J. Fields. All rights reserved. */

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../OI.h"

#include <cstring>

  /******************/
 /* TESTABLE CLASS */
/******************/
class OpenInterface_TC : public roomba::series500::OpenInterface {
  public:
	using roomba::series500::OpenInterface::_fnSerialWrite;
	using roomba::series500::OpenInterface::_baud_code;
	using roomba::series500::OpenInterface::_mode;
};

namespace {

  /******************/
 /* MOCK SCENARIOS */
/******************/
class DefaultInitialization : public ::testing::Test {
  protected:
	DefaultInitialization (
		void
	) {}
	
	//virtual ~Initialization() {}
	//virtual void SetUp() {}
	//virtual void TearDown() {}
	
	OpenInterface_TC OI_tc;
};

class fnSerialWriteIsAvailable : public ::testing::Test {
  protected:
	fnSerialWriteIsAvailable (
		void
	)
	{
		OI_tc.begin(
			[this] (
				const uint8_t * byte_array_,
				size_t length_
			) {
				strncpy(serial_bus, reinterpret_cast<const char *>(byte_array_), 64);
				serial_bus[63] = '\0';
				return strlen(serial_bus);
			}
		);
	}
	
	//virtual ~Initialization() {}
	//virtual void SetUp() {}
	//virtual void TearDown() {}
	
	char serial_bus[64];
	OpenInterface_TC OI_tc;
};

/*
The first argument is the name of the test case (or fixture), and the
second argument is the test's name within the test case. Both names must
be valid C++ identifiers, and they should not contain underscore (_). A
test's full name consists of its containing test case and its individual
name. Tests from different test cases can have the same individual name.
*/

TEST_F(DefaultInitialization, WHENInitializedTHENCallingFnSerialWriteWillNotThrowError) {
	OI_tc._fnSerialWrite(NULL, 0);
}

TEST_F(DefaultInitialization, WHENInitializedTHENBaudCodeWillBeSetToBAUD_115200) {
	ASSERT_EQ(roomba::series500::BAUD_115200, OI_tc._baud_code);
}

TEST_F(DefaultInitialization, WHENInitializedTHENModeWillBeSetToOFF) {
	ASSERT_EQ(roomba::series500::OFF, OI_tc._mode);
}

TEST_F(DefaultInitialization, WHENBeginIsCalledTHENFnSerialWriteIsStored) {
	ASSERT_EQ(0, OI_tc._fnSerialWrite(NULL, 0));
	OI_tc.begin([](const uint8_t *, size_t){ return 69; }, roomba::series500::BAUD_19200);
	ASSERT_EQ(69, OI_tc._fnSerialWrite(NULL, 0));
}

TEST_F(DefaultInitialization, WHENErrorIsReturnedTHENFnSerialWriteIsNotStored) {
	ASSERT_NE(roomba::series500::OpenInterface::SUCCESS, OI_tc.begin([](const uint8_t *, size_t){ return 69; }, roomba::series500::BAUD_300));
	ASSERT_EQ(0, OI_tc._fnSerialWrite(NULL, 0));
}

TEST_F(DefaultInitialization, WHENBeginIsCalledWithOneParameterTHENBaudCodeIsSetToBAUD_115200) {
	OI_tc._baud_code = roomba::series500::BAUD_19200;
	OI_tc.begin([](const uint8_t *, size_t){ return 69; });
	ASSERT_EQ(roomba::series500::BAUD_115200, OI_tc._baud_code);
}

TEST_F(DefaultInitialization, WHENBeginIsCalledWithBothParametersTHENBaudCodeIsSet) {
	OI_tc.begin([](const uint8_t *, size_t){ return 69; }, roomba::series500::BAUD_19200);
	ASSERT_EQ(roomba::series500::BAUD_19200, OI_tc._baud_code);
}

TEST_F(DefaultInitialization, WHENBeginIsCalledWithBadBaudTHENBaudIsNotStored) {
	OI_tc.begin([](const uint8_t *, size_t){ return 69; }, roomba::series500::BAUD_300);
	ASSERT_NE(roomba::series500::BAUD_300, OI_tc._baud_code);
}

TEST_F(DefaultInitialization, WHENBeginIsCalledWithBadBaudTHENErrorIsReturned) {
	ASSERT_EQ(roomba::series500::OpenInterface::INVALID_NON_OI_BAUD_RATE, OI_tc.begin([](const uint8_t *, size_t){ return 69; }, roomba::series500::BAUD_300));
}

TEST_F(fnSerialWriteIsAvailable, WHENStartIsCalledTHEN128IsWrittenToTheSerialBus) {
	OI_tc.start();
	ASSERT_EQ(128, static_cast<uint8_t>(*serial_bus));
}

TEST_F(fnSerialWriteIsAvailable, WHENStartIsCalledTHENModeIsSetToPassive) {
	OI_tc.start();
	ASSERT_EQ(roomba::series500::PASSIVE, OI_tc._mode);
}

} // namespace

/* Created and copyrighted by Zachary J. Fields. All rights reserved. */
