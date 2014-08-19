/* Created and copyrighted by Zachary J. Fields. All rights reserved. */

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../OI.h"

#include <cstring>

using namespace roomba::series500;

  /******************/
 /* TESTABLE CLASS */
/******************/
class OpenInterface_TC : public OpenInterface {
  public:
	using OpenInterface::_fnSerialWrite;
	using OpenInterface::_mode;
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
		OI_tc.connectToSerialBus([](const uint8_t *, size_t){ return 0; });
	}
	
	//virtual ~Initialization() {}
	//virtual void SetUp() {}
	//virtual void TearDown() {}
	
	OpenInterface_TC OI_tc;
};

class FailedSerialTransactionOIAlreadyStarted : public ::testing::Test {
  protected:
	FailedSerialTransactionOIAlreadyStarted (
		void
	) {
		OI_tc.connectToSerialBus([](const uint8_t *, size_t){ return 0; });
		OI_tc._mode = PASSIVE;
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
	) {
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
	virtual void SetUp() {
		serial_bus = new char[64]();
	}
	virtual void TearDown() {
		delete[](serial_bus);
	}
	
	char *serial_bus;
	OpenInterface_TC OI_tc;
};

class fnSerialWriteIsAvailableOIAlreadyStarted : public ::testing::Test {
  protected:
	fnSerialWriteIsAvailableOIAlreadyStarted (
		void
	) {
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
		OI_tc._mode = PASSIVE;
	}
		
	//virtual ~Initialization() {}
	virtual void SetUp() {
		serial_bus = new char[64]();
	}
	virtual void TearDown() {
		delete[](serial_bus);
	}
	
	char *serial_bus;
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
	ASSERT_EQ(OFF, OI_tc._mode);
}

TEST_F(DefaultInitialization, connectToSerialBus$WHENCalledTHENFnSerialWriteIsStored) {
	ASSERT_EQ(0, OI_tc._fnSerialWrite(NULL, 0));
	OI_tc.connectToSerialBus([](const uint8_t *, size_t){ return 69; });
	ASSERT_EQ(69, OI_tc._fnSerialWrite(NULL, 0));
}

TEST_F(fnSerialWriteIsAvailable, start$WHENCalledTHEN128IsWrittenToTheSerialBus) {
	OI_tc.start();
	ASSERT_EQ(128, static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(fnSerialWriteIsAvailable, start$WHENCalledTHENModeIsSetToPassive) {
	OI_tc.start();
	ASSERT_EQ(PASSIVE, OI_tc._mode);
}

TEST_F(FailedSerialTransaction, start$WHENfnSerialWriteFailsTHENReturnsError) {
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.start());
}

TEST_F(FailedSerialTransaction, start$WHENReturnsErrorTHENModeIsUnchanged) {
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.start());
	ASSERT_EQ(OFF, OI_tc._mode);
}

TEST_F(fnSerialWriteIsAvailableOIAlreadyStarted, baud$WHENCalledTHEN129AndParametersAreWrittenToTheSerialBus) {
	OI_tc.baud(BAUD_57600);
	ASSERT_EQ(129, static_cast<uint8_t>(serial_bus[0]));
	ASSERT_EQ(BAUD_57600, static_cast<uint8_t>(serial_bus[1]));
}

TEST_F(fnSerialWriteIsAvailableOIAlreadyStarted, baud$WHENCalledTHENBlockFor100ms) {
	std::chrono::steady_clock::time_point begin, end;
	
	begin = std::chrono::steady_clock::now();
	OI_tc.baud(BAUD_57600);
	end = std::chrono::steady_clock::now();
	
	ASSERT_LE(100, (std::chrono::duration_cast<std::chrono::milliseconds>(end - begin)).count());
}

TEST_F(fnSerialWriteIsAvailable, baud$WHENOIModeIsOffTHENReturnsError) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.baud(BAUD_57600));
}

