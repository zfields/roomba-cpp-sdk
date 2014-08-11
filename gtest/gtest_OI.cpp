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

class FailedSerialTransaction : public ::testing::Test {
  protected:
	FailedSerialTransaction (
		void
	) {
		OI_tc._mode = roomba::series500::PASSIVE;
		OI_tc.connectToSerialBus([](const uint8_t *, size_t){ return 0; });
	}
	
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
		OI_tc.connectToSerialBus(
			[this] (
				const uint8_t * byte_array_,
				size_t length_
			) {
				length_ = ((length_ <= 64) * length_) + ((length_ > 64) * 64);
				memcpy(serial_bus, byte_array_, length_);
				return strnlen(serial_bus, 64);
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

TEST_F(DefaultInitialization, constructor$WHENInitializedTHENCallingFnSerialWriteWillNotThrowError) {
	OI_tc._fnSerialWrite(NULL, 0);
}

TEST_F(DefaultInitialization, constructor$WHENInitializedTHENModeWillBeSetToOFF) {
	ASSERT_EQ(roomba::series500::OFF, OI_tc._mode);
}

TEST_F(DefaultInitialization, connectToSerialBus$WHENCalledTHENFnSerialWriteIsStored) {
	ASSERT_EQ(0, OI_tc._fnSerialWrite(NULL, 0));
	OI_tc.connectToSerialBus([](const uint8_t *, size_t){ return 69; });
	ASSERT_EQ(69, OI_tc._fnSerialWrite(NULL, 0));
}

TEST_F(fnSerialWriteIsAvailable, start$WHENCalledTHEN128IsWrittenToTheSerialBus) {
	OI_tc.start();
	ASSERT_EQ(roomba::series500::command::START, static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(fnSerialWriteIsAvailable, start$WHENCalledTHENModeIsSetToPassive) {
	OI_tc.start();
	ASSERT_EQ(roomba::series500::PASSIVE, OI_tc._mode);
}

TEST_F(FailedSerialTransaction, start$WHENfnSerialWriteFailsTHENReturnsError) {
	ASSERT_EQ(roomba::series500::OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.start());
}

TEST_F(fnSerialWriteIsAvailable, baud$WHENCalledTHEN129AndParametersAreWrittenToTheSerialBus) {
	OI_tc.start();
	OI_tc.baud(roomba::series500::BAUD_57600);
	ASSERT_EQ(roomba::series500::command::BAUD, static_cast<uint8_t>(serial_bus[0]));
	ASSERT_EQ(roomba::series500::BAUD_57600, static_cast<uint8_t>(serial_bus[1]));
}

TEST_F(fnSerialWriteIsAvailable, baud$WHENCalledTHENBlockFor100ms) {
	std::chrono::steady_clock::time_point begin, end;
	
	OI_tc.start();
	begin = std::chrono::steady_clock::now();
	OI_tc.baud(roomba::series500::BAUD_57600);
	end = std::chrono::steady_clock::now();
	
	ASSERT_LE(100, (std::chrono::duration_cast<std::chrono::milliseconds>(end - begin)).count());
}

TEST_F(fnSerialWriteIsAvailable, baud$WHENOIModeIsOffTHENReturnsError) {
	ASSERT_EQ(roomba::series500::OpenInterface::OI_NOT_STARTED, OI_tc.baud(roomba::series500::BAUD_57600));
}

TEST_F(FailedSerialTransaction, baud$WHENfnSerialWriteFailsTHENReturnsError) {
	OI_tc.start();
	ASSERT_EQ(roomba::series500::OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.baud(roomba::series500::BAUD_57600));
}

} // namespace

/* Created and copyrighted by Zachary J. Fields. All rights reserved. */