TEST_F(FailedSerialTransactionOIAlreadyStarted, baud$WHENfnSerialWriteFailsTHENReturnsError) {
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.baud(BAUD_57600));
}

TEST_F(fnSerialWriteIsAvailable, baud$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.baud(BAUD_57600));
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(fnSerialWriteIsAvailableOIAlreadyStarted, safe$WHENCalledTHEN131IsWrittenToTheSerialBus) {
	OI_tc.safe();
	ASSERT_EQ(131, static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(fnSerialWriteIsAvailableOIAlreadyStarted, safe$WHENCalledTHENModeIsSetToSafe) {
	OI_tc.safe();
	ASSERT_EQ(SAFE, OI_tc._mode);
}

TEST_F(fnSerialWriteIsAvailable, safe$WHENOIModeIsOffTHENReturnsError) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.safe());
}

TEST_F(FailedSerialTransactionOIAlreadyStarted, safe$WHENfnSerialWriteFailsTHENReturnsError) {
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.safe());
}

TEST_F(fnSerialWriteIsAvailable, safe$WHENReturnsOINotStartedErrorTHENModeIsUnchanged) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.safe());
	ASSERT_EQ(OFF, OI_tc._mode);
}

TEST_F(FailedSerialTransactionOIAlreadyStarted, safe$WHENReturnsSerialTransferFailureErrorTHENModeIsUnchanged) {
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.safe());
	ASSERT_EQ(PASSIVE, OI_tc._mode);
}

TEST_F(fnSerialWriteIsAvailable, safe$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.safe());
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(fnSerialWriteIsAvailableOIAlreadyStarted, full$WHENCalledTHEN132IsWrittenToTheSerialBus) {
	OI_tc.full();
	ASSERT_EQ(132, static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(fnSerialWriteIsAvailableOIAlreadyStarted, full$WHENCalledTHENModeIsSetToFull) {
	OI_tc.full();
	ASSERT_EQ(FULL, OI_tc._mode);
}

TEST_F(fnSerialWriteIsAvailable, full$WHENOIModeIsOffTHENReturnsError) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.full());
}

TEST_F(FailedSerialTransactionOIAlreadyStarted, full$WHENfnSerialWriteFailsTHENReturnsError) {
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.full());
}

TEST_F(fnSerialWriteIsAvailable, full$WHENReturnsOINotStartedErrorTHENModeIsUnchanged) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.full());
	ASSERT_EQ(OFF, OI_tc._mode);
}

TEST_F(FailedSerialTransactionOIAlreadyStarted, full$WHENReturnsSerialTransferFailureErrorTHENModeIsUnchanged) {
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.full());
	ASSERT_EQ(PASSIVE, OI_tc._mode);
}

TEST_F(fnSerialWriteIsAvailable, full$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.full());
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(fnSerialWriteIsAvailableOIAlreadyStarted, power$WHENCalledTHEN133IsWrittenToTheSerialBus) {
	OI_tc.power();
	ASSERT_EQ(133, static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(fnSerialWriteIsAvailableOIAlreadyStarted, power$WHENCalledTHENModeIsSetToPassive) {
	OI_tc._mode = FULL;
	OI_tc.power();
	ASSERT_EQ(PASSIVE, OI_tc._mode);
}

TEST_F(fnSerialWriteIsAvailable, power$WHENOIModeIsOffTHENReturnsError) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.power());
}

TEST_F(FailedSerialTransactionOIAlreadyStarted, power$WHENfnSerialWriteFailsTHENReturnsError) {
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.power());
}

TEST_F(fnSerialWriteIsAvailable, power$WHENReturnsOINotStartedErrorTHENModeIsUnchanged) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.power());
	ASSERT_EQ(OFF, OI_tc._mode);
}

TEST_F(FailedSerialTransactionOIAlreadyStarted, power$WHENReturnsSerialTransferFailureErrorTHENModeIsUnchanged) {
	OI_tc._mode = FULL;
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.power());
	ASSERT_EQ(FULL, OI_tc._mode);
}

TEST_F(fnSerialWriteIsAvailable, power$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.power());
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(fnSerialWriteIsAvailableOIAlreadyStarted, spot$WHENCalledTHEN134IsWrittenToTheSerialBus) {
	OI_tc.spot();
	ASSERT_EQ(134, static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(fnSerialWriteIsAvailable, spot$WHENOIModeIsOffTHENReturnsError) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.spot());
}

TEST_F(fnSerialWriteIsAvailableOIAlreadyStarted, spot$WHENCalledTHENModeIsSetToPassive) {
	OI_tc._mode = FULL;
	OI_tc.spot();
	ASSERT_EQ(PASSIVE, OI_tc._mode);
}

TEST_F(FailedSerialTransactionOIAlreadyStarted, spot$WHENfnSerialWriteFailsTHENReturnsError) {
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.spot());
}

TEST_F(fnSerialWriteIsAvailable, spot$WHENReturnsOINotStartedErrorTHENModeIsUnchanged) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.spot());
	ASSERT_EQ(OFF, OI_tc._mode);
}

TEST_F(FailedSerialTransactionOIAlreadyStarted, spot$WHENReturnsSerialTransferFailureErrorTHENModeIsUnchanged) {
	OI_tc._mode = FULL;
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.spot());
	ASSERT_EQ(FULL, OI_tc._mode);
}

TEST_F(fnSerialWriteIsAvailable, spot$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.spot());
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(fnSerialWriteIsAvailableOIAlreadyStarted, clean$WHENCalledTHEN135IsWrittenToTheSerialBus) {
	OI_tc.clean();
	ASSERT_EQ(135, static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(fnSerialWriteIsAvailable, clean$WHENOIModeIsOffTHENReturnsError) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.clean());
}

TEST_F(fnSerialWriteIsAvailableOIAlreadyStarted, clean$WHENCalledTHENModeIsSetToPassive) {
	OI_tc._mode = FULL;
	OI_tc.clean();
	ASSERT_EQ(PASSIVE, OI_tc._mode);
}

TEST_F(FailedSerialTransactionOIAlreadyStarted, clean$WHENfnSerialWriteFailsTHENReturnsError) {
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.clean());
}

TEST_F(fnSerialWriteIsAvailable, clean$WHENReturnsOINotStartedErrorTHENModeIsUnchanged) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.clean());
	ASSERT_EQ(OFF, OI_tc._mode);
}

TEST_F(FailedSerialTransactionOIAlreadyStarted, clean$WHENReturnsSerialTransferFailureErrorTHENModeIsUnchanged) {
	OI_tc._mode = FULL;
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.clean());
	ASSERT_EQ(FULL, OI_tc._mode);
}

TEST_F(fnSerialWriteIsAvailable, clean$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.clean());
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(fnSerialWriteIsAvailableOIAlreadyStarted, max$WHENCalledTHEN136IsWrittenToTheSerialBus) {
	OI_tc.max();
	ASSERT_EQ(136, static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(fnSerialWriteIsAvailable, max$WHENOIModeIsOffTHENReturnsError) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.max());
}

TEST_F(fnSerialWriteIsAvailableOIAlreadyStarted, max$WHENCalledTHENModeIsSetToPassive) {
	OI_tc._mode = FULL;
	OI_tc.max();
	ASSERT_EQ(PASSIVE, OI_tc._mode);
}

TEST_F(FailedSerialTransactionOIAlreadyStarted, max$WHENfnSerialWriteFailsTHENReturnsError) {
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.max());
}

TEST_F(fnSerialWriteIsAvailable, max$WHENReturnsOINotStartedErrorTHENModeIsUnchanged) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.max());
	ASSERT_EQ(OFF, OI_tc._mode);
}

TEST_F(FailedSerialTransactionOIAlreadyStarted, max$WHENReturnsSerialTransferFailureErrorTHENModeIsUnchanged) {
	OI_tc._mode = FULL;
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.max());
	ASSERT_EQ(FULL, OI_tc._mode);
}

TEST_F(fnSerialWriteIsAvailable, max$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.max());
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(fnSerialWriteIsAvailableOIAlreadyStarted, drive$WHENCalledTHEN137AndParametersAreWrittenToTheSerialBus) {
	OI_tc._mode = FULL;
	OI_tc.drive(-487, 1998);
	
	ASSERT_EQ(137, static_cast<uint8_t>(serial_bus[0]));
	EXPECT_EQ(-487, *reinterpret_cast<int16_t *>(&serial_bus[1]));
	EXPECT_EQ(1998, *reinterpret_cast<int16_t *>(&serial_bus[3]));
}

TEST_F(fnSerialWriteIsAvailableOIAlreadyStarted, drive$WHENVelocityIsGreaterThan500THENParameterIsInvalid) {
	OI_tc._mode = FULL;
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.drive(501, 1998));
}

TEST_F(fnSerialWriteIsAvailableOIAlreadyStarted, drive$WHENVelocityIsLessThanNegative500THENParameterIsInvalid) {
	OI_tc._mode = FULL;
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.drive(-501, 1998));
}

TEST_F(fnSerialWriteIsAvailableOIAlreadyStarted, drive$WHENRadiusIsGreaterThan2000THENParameterIsInvalid) {
	OI_tc._mode = FULL;
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.drive(487, 2001));
}

TEST_F(fnSerialWriteIsAvailableOIAlreadyStarted, drive$WHENRadiusIsLessThanNegative2000THENParameterIsInvalid) {
	OI_tc._mode = FULL;
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.drive(487, -2001));
}

TEST_F(fnSerialWriteIsAvailableOIAlreadyStarted, drive$WHENTimeParameterIsInvalidTHENNoDataIsWrittenToSerialBus) {
	OI_tc._mode = FULL;
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.drive(501, 1998));
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(fnSerialWriteIsAvailable, drive$WHENOIModeIsOffTHENReturnsError) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.drive(-487, 1998));
}

TEST_F(FailedSerialTransactionOIAlreadyStarted, drive$WHENfnSerialWriteFailsTHENReturnsError) {
	OI_tc._mode = FULL;
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.drive(-487, 1998));
}

TEST_F(fnSerialWriteIsAvailable, drive$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.drive(-487, 1998));
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(fnSerialWriteIsAvailableOIAlreadyStarted, drive$WHENOIModeIsPassiveTHENReturnsError) {
	ASSERT_EQ(OpenInterface::INVALID_MODE_FOR_REQUESTED_OPERATION, OI_tc.drive(-487, 1998));
}

TEST_F(fnSerialWriteIsAvailableOIAlreadyStarted, drive$WHENOIModeIsPassiveTHENNoDataIsWrittenToSerialBus) {
	ASSERT_EQ(OpenInterface::INVALID_MODE_FOR_REQUESTED_OPERATION, OI_tc.drive(-487, 1998));
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(fnSerialWriteIsAvailableOIAlreadyStarted, drive$WHENRadiusIsEqualToSpecialValue32767THENParameterIsAllowed) {
	OI_tc._mode = FULL;
	OI_tc.drive(-487, 32767);
	
	ASSERT_EQ(137, static_cast<uint8_t>(serial_bus[0]));
	EXPECT_EQ(-487, *reinterpret_cast<int16_t *>(&serial_bus[1]));
	EXPECT_EQ(32767, *reinterpret_cast<int16_t *>(&serial_bus[3]));
}

TEST_F(fnSerialWriteIsAvailableOIAlreadyStarted, seekDock$WHENCalledTHEN143IsWrittenToTheSerialBus) {
	OI_tc.seekDock();
	ASSERT_EQ(143, static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(fnSerialWriteIsAvailable, seekDock$WHENOIModeIsOffTHENReturnsError) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.seekDock());
}

TEST_F(fnSerialWriteIsAvailableOIAlreadyStarted, seekDock$WHENCalledTHENModeIsSetToPassive) {
	OI_tc._mode = FULL;
	OI_tc.seekDock();
	ASSERT_EQ(PASSIVE, OI_tc._mode);
}

TEST_F(FailedSerialTransactionOIAlreadyStarted, seekDock$WHENfnSerialWriteFailsTHENReturnsError) {
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.seekDock());
}

TEST_F(fnSerialWriteIsAvailable, seekDock$WHENReturnsOINotStartedErrorTHENModeIsUnchanged) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.seekDock());
	ASSERT_EQ(OFF, OI_tc._mode);
}

TEST_F(FailedSerialTransactionOIAlreadyStarted, seekDock$WHENReturnsSerialTransferFailureErrorTHENModeIsUnchanged) {
	OI_tc._mode = FULL;
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.seekDock());
	ASSERT_EQ(FULL, OI_tc._mode);
}

TEST_F(fnSerialWriteIsAvailable, seekDock$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.seekDock());
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(fnSerialWriteIsAvailableOIAlreadyStarted, schedule$WHENCalledTHEN167AndParametersAreWrittenToTheSerialBus) {
	bitmask::Days days = static_cast<bitmask::Days>(bitmask::SUNDAY | bitmask::MONDAY | bitmask::TUESDAY | bitmask::WEDNESDAY | bitmask::THURSDAY | bitmask::FRIDAY | bitmask::SATURDAY);
	OpenInterface::clock_time_t clk_time[7];
	clk_time[0].hour = 15;
	clk_time[0].minute = 35;
	clk_time[1].hour = 10;
	clk_time[1].minute = 15;
	clk_time[2].hour = 16;
	clk_time[2].minute = 40;
	clk_time[3].hour = 8;
	clk_time[3].minute = 45;
	clk_time[4].hour = 9;
	clk_time[4].minute = 30;
	clk_time[5].hour = 14;
	clk_time[5].minute = 10;
	clk_time[6].hour = 11;
	clk_time[6].minute = 55;
	
	OI_tc.schedule(days, clk_time);
	
	ASSERT_EQ(167, static_cast<uint8_t>(serial_bus[0]));
	EXPECT_EQ(127, static_cast<uint8_t>(serial_bus[1]));
	EXPECT_EQ(15, static_cast<uint8_t>(serial_bus[2]));
	EXPECT_EQ(35, static_cast<uint8_t>(serial_bus[3]));
	EXPECT_EQ(10, static_cast<uint8_t>(serial_bus[4]));
	EXPECT_EQ(15, static_cast<uint8_t>(serial_bus[5]));
	EXPECT_EQ(16, static_cast<uint8_t>(serial_bus[6]));
	EXPECT_EQ(40, static_cast<uint8_t>(serial_bus[7]));
	EXPECT_EQ(8, static_cast<uint8_t>(serial_bus[8]));
	EXPECT_EQ(45, static_cast<uint8_t>(serial_bus[9]));
	EXPECT_EQ(9, static_cast<uint8_t>(serial_bus[10]));
	EXPECT_EQ(30, static_cast<uint8_t>(serial_bus[11]));
	EXPECT_EQ(14, static_cast<uint8_t>(serial_bus[12]));
	EXPECT_EQ(10, static_cast<uint8_t>(serial_bus[13]));
	EXPECT_EQ(11, static_cast<uint8_t>(serial_bus[14]));
	EXPECT_EQ(55, static_cast<uint8_t>(serial_bus[15]));
}

TEST_F(fnSerialWriteIsAvailableOIAlreadyStarted, schedule$WHENCalledWithDisableInBitMaskTHEN167AndAllZerosAreWrittenToTheSerialBus) {
	OpenInterface::clock_time_t clk_time[7];
	clk_time[0].hour = 15;
	clk_time[0].minute = 35;
	clk_time[1].hour = 10;
	clk_time[1].minute = 15;
	clk_time[2].hour = 16;
	clk_time[2].minute = 40;
	clk_time[3].hour = 8;
	clk_time[3].minute = 45;
	clk_time[4].hour = 9;
	clk_time[4].minute = 30;
	clk_time[5].hour = 14;
	clk_time[5].minute = 10;
	clk_time[6].hour = 11;
	clk_time[6].minute = 55;
	
	OI_tc.schedule(bitmask::DISABLE, clk_time);
	
	ASSERT_EQ(167, static_cast<uint8_t>(serial_bus[0]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[1]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[2]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[3]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[4]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[5]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[6]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[7]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[8]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[9]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[10]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[11]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[12]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[13]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[14]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[15]));
}

TEST_F(fnSerialWriteIsAvailableOIAlreadyStarted, schedule$WHENCalledWithSparseCParametersTHENFullZeroFilledSerialParametersAreWrittenToTheSerialBus) {
	bitmask::Days days = static_cast<bitmask::Days>(bitmask::WEDNESDAY | bitmask::SUNDAY);
	OpenInterface::clock_time_t clk_time[2];
	clk_time[0].hour = 15;
	clk_time[0].minute = 35;
	clk_time[1].hour = 10;
	
	OI_tc.schedule(days, clk_time);
	
	ASSERT_EQ(167, static_cast<uint8_t>(serial_bus[0]));
	EXPECT_EQ(9, static_cast<uint8_t>(serial_bus[1]));
	EXPECT_EQ(15, static_cast<uint8_t>(serial_bus[2]));
	EXPECT_EQ(35, static_cast<uint8_t>(serial_bus[3]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[4]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[5]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[6]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[7]));
	EXPECT_EQ(10, static_cast<uint8_t>(serial_bus[8]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[9]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[10]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[11]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[12]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[13]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[14]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[15]));
}

TEST_F(fnSerialWriteIsAvailableOIAlreadyStarted, schedule$WHENCalledWithNULLArrayTHEN167AndAllZerosAreWrittenToTheSerialBus) {
	bitmask::Days days = static_cast<bitmask::Days>(bitmask::WEDNESDAY | bitmask::SUNDAY);
	
	OI_tc.schedule(days, NULL);
	
	ASSERT_EQ(167, static_cast<uint8_t>(serial_bus[0]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[1]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[2]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[3]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[4]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[5]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[6]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[7]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[8]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[9]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[10]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[11]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[12]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[13]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[14]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[15]));
}

TEST_F(fnSerialWriteIsAvailableOIAlreadyStarted, schedule$WHENCalledWithInvalidTimeParametersTHENCorrespondingDayIsIgnored) {
	bitmask::Days days = static_cast<bitmask::Days>(bitmask::WEDNESDAY | bitmask::SUNDAY);
	OpenInterface::clock_time_t clk_time[2];
	clk_time[0].hour = 15;
	clk_time[0].minute = 35;
	clk_time[1].hour = 31;
	clk_time[1].minute = 18;
	
	OI_tc.schedule(days, clk_time);
	
	ASSERT_EQ(167, static_cast<uint8_t>(serial_bus[0]));
	EXPECT_EQ(9, static_cast<uint8_t>(serial_bus[1]));
	EXPECT_EQ(15, static_cast<uint8_t>(serial_bus[2]));
	EXPECT_EQ(35, static_cast<uint8_t>(serial_bus[3]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[4]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[5]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[6]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[7]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[8]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[9]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[10]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[11]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[12]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[13]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[14]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[15]));
}

TEST_F(fnSerialWriteIsAvailable, schedule$WHENOIModeIsOffTHENReturnsError) {
	bitmask::Days days = static_cast<bitmask::Days>(bitmask::WEDNESDAY | bitmask::SUNDAY);
	OpenInterface::clock_time_t clk_time[2];
	clk_time[0].hour = 15;
	clk_time[0].minute = 35;
	clk_time[1].hour = 10;
	
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.schedule(days, clk_time));
}

TEST_F(FailedSerialTransactionOIAlreadyStarted, schedule$WHENfnSerialWriteFailsTHENReturnsError) {
	bitmask::Days days = static_cast<bitmask::Days>(bitmask::WEDNESDAY | bitmask::SUNDAY);
	OpenInterface::clock_time_t clk_time[2];
	clk_time[0].hour = 15;
	clk_time[0].minute = 35;
	clk_time[1].hour = 10;
	
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.schedule(days, clk_time));
}

TEST_F(fnSerialWriteIsAvailable, schedule$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	bitmask::Days days = static_cast<bitmask::Days>(bitmask::WEDNESDAY | bitmask::SUNDAY);
	OpenInterface::clock_time_t clk_time[2];
	clk_time[0].hour = 15;
	clk_time[0].minute = 35;
	clk_time[1].hour = 10;
	
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.schedule(days, clk_time));
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(fnSerialWriteIsAvailableOIAlreadyStarted, setDayTime$WHENCalledTHEN168AndParametersAreWrittenToTheSerialBus) {
	OI_tc.setDayTime(TUESDAY, OpenInterface::clock_time_t(11,23));
	
	ASSERT_EQ(168, static_cast<uint8_t>(serial_bus[0]));
	EXPECT_EQ(2, static_cast<uint8_t>(serial_bus[1]));
	EXPECT_EQ(11, static_cast<uint8_t>(serial_bus[2]));
	EXPECT_EQ(23, static_cast<uint8_t>(serial_bus[3]));
}

TEST_F(fnSerialWriteIsAvailableOIAlreadyStarted, setDayTime$WHENHourIsGreaterThan23THENTimeParameterIsInvalid) {
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.setDayTime(TUESDAY, OpenInterface::clock_time_t(24,18)));
}

TEST_F(fnSerialWriteIsAvailableOIAlreadyStarted, setDayTime$WHENHourIsLessThanZeroTHENTimeParameterIsInvalid) {
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.setDayTime(TUESDAY, OpenInterface::clock_time_t(-1,18)));
}

TEST_F(fnSerialWriteIsAvailableOIAlreadyStarted, setDayTime$WHENMinuteIsGreaterThan60THENTimeParameterIsInvalid) {
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.setDayTime(TUESDAY, OpenInterface::clock_time_t(22,60)));
}

TEST_F(fnSerialWriteIsAvailableOIAlreadyStarted, setDayTime$WHENMinuteIsLessThanZeroTHENTimeParameterIsInvalid) {
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.setDayTime(TUESDAY, OpenInterface::clock_time_t(22,-1)));
}

TEST_F(fnSerialWriteIsAvailableOIAlreadyStarted, setDayTime$WHENTimeParameterIsInvalidTHENNoDataIsWrittenToSerialBus) {
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.setDayTime(TUESDAY, OpenInterface::clock_time_t(31,18)));
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(fnSerialWriteIsAvailable, setDayTime$WHENOIModeIsOffTHENReturnsError) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.setDayTime(TUESDAY, OpenInterface::clock_time_t(11,23)));
}

TEST_F(FailedSerialTransactionOIAlreadyStarted, setDayTime$WHENfnSerialWriteFailsTHENReturnsError) {
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.setDayTime(TUESDAY, OpenInterface::clock_time_t(11,23)));
}

TEST_F(fnSerialWriteIsAvailable, setDayTime$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.setDayTime(TUESDAY, OpenInterface::clock_time_t(11,23)));
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

} // namespace

/* Created and copyrighted by Zachary J. Fields. All rights reserved. */
