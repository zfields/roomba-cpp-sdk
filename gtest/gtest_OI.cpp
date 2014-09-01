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
class ObjectInitialization : public ::testing::Test {
  protected:
	ObjectInitialization (
		void
	) {}
	
	//virtual ~Initialization() {}
	//virtual void SetUp() {}
	//virtual void TearDown() {}
	
	OpenInterface_TC OI_tc;
};

class SerialTransactionFailureOIModeOFF : public ::testing::Test {
  protected:
	SerialTransactionFailureOIModeOFF (
		void
	) {
		OI_tc.connectToSerialBus([](const uint8_t *, size_t){ return 0; });
	}
	
	//virtual ~Initialization() {}
	//virtual void SetUp() {}
	//virtual void TearDown() {}
	
	OpenInterface_TC OI_tc;
};

class SerialTransactionFailureOIModePASSIVE : public ::testing::Test {
  protected:
	SerialTransactionFailureOIModePASSIVE (
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

class SerialTransactionFailureOIModeFULL : public ::testing::Test {
  protected:
	SerialTransactionFailureOIModeFULL (
		void
	) {
		OI_tc.connectToSerialBus([](const uint8_t *, size_t){ return 0; });
		OI_tc._mode = FULL;
	}
	
	//virtual ~Initialization() {}
	//virtual void SetUp() {}
	//virtual void TearDown() {}
	
	OpenInterface_TC OI_tc;
};

class AllSystemsGoOIModeOFF : public ::testing::Test {
  protected:
	AllSystemsGoOIModeOFF (
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

class AllSystemsGoOIModePASSIVE : public ::testing::Test {
  protected:
	AllSystemsGoOIModePASSIVE (
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

class AllSystemsGoOIModeFULL : public ::testing::Test {
  protected:
	AllSystemsGoOIModeFULL (
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
		OI_tc._mode = FULL;
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

TEST_F(ObjectInitialization, constructor$WHENInitializedTHENCallingFnSerialWriteWillNotThrowError) {
	OI_tc._fnSerialWrite(NULL, 0);
}

TEST_F(ObjectInitialization, constructor$WHENInitializedTHENModeWillBeSetToOFF) {
	ASSERT_EQ(OFF, OI_tc._mode);
}

TEST_F(ObjectInitialization, connectToSerialBus$WHENCalledTHENFnSerialWriteIsStored) {
	ASSERT_EQ(0, OI_tc._fnSerialWrite(NULL, 0));
	OI_tc.connectToSerialBus([](const uint8_t *, size_t){ return 69; });
	ASSERT_EQ(69, OI_tc._fnSerialWrite(NULL, 0));
}

TEST_F(AllSystemsGoOIModeOFF, start$WHENCalledTHEN128IsWrittenToTheSerialBus) {
	OI_tc.start();
	ASSERT_EQ(128, static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModeOFF, start$WHENCalledTHENModeIsSetToPassive) {
	OI_tc.start();
	ASSERT_EQ(PASSIVE, OI_tc._mode);
}

TEST_F(SerialTransactionFailureOIModeOFF, start$WHENfnSerialWriteFailsTHENReturnsError) {
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.start());
}

TEST_F(SerialTransactionFailureOIModeOFF, start$WHENReturnsErrorTHENModeIsUnchanged) {
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.start());
	ASSERT_EQ(OFF, OI_tc._mode);
}

TEST_F(AllSystemsGoOIModePASSIVE, baud$WHENCalledTHEN129AndParametersAreWrittenToTheSerialBus) {
	OI_tc.baud(BAUD_57600);
	ASSERT_EQ(129, static_cast<uint8_t>(serial_bus[0]));
	ASSERT_EQ(BAUD_57600, static_cast<uint8_t>(serial_bus[1]));
}

TEST_F(AllSystemsGoOIModePASSIVE, baud$WHENCalledTHENBlockFor100ms) {
	std::chrono::steady_clock::time_point begin, end;
	
	begin = std::chrono::steady_clock::now();
	OI_tc.baud(BAUD_57600);
	end = std::chrono::steady_clock::now();
	
	ASSERT_LE(100, (std::chrono::duration_cast<std::chrono::milliseconds>(end - begin)).count());
}

TEST_F(AllSystemsGoOIModeOFF, baud$WHENOIModeIsOffTHENReturnsError) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.baud(BAUD_57600));
}

TEST_F(SerialTransactionFailureOIModePASSIVE, baud$WHENfnSerialWriteFailsTHENReturnsError) {
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.baud(BAUD_57600));
}

TEST_F(AllSystemsGoOIModeOFF, baud$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.baud(BAUD_57600));
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModePASSIVE, safe$WHENCalledTHEN131IsWrittenToTheSerialBus) {
	OI_tc.safe();
	ASSERT_EQ(131, static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModePASSIVE, safe$WHENCalledTHENModeIsSetToSafe) {
	OI_tc.safe();
	ASSERT_EQ(SAFE, OI_tc._mode);
}

TEST_F(AllSystemsGoOIModeOFF, safe$WHENOIModeIsOffTHENReturnsError) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.safe());
}

TEST_F(SerialTransactionFailureOIModePASSIVE, safe$WHENfnSerialWriteFailsTHENReturnsError) {
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.safe());
}

TEST_F(AllSystemsGoOIModeOFF, safe$WHENReturnsOINotStartedErrorTHENModeIsUnchanged) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.safe());
	ASSERT_EQ(OFF, OI_tc._mode);
}

TEST_F(SerialTransactionFailureOIModePASSIVE, safe$WHENReturnsSerialTransferFailureErrorTHENModeIsUnchanged) {
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.safe());
	ASSERT_EQ(PASSIVE, OI_tc._mode);
}

TEST_F(AllSystemsGoOIModeOFF, safe$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.safe());
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModePASSIVE, full$WHENCalledTHEN132IsWrittenToTheSerialBus) {
	OI_tc.full();
	ASSERT_EQ(132, static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModePASSIVE, full$WHENCalledTHENModeIsSetToFull) {
	OI_tc.full();
	ASSERT_EQ(FULL, OI_tc._mode);
}

TEST_F(AllSystemsGoOIModeOFF, full$WHENOIModeIsOffTHENReturnsError) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.full());
}

TEST_F(SerialTransactionFailureOIModePASSIVE, full$WHENfnSerialWriteFailsTHENReturnsError) {
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.full());
}

TEST_F(AllSystemsGoOIModeOFF, full$WHENReturnsOINotStartedErrorTHENModeIsUnchanged) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.full());
	ASSERT_EQ(OFF, OI_tc._mode);
}

TEST_F(SerialTransactionFailureOIModePASSIVE, full$WHENReturnsSerialTransferFailureErrorTHENModeIsUnchanged) {
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.full());
	ASSERT_EQ(PASSIVE, OI_tc._mode);
}

TEST_F(AllSystemsGoOIModeOFF, full$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.full());
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModePASSIVE, power$WHENCalledTHEN133IsWrittenToTheSerialBus) {
	OI_tc.power();
	ASSERT_EQ(133, static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModeFULL, power$WHENCalledTHENModeIsSetToPassive) {
	OI_tc.power();
	ASSERT_EQ(PASSIVE, OI_tc._mode);
}

TEST_F(AllSystemsGoOIModeOFF, power$WHENOIModeIsOffTHENReturnsError) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.power());
}

TEST_F(SerialTransactionFailureOIModePASSIVE, power$WHENfnSerialWriteFailsTHENReturnsError) {
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.power());
}

TEST_F(AllSystemsGoOIModeOFF, power$WHENReturnsOINotStartedErrorTHENModeIsUnchanged) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.power());
	ASSERT_EQ(OFF, OI_tc._mode);
}

TEST_F(SerialTransactionFailureOIModeFULL, power$WHENReturnsSerialTransferFailureErrorTHENModeIsUnchanged) {
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.power());
	ASSERT_EQ(FULL, OI_tc._mode);
}

TEST_F(AllSystemsGoOIModeOFF, power$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.power());
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModePASSIVE, spot$WHENCalledTHEN134IsWrittenToTheSerialBus) {
	OI_tc.spot();
	ASSERT_EQ(134, static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModeOFF, spot$WHENOIModeIsOffTHENReturnsError) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.spot());
}

TEST_F(AllSystemsGoOIModeFULL, spot$WHENCalledTHENModeIsSetToPassive) {
	OI_tc.spot();
	ASSERT_EQ(PASSIVE, OI_tc._mode);
}

TEST_F(SerialTransactionFailureOIModePASSIVE, spot$WHENfnSerialWriteFailsTHENReturnsError) {
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.spot());
}

TEST_F(AllSystemsGoOIModeOFF, spot$WHENReturnsOINotStartedErrorTHENModeIsUnchanged) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.spot());
	ASSERT_EQ(OFF, OI_tc._mode);
}

TEST_F(SerialTransactionFailureOIModeFULL, spot$WHENReturnsSerialTransferFailureErrorTHENModeIsUnchanged) {
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.spot());
	ASSERT_EQ(FULL, OI_tc._mode);
}

TEST_F(AllSystemsGoOIModeOFF, spot$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.spot());
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModePASSIVE, clean$WHENCalledTHEN135IsWrittenToTheSerialBus) {
	OI_tc.clean();
	ASSERT_EQ(135, static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModeOFF, clean$WHENOIModeIsOffTHENReturnsError) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.clean());
}

TEST_F(AllSystemsGoOIModeFULL, clean$WHENCalledTHENModeIsSetToPassive) {
	OI_tc.clean();
	ASSERT_EQ(PASSIVE, OI_tc._mode);
}

TEST_F(SerialTransactionFailureOIModePASSIVE, clean$WHENfnSerialWriteFailsTHENReturnsError) {
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.clean());
}

TEST_F(AllSystemsGoOIModeOFF, clean$WHENReturnsOINotStartedErrorTHENModeIsUnchanged) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.clean());
	ASSERT_EQ(OFF, OI_tc._mode);
}

TEST_F(SerialTransactionFailureOIModeFULL, clean$WHENReturnsSerialTransferFailureErrorTHENModeIsUnchanged) {
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.clean());
	ASSERT_EQ(FULL, OI_tc._mode);
}

TEST_F(AllSystemsGoOIModeOFF, clean$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.clean());
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModePASSIVE, max$WHENCalledTHEN136IsWrittenToTheSerialBus) {
	OI_tc.max();
	ASSERT_EQ(136, static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModeOFF, max$WHENOIModeIsOffTHENReturnsError) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.max());
}

TEST_F(AllSystemsGoOIModeFULL, max$WHENCalledTHENModeIsSetToPassive) {
	OI_tc.max();
	ASSERT_EQ(PASSIVE, OI_tc._mode);
}

TEST_F(SerialTransactionFailureOIModePASSIVE, max$WHENfnSerialWriteFailsTHENReturnsError) {
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.max());
}

TEST_F(AllSystemsGoOIModeOFF, max$WHENReturnsOINotStartedErrorTHENModeIsUnchanged) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.max());
	ASSERT_EQ(OFF, OI_tc._mode);
}

TEST_F(SerialTransactionFailureOIModeFULL, max$WHENReturnsSerialTransferFailureErrorTHENModeIsUnchanged) {
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.max());
	ASSERT_EQ(FULL, OI_tc._mode);
}

TEST_F(AllSystemsGoOIModeOFF, max$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.max());
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModeFULL, drive$WHENCalledTHEN137AndParametersAreWrittenToTheSerialBus) {
	OI_tc.drive(-487, 1998);
	
	ASSERT_EQ(137, static_cast<uint8_t>(serial_bus[0]));
	EXPECT_EQ(254, static_cast<uint8_t>(serial_bus[1]));
	EXPECT_EQ(25, static_cast<uint8_t>(serial_bus[2]));
	EXPECT_EQ(7, static_cast<uint8_t>(serial_bus[3]));
	EXPECT_EQ(206, static_cast<uint8_t>(serial_bus[4]));
}

TEST_F(AllSystemsGoOIModeFULL, drive$WHENVelocityIsGreaterThan500THENParameterIsInvalid) {
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.drive(501, 1998));
}

TEST_F(AllSystemsGoOIModeFULL, drive$WHENVelocityIsLessThanNegative500THENParameterIsInvalid) {
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.drive(-501, 1998));
}

TEST_F(AllSystemsGoOIModeFULL, drive$WHENRadiusIsGreaterThan2000THENParameterIsInvalid) {
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.drive(487, 2001));
}

TEST_F(AllSystemsGoOIModeFULL, drive$WHENRadiusIsLessThanNegative2000THENParameterIsInvalid) {
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.drive(487, -2001));
}

TEST_F(AllSystemsGoOIModeFULL, drive$WHENParametersAreInvalidTHENNoDataIsWrittenToSerialBus) {
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.drive(501, 1998));
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModeOFF, drive$WHENOIModeIsOffTHENReturnsError) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.drive(-487, 1998));
}

TEST_F(SerialTransactionFailureOIModeFULL, drive$WHENfnSerialWriteFailsTHENReturnsError) {
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.drive(-487, 1998));
}

TEST_F(AllSystemsGoOIModeOFF, drive$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.drive(-487, 1998));
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModePASSIVE, drive$WHENOIModeIsPassiveTHENReturnsError) {
	ASSERT_EQ(OpenInterface::INVALID_MODE_FOR_REQUESTED_OPERATION, OI_tc.drive(-487, 1998));
}

TEST_F(AllSystemsGoOIModePASSIVE, drive$WHENOIModeIsPassiveTHENNoDataIsWrittenToSerialBus) {
	ASSERT_EQ(OpenInterface::INVALID_MODE_FOR_REQUESTED_OPERATION, OI_tc.drive(-487, 1998));
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModeFULL, drive$WHENRadiusIsEqualToSpecialValue32767THENParameterIsAllowed) {
	OI_tc.drive(-487, 32767);
	
	ASSERT_EQ(137, static_cast<uint8_t>(serial_bus[0]));
	EXPECT_EQ(254, static_cast<uint8_t>(serial_bus[1]));
	EXPECT_EQ(25, static_cast<uint8_t>(serial_bus[2]));
	EXPECT_EQ(127, static_cast<uint8_t>(serial_bus[3]));
	EXPECT_EQ(255, static_cast<uint8_t>(serial_bus[4]));
}

TEST_F(AllSystemsGoOIModeFULL, motors$WHENCalledTHEN138AndParametersAreWrittenToTheSerialBus) {
	bitmask::MotorStates motor_states = static_cast<bitmask::MotorStates>(bitmask::VACUUM_ENGAGED | bitmask::SIDE_BRUSH_ENGAGED | bitmask::SIDE_BRUSH_CLOCKWISE);
	OI_tc.motors(motor_states);
	
	ASSERT_EQ(138, static_cast<uint8_t>(serial_bus[0]));
	EXPECT_EQ(11, static_cast<uint8_t>(serial_bus[1]));
}

TEST_F(AllSystemsGoOIModeFULL, motors$WHENMotorStateMaskIsGreaterThan31THENParameterIsInvalid) {
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.motors(static_cast<bitmask::MotorStates>(32)));
}

TEST_F(AllSystemsGoOIModeFULL, motors$WHENParametersAreInvalidTHENNoDataIsWrittenToSerialBus) {
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.motors(static_cast<bitmask::MotorStates>(32)));
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModeOFF, motors$WHENOIModeIsOffTHENReturnsError) {
	bitmask::MotorStates motor_states = static_cast<bitmask::MotorStates>(bitmask::VACUUM_ENGAGED | bitmask::SIDE_BRUSH_ENGAGED | bitmask::SIDE_BRUSH_CLOCKWISE);
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.motors(motor_states));
}

TEST_F(SerialTransactionFailureOIModeFULL, motors$WHENfnSerialWriteFailsTHENReturnsError) {
	bitmask::MotorStates motor_states = static_cast<bitmask::MotorStates>(bitmask::VACUUM_ENGAGED | bitmask::SIDE_BRUSH_ENGAGED | bitmask::SIDE_BRUSH_CLOCKWISE);
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.motors(motor_states));
}

TEST_F(AllSystemsGoOIModeOFF, motors$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	bitmask::MotorStates motor_states = static_cast<bitmask::MotorStates>(bitmask::VACUUM_ENGAGED | bitmask::SIDE_BRUSH_ENGAGED | bitmask::SIDE_BRUSH_CLOCKWISE);
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.motors(motor_states));
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModePASSIVE, motors$WHENOIModeIsPassiveTHENReturnsError) {
	bitmask::MotorStates motor_states = static_cast<bitmask::MotorStates>(bitmask::VACUUM_ENGAGED | bitmask::SIDE_BRUSH_ENGAGED | bitmask::SIDE_BRUSH_CLOCKWISE);
	ASSERT_EQ(OpenInterface::INVALID_MODE_FOR_REQUESTED_OPERATION, OI_tc.motors(motor_states));
}

TEST_F(AllSystemsGoOIModePASSIVE, motors$WHENOIModeIsPassiveTHENNoDataIsWrittenToSerialBus) {
	bitmask::MotorStates motor_states = static_cast<bitmask::MotorStates>(bitmask::VACUUM_ENGAGED | bitmask::SIDE_BRUSH_ENGAGED | bitmask::SIDE_BRUSH_CLOCKWISE);
	ASSERT_EQ(OpenInterface::INVALID_MODE_FOR_REQUESTED_OPERATION, OI_tc.motors(motor_states));
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModeFULL, leds$WHENCalledTHEN139AndParametersAreWrittenToTheSerialBus) {
	OI_tc.leds(static_cast<bitmask::display::LEDs>(bitmask::display::SPOT | bitmask::display::DEBRIS), 64, 192);
	
	ASSERT_EQ(139, static_cast<uint8_t>(serial_bus[0]));
	EXPECT_EQ(3, static_cast<uint8_t>(serial_bus[1]));
	EXPECT_EQ(64, static_cast<uint8_t>(serial_bus[2]));
	EXPECT_EQ(192, static_cast<uint8_t>(serial_bus[3]));
}

TEST_F(AllSystemsGoOIModeFULL, leds$WHENLedMaskIsGreaterThan15THENParameterIsInvalid) {
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.leds(static_cast<bitmask::display::LEDs>(16), 64, 192));
}

TEST_F(AllSystemsGoOIModeFULL, leds$WHENParametersAreInvalidTHENNoDataIsWrittenToSerialBus) {
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.leds(static_cast<bitmask::display::LEDs>(16), 64, 192));
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModeOFF, leds$WHENOIModeIsOffTHENReturnsError) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.leds(static_cast<bitmask::display::LEDs>(bitmask::display::SPOT | bitmask::display::DEBRIS), 64, 192));
}

TEST_F(SerialTransactionFailureOIModeFULL, leds$WHENfnSerialWriteFailsTHENReturnsError) {
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.leds(static_cast<bitmask::display::LEDs>(bitmask::display::SPOT | bitmask::display::DEBRIS), 64, 192));
}

TEST_F(AllSystemsGoOIModeOFF, leds$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.leds(static_cast<bitmask::display::LEDs>(bitmask::display::SPOT | bitmask::display::DEBRIS), 64, 192));
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModePASSIVE, leds$WHENOIModeIsPassiveTHENReturnsError) {
	ASSERT_EQ(OpenInterface::INVALID_MODE_FOR_REQUESTED_OPERATION, OI_tc.leds(static_cast<bitmask::display::LEDs>(bitmask::display::SPOT | bitmask::display::DEBRIS), 64, 192));
}

TEST_F(AllSystemsGoOIModePASSIVE, leds$WHENOIModeIsPassiveTHENNoDataIsWrittenToSerialBus) {
	ASSERT_EQ(OpenInterface::INVALID_MODE_FOR_REQUESTED_OPERATION, OI_tc.leds(static_cast<bitmask::display::LEDs>(bitmask::display::SPOT | bitmask::display::DEBRIS), 64, 192));
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModePASSIVE, song$WHENCalledTHEN140AndParametersAreWrittenToTheSerialBus) {
	std::vector<OpenInterface::note_t> fur_elise;
	fur_elise.push_back(std::make_pair(E4, 11));
	fur_elise.push_back(std::make_pair(D4_SHARP, 11));
	fur_elise.push_back(std::make_pair(E4, 11));
	fur_elise.push_back(std::make_pair(D4_SHARP, 11));
	fur_elise.push_back(std::make_pair(E4, 11));
	fur_elise.push_back(std::make_pair(B3, 11));
	fur_elise.push_back(std::make_pair(D4, 11));
	fur_elise.push_back(std::make_pair(C4, 11));
	fur_elise.push_back(std::make_pair(A3, 32));
	
	OI_tc.song(1, fur_elise);
	
	ASSERT_EQ(140, static_cast<uint8_t>(serial_bus[0]));
	EXPECT_EQ(1, static_cast<uint8_t>(serial_bus[1]));
	EXPECT_EQ(9, static_cast<uint8_t>(serial_bus[2]));
	EXPECT_EQ(64, static_cast<uint8_t>(serial_bus[3]));
	EXPECT_EQ(11, static_cast<uint8_t>(serial_bus[4]));
	EXPECT_EQ(63, static_cast<uint8_t>(serial_bus[5]));
	EXPECT_EQ(11, static_cast<uint8_t>(serial_bus[6]));
	EXPECT_EQ(64, static_cast<uint8_t>(serial_bus[7]));
	EXPECT_EQ(11, static_cast<uint8_t>(serial_bus[8]));
	EXPECT_EQ(63, static_cast<uint8_t>(serial_bus[9]));
	EXPECT_EQ(11, static_cast<uint8_t>(serial_bus[10]));
	EXPECT_EQ(64, static_cast<uint8_t>(serial_bus[11]));
	EXPECT_EQ(11, static_cast<uint8_t>(serial_bus[12]));
	EXPECT_EQ(59, static_cast<uint8_t>(serial_bus[13]));
	EXPECT_EQ(11, static_cast<uint8_t>(serial_bus[14]));
	EXPECT_EQ(62, static_cast<uint8_t>(serial_bus[15]));
	EXPECT_EQ(11, static_cast<uint8_t>(serial_bus[16]));
	EXPECT_EQ(60, static_cast<uint8_t>(serial_bus[17]));
	EXPECT_EQ(11, static_cast<uint8_t>(serial_bus[18]));
	EXPECT_EQ(57, static_cast<uint8_t>(serial_bus[19]));
	EXPECT_EQ(32, static_cast<uint8_t>(serial_bus[20]));
}

TEST_F(AllSystemsGoOIModePASSIVE, song$WHENSongNumberIsGreaterThan4THENParameterIsInvalid) {
	std::vector<OpenInterface::note_t> fur_elise;
	fur_elise.push_back(std::make_pair(E4, 11));
	fur_elise.push_back(std::make_pair(D4_SHARP, 11));
	fur_elise.push_back(std::make_pair(E4, 11));
	fur_elise.push_back(std::make_pair(D4_SHARP, 11));
	fur_elise.push_back(std::make_pair(E4, 11));
	fur_elise.push_back(std::make_pair(B3, 11));
	fur_elise.push_back(std::make_pair(D4, 11));
	fur_elise.push_back(std::make_pair(C4, 11));
	fur_elise.push_back(std::make_pair(A3, 32));
	
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.song(5, fur_elise));
}

TEST_F(AllSystemsGoOIModePASSIVE, song$WHENSongIsZeroNotesTHENParameterIsInvalid) {
	std::vector<OpenInterface::note_t> no_song;
	
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.song(1, no_song));
}

TEST_F(AllSystemsGoOIModePASSIVE, song$WHENSongIsMoreThan16NotesTHENParameterIsInvalid) {
	std::vector<OpenInterface::note_t> fur_elise_ep;
	fur_elise_ep.push_back(std::make_pair(E4, 11));
	fur_elise_ep.push_back(std::make_pair(D4_SHARP, 11));
	fur_elise_ep.push_back(std::make_pair(E4, 11));
	fur_elise_ep.push_back(std::make_pair(D4_SHARP, 11));
	fur_elise_ep.push_back(std::make_pair(E4, 11));
	fur_elise_ep.push_back(std::make_pair(B3, 11));
	fur_elise_ep.push_back(std::make_pair(D4, 11));
	fur_elise_ep.push_back(std::make_pair(C4, 11));
	fur_elise_ep.push_back(std::make_pair(A3, 32));
	fur_elise_ep.push_back(std::make_pair(A3, 21));
	fur_elise_ep.push_back(std::make_pair(A3, 11));
	fur_elise_ep.push_back(std::make_pair(B3, 32));
	fur_elise_ep.push_back(std::make_pair(B3, 21));
	fur_elise_ep.push_back(std::make_pair(B3, 11));
	fur_elise_ep.push_back(std::make_pair(C4, 43));
	fur_elise_ep.push_back(std::make_pair(E4, 11));
	fur_elise_ep.push_back(std::make_pair(D4_SHARP, 11));
	
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.song(1, fur_elise_ep));
}

TEST_F(AllSystemsGoOIModePASSIVE, song$WHENParametersAreInvalidTHENNoDataIsWrittenToSerialBus) {
	std::vector<OpenInterface::note_t> fur_elise;
	fur_elise.push_back(std::make_pair(E4, 11));
	fur_elise.push_back(std::make_pair(D4_SHARP, 11));
	fur_elise.push_back(std::make_pair(E4, 11));
	fur_elise.push_back(std::make_pair(D4_SHARP, 11));
	fur_elise.push_back(std::make_pair(E4, 11));
	fur_elise.push_back(std::make_pair(B3, 11));
	fur_elise.push_back(std::make_pair(D4, 11));
	fur_elise.push_back(std::make_pair(C4, 11));
	fur_elise.push_back(std::make_pair(A3, 32));
	
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.song(5, fur_elise));
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModeOFF, song$WHENOIModeIsOffTHENReturnsError) {
	std::vector<OpenInterface::note_t> fur_elise;
	fur_elise.push_back(std::make_pair(E4, 11));
	fur_elise.push_back(std::make_pair(D4_SHARP, 11));
	fur_elise.push_back(std::make_pair(E4, 11));
	fur_elise.push_back(std::make_pair(D4_SHARP, 11));
	fur_elise.push_back(std::make_pair(E4, 11));
	fur_elise.push_back(std::make_pair(B3, 11));
	fur_elise.push_back(std::make_pair(D4, 11));
	fur_elise.push_back(std::make_pair(C4, 11));
	fur_elise.push_back(std::make_pair(A3, 32));
	
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.song(1, fur_elise));
}

TEST_F(SerialTransactionFailureOIModePASSIVE, song$WHENfnSerialWriteFailsTHENReturnsError) {
	std::vector<OpenInterface::note_t> fur_elise;
	fur_elise.push_back(std::make_pair(E4, 11));
	fur_elise.push_back(std::make_pair(D4_SHARP, 11));
	fur_elise.push_back(std::make_pair(E4, 11));
	fur_elise.push_back(std::make_pair(D4_SHARP, 11));
	fur_elise.push_back(std::make_pair(E4, 11));
	fur_elise.push_back(std::make_pair(B3, 11));
	fur_elise.push_back(std::make_pair(D4, 11));
	fur_elise.push_back(std::make_pair(C4, 11));
	fur_elise.push_back(std::make_pair(A3, 32));
	
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.song(1, fur_elise));
}

TEST_F(AllSystemsGoOIModeOFF, song$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	std::vector<OpenInterface::note_t> fur_elise;
	fur_elise.push_back(std::make_pair(E4, 11));
	fur_elise.push_back(std::make_pair(D4_SHARP, 11));
	fur_elise.push_back(std::make_pair(E4, 11));
	fur_elise.push_back(std::make_pair(D4_SHARP, 11));
	fur_elise.push_back(std::make_pair(E4, 11));
	fur_elise.push_back(std::make_pair(B3, 11));
	fur_elise.push_back(std::make_pair(D4, 11));
	fur_elise.push_back(std::make_pair(C4, 11));
	fur_elise.push_back(std::make_pair(A3, 32));
	
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.song(1, fur_elise));
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModeFULL, play$WHENCalledTHEN141AndParametersAreWrittenToTheSerialBus) {
	OI_tc.play(1);
	
	ASSERT_EQ(141, static_cast<uint8_t>(serial_bus[0]));
	EXPECT_EQ(1, static_cast<uint8_t>(serial_bus[1]));
}

TEST_F(AllSystemsGoOIModeFULL, play$WHENSongNumberIsGreaterThan4THENParameterIsInvalid) {
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.play(5));
}

TEST_F(AllSystemsGoOIModeFULL, play$WHENParametersAreInvalidTHENNoDataIsWrittenToSerialBus) {
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.play(5));
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModeOFF, play$WHENOIModeIsOffTHENReturnsError) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.play(1));
}

TEST_F(SerialTransactionFailureOIModeFULL, play$WHENfnSerialWriteFailsTHENReturnsError) {
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.play(1));
}

TEST_F(AllSystemsGoOIModeOFF, play$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.play(1));
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModePASSIVE, play$WHENOIModeIsPassiveTHENReturnsError) {
	ASSERT_EQ(OpenInterface::INVALID_MODE_FOR_REQUESTED_OPERATION, OI_tc.play(1));
}

TEST_F(AllSystemsGoOIModePASSIVE, play$WHENOIModeIsPassiveTHENNoDataIsWrittenToSerialBus) {
	ASSERT_EQ(OpenInterface::INVALID_MODE_FOR_REQUESTED_OPERATION, OI_tc.play(1));
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModePASSIVE, seekDock$WHENCalledTHEN143IsWrittenToTheSerialBus) {
	OI_tc.seekDock();
	ASSERT_EQ(143, static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModeOFF, seekDock$WHENOIModeIsOffTHENReturnsError) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.seekDock());
}

TEST_F(AllSystemsGoOIModeFULL, seekDock$WHENCalledTHENModeIsSetToPassive) {
	OI_tc.seekDock();
	ASSERT_EQ(PASSIVE, OI_tc._mode);
}

TEST_F(SerialTransactionFailureOIModePASSIVE, seekDock$WHENfnSerialWriteFailsTHENReturnsError) {
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.seekDock());
}

TEST_F(AllSystemsGoOIModeOFF, seekDock$WHENReturnsOINotStartedErrorTHENModeIsUnchanged) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.seekDock());
	ASSERT_EQ(OFF, OI_tc._mode);
}

TEST_F(SerialTransactionFailureOIModeFULL, seekDock$WHENReturnsSerialTransferFailureErrorTHENModeIsUnchanged) {
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.seekDock());
	ASSERT_EQ(FULL, OI_tc._mode);
}

TEST_F(AllSystemsGoOIModeOFF, seekDock$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.seekDock());
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModeFULL, pwmMotors$WHENCalledTHEN144AndParametersAreWrittenToTheSerialBus) {
	OI_tc.pwmMotors(96, -64, 127);
	
	ASSERT_EQ(144, static_cast<uint8_t>(serial_bus[0]));
	EXPECT_EQ(96, static_cast<uint8_t>(serial_bus[1]));
	EXPECT_EQ(192, static_cast<uint8_t>(serial_bus[2]));
	EXPECT_EQ(127, static_cast<uint8_t>(serial_bus[3]));
}

TEST_F(AllSystemsGoOIModeFULL, pwmMotors$WHENMainBrushIsLessThanNegative127THENParameterIsInvalid) {
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.pwmMotors(-128, -127, 0));
}

TEST_F(AllSystemsGoOIModeFULL, pwmMotors$WHENSideBrushIsLessThanNegative127THENParameterIsInvalid) {
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.pwmMotors(-127, -128, 0));
}

TEST_F(AllSystemsGoOIModeFULL, pwmMotors$WHENVacuumIsLessThanZeroTHENParameterIsInvalid) {
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.pwmMotors(-127, -127, -1));
}

TEST_F(AllSystemsGoOIModeFULL, pwmMotors$WHENParametersAreInvalidTHENNoDataIsWrittenToSerialBus) {
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.pwmMotors(-127, -128, 0));
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModeOFF, pwmMotors$WHENOIModeIsOffTHENReturnsError) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.pwmMotors(96, -64, 127));
}

TEST_F(SerialTransactionFailureOIModeFULL, pwmMotors$WHENfnSerialWriteFailsTHENReturnsError) {
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.pwmMotors(96, -64, 127));
}

TEST_F(AllSystemsGoOIModeOFF, pwmMotors$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.pwmMotors(96, -64, 127));
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModePASSIVE, pwmMotors$WHENOIModeIsPassiveTHENReturnsError) {
	ASSERT_EQ(OpenInterface::INVALID_MODE_FOR_REQUESTED_OPERATION, OI_tc.pwmMotors(96, -64, 127));
}

TEST_F(AllSystemsGoOIModePASSIVE, pwmMotors$WHENOIModeIsPassiveTHENNoDataIsWrittenToSerialBus) {
	ASSERT_EQ(OpenInterface::INVALID_MODE_FOR_REQUESTED_OPERATION, OI_tc.pwmMotors(96, -64, 127));
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModeFULL, driveDirect$WHENCalledTHEN145AndParametersAreWrittenToTheSerialBus) {
	OI_tc.driveDirect(-150, 150);
	
	ASSERT_EQ(145, static_cast<uint8_t>(serial_bus[0]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[1]));
	EXPECT_EQ(150, static_cast<uint8_t>(serial_bus[2]));
	EXPECT_EQ(255, static_cast<uint8_t>(serial_bus[3]));
	EXPECT_EQ(106, static_cast<uint8_t>(serial_bus[4]));
}

TEST_F(AllSystemsGoOIModeFULL, driveDirect$WHENLeftWheelVelocityIsGreaterThan500THENParameterIsInvalid) {
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.driveDirect(501, 500));
}

TEST_F(AllSystemsGoOIModeFULL, driveDirect$WHENLeftWheelVelocityIsLessThanNegative500THENParameterIsInvalid) {
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.driveDirect(-501, -500));
}

TEST_F(AllSystemsGoOIModeFULL, driveDirect$WHENRightWheelVelocityIsGreaterThan500THENParameterIsInvalid) {
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.driveDirect(500, 501));
}

TEST_F(AllSystemsGoOIModeFULL, driveDirect$WHENRightWheelVelocityIsLessThanNegative500THENParameterIsInvalid) {
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.driveDirect(-500, -501));
}

TEST_F(AllSystemsGoOIModeFULL, driveDirect$WHENParametersAreInvalidTHENNoDataIsWrittenToSerialBus) {
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.driveDirect(501, 500));
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModeOFF, driveDirect$WHENOIModeIsOffTHENReturnsError) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.driveDirect(-150, 150));
}

TEST_F(SerialTransactionFailureOIModeFULL, driveDirect$WHENfnSerialWriteFailsTHENReturnsError) {
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.driveDirect(-150, 150));
}

TEST_F(AllSystemsGoOIModeOFF, driveDirect$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.driveDirect(-150, 150));
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModePASSIVE, driveDirect$WHENOIModeIsPassiveTHENReturnsError) {
	ASSERT_EQ(OpenInterface::INVALID_MODE_FOR_REQUESTED_OPERATION, OI_tc.driveDirect(-150, 150));
}

TEST_F(AllSystemsGoOIModePASSIVE, driveDirect$WHENOIModeIsPassiveTHENNoDataIsWrittenToSerialBus) {
	ASSERT_EQ(OpenInterface::INVALID_MODE_FOR_REQUESTED_OPERATION, OI_tc.driveDirect(-150, 150));
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModeFULL, drivePWM$WHENCalledTHEN146AndParametersAreWrittenToTheSerialBus) {
	OI_tc.drivePWM(-32, 32);
	
	ASSERT_EQ(146, static_cast<uint8_t>(serial_bus[0]));
	EXPECT_EQ(0, static_cast<uint8_t>(serial_bus[1]));
	EXPECT_EQ(32, static_cast<uint8_t>(serial_bus[2]));
	EXPECT_EQ(255, static_cast<uint8_t>(serial_bus[3]));
	EXPECT_EQ(224, static_cast<uint8_t>(serial_bus[4]));
}

TEST_F(AllSystemsGoOIModeFULL, drivePWM$WHENLeftWheelVelocityIsGreaterThan255THENParameterIsInvalid) {
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.drivePWM(256, 255));
}

TEST_F(AllSystemsGoOIModeFULL, drivePWM$WHENLeftWheelVelocityIsLessThanNegative255THENParameterIsInvalid) {
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.drivePWM(-256, -255));
}

TEST_F(AllSystemsGoOIModeFULL, drivePWM$WHENRightWheelVelocityIsGreaterThan255THENParameterIsInvalid) {
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.drivePWM(255, 256));
}

TEST_F(AllSystemsGoOIModeFULL, drivePWM$WHENRightWheelVelocityIsLessThanNegative255THENParameterIsInvalid) {
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.drivePWM(-255, -256));
}

TEST_F(AllSystemsGoOIModeFULL, drivePWM$WHENParametersAreInvalidTHENNoDataIsWrittenToSerialBus) {
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.drivePWM(501, 500));
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModeOFF, drivePWM$WHENOIModeIsOffTHENReturnsError) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.drivePWM(-32, 32));
}

TEST_F(SerialTransactionFailureOIModeFULL, drivePWM$WHENfnSerialWriteFailsTHENReturnsError) {
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.drivePWM(-32, 32));
}

TEST_F(AllSystemsGoOIModeOFF, drivePWM$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.drivePWM(-32, 32));
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModePASSIVE, drivePWM$WHENOIModeIsPassiveTHENReturnsError) {
	ASSERT_EQ(OpenInterface::INVALID_MODE_FOR_REQUESTED_OPERATION, OI_tc.drivePWM(-32, 32));
}

TEST_F(AllSystemsGoOIModePASSIVE, drivePWM$WHENOIModeIsPassiveTHENNoDataIsWrittenToSerialBus) {
	ASSERT_EQ(OpenInterface::INVALID_MODE_FOR_REQUESTED_OPERATION, OI_tc.drivePWM(-32, 32));
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModeFULL, schedulingLEDs$WHENCalledTHEN162AndParametersAreWrittenToTheSerialBus) {
	OI_tc.schedulingLEDs(static_cast<bitmask::Days>(bitmask::TUESDAY | bitmask::SATURDAY), static_cast<bitmask::display::SchedulingLEDs>(bitmask::display::CLOCK | bitmask::display::COLON | bitmask::display::PM));
	
	ASSERT_EQ(162, static_cast<uint8_t>(serial_bus[0]));
	EXPECT_EQ(68, static_cast<uint8_t>(serial_bus[1]));
	EXPECT_EQ(11, static_cast<uint8_t>(serial_bus[2]));
}

TEST_F(AllSystemsGoOIModeFULL, schedulingLEDs$WHENDayMaskIsGreaterThan127THENParameterIsInvalid) {
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.schedulingLEDs(static_cast<bitmask::Days>(128), static_cast<bitmask::display::SchedulingLEDs>(bitmask::display::CLOCK | bitmask::display::COLON | bitmask::display::PM)));
}

TEST_F(AllSystemsGoOIModeFULL, schedulingLEDs$WHENDisplayMaskIsGreaterThan31THENParameterIsInvalid) {
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.schedulingLEDs(static_cast<bitmask::Days>(bitmask::TUESDAY | bitmask::SATURDAY), static_cast<bitmask::display::SchedulingLEDs>(32)));
}

TEST_F(AllSystemsGoOIModeFULL, schedulingLEDs$WHENParametersAreInvalidTHENNoDataIsWrittenToSerialBus) {
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.schedulingLEDs(static_cast<bitmask::Days>(bitmask::TUESDAY | bitmask::SATURDAY), static_cast<bitmask::display::SchedulingLEDs>(32)));
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModeOFF, schedulingLEDs$WHENOIModeIsOffTHENReturnsError) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.schedulingLEDs(static_cast<bitmask::Days>(bitmask::TUESDAY | bitmask::SATURDAY), static_cast<bitmask::display::SchedulingLEDs>(bitmask::display::CLOCK | bitmask::display::COLON | bitmask::display::PM)));
}

TEST_F(SerialTransactionFailureOIModeFULL, schedulingLEDs$WHENfnSerialWriteFailsTHENReturnsError) {
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.schedulingLEDs(static_cast<bitmask::Days>(bitmask::TUESDAY | bitmask::SATURDAY), static_cast<bitmask::display::SchedulingLEDs>(bitmask::display::CLOCK | bitmask::display::COLON | bitmask::display::PM)));
}

TEST_F(AllSystemsGoOIModeOFF, schedulingLEDs$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.schedulingLEDs(static_cast<bitmask::Days>(bitmask::TUESDAY | bitmask::SATURDAY), static_cast<bitmask::display::SchedulingLEDs>(bitmask::display::CLOCK | bitmask::display::COLON | bitmask::display::PM)));
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModePASSIVE, schedulingLEDs$WHENOIModeIsPassiveTHENReturnsError) {
	ASSERT_EQ(OpenInterface::INVALID_MODE_FOR_REQUESTED_OPERATION, OI_tc.schedulingLEDs(static_cast<bitmask::Days>(bitmask::TUESDAY | bitmask::SATURDAY), static_cast<bitmask::display::SchedulingLEDs>(bitmask::display::CLOCK | bitmask::display::COLON | bitmask::display::PM)));
}

TEST_F(AllSystemsGoOIModePASSIVE, schedulingLEDs$WHENOIModeIsPassiveTHENNoDataIsWrittenToSerialBus) {
	ASSERT_EQ(OpenInterface::INVALID_MODE_FOR_REQUESTED_OPERATION, OI_tc.schedulingLEDs(static_cast<bitmask::Days>(bitmask::TUESDAY | bitmask::SATURDAY), static_cast<bitmask::display::SchedulingLEDs>(bitmask::display::CLOCK | bitmask::display::COLON | bitmask::display::PM)));
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModeFULL, digitLEDsRaw$WHENCalledTHEN163AndParametersAreWrittenToTheSerialBus) {
	bitmask::display::DigitN seven_segments[4] = {
		static_cast<bitmask::display::DigitN>(bitmask::display::A | bitmask::display::B),
		static_cast<bitmask::display::DigitN>(bitmask::display::C | bitmask::display::D),
		static_cast<bitmask::display::DigitN>(bitmask::display::E | bitmask::display::F),
		static_cast<bitmask::display::DigitN>(bitmask::display::G | bitmask::display::A)
	};
	
	OI_tc.digitLEDsRaw(seven_segments);
	
	ASSERT_EQ(163, static_cast<uint8_t>(serial_bus[0]));
	EXPECT_EQ(3, static_cast<uint8_t>(serial_bus[1]));
	EXPECT_EQ(12, static_cast<uint8_t>(serial_bus[2]));
	EXPECT_EQ(48, static_cast<uint8_t>(serial_bus[3]));
	EXPECT_EQ(65, static_cast<uint8_t>(serial_bus[4]));
}

TEST_F(AllSystemsGoOIModeFULL, digitLEDsRaw$WHENDigit3MaskIsGreaterThan127THENParameterIsInvalid) {
	bitmask::display::DigitN seven_segments[4] = {
		static_cast<bitmask::display::DigitN>(128),
		static_cast<bitmask::display::DigitN>(bitmask::display::C | bitmask::display::D),
		static_cast<bitmask::display::DigitN>(bitmask::display::E | bitmask::display::F),
		static_cast<bitmask::display::DigitN>(bitmask::display::G | bitmask::display::A)
	};
	
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.digitLEDsRaw(seven_segments));
}

TEST_F(AllSystemsGoOIModeFULL, digitLEDsRaw$WHENDigit2MaskIsGreaterThan127THENParameterIsInvalid) {
	bitmask::display::DigitN seven_segments[4] = {
		static_cast<bitmask::display::DigitN>(bitmask::display::A | bitmask::display::B),
		static_cast<bitmask::display::DigitN>(128),
		static_cast<bitmask::display::DigitN>(bitmask::display::E | bitmask::display::F),
		static_cast<bitmask::display::DigitN>(bitmask::display::G | bitmask::display::A)
	};
	
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.digitLEDsRaw(seven_segments));
}

TEST_F(AllSystemsGoOIModeFULL, digitLEDsRaw$WHENDigit1MaskIsGreaterThan127THENParameterIsInvalid) {
	bitmask::display::DigitN seven_segments[4] = {
		static_cast<bitmask::display::DigitN>(bitmask::display::A | bitmask::display::B),
		static_cast<bitmask::display::DigitN>(bitmask::display::C | bitmask::display::D),
		static_cast<bitmask::display::DigitN>(128),
		static_cast<bitmask::display::DigitN>(bitmask::display::G | bitmask::display::A)
	};
	
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.digitLEDsRaw(seven_segments));
}

TEST_F(AllSystemsGoOIModeFULL, digitLEDsRaw$WHENDigit0MaskIsGreaterThan127THENParameterIsInvalid) {
	bitmask::display::DigitN seven_segments[4] = {
		static_cast<bitmask::display::DigitN>(bitmask::display::A | bitmask::display::B),
		static_cast<bitmask::display::DigitN>(bitmask::display::C | bitmask::display::D),
		static_cast<bitmask::display::DigitN>(bitmask::display::E | bitmask::display::F),
		static_cast<bitmask::display::DigitN>(128)
	};
	
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.digitLEDsRaw(seven_segments));
}

TEST_F(AllSystemsGoOIModeFULL, digitLEDsRaw$WHENParametersAreInvalidTHENNoDataIsWrittenToSerialBus) {
	bitmask::display::DigitN seven_segments[4] = {
		static_cast<bitmask::display::DigitN>(bitmask::display::A | bitmask::display::B),
		static_cast<bitmask::display::DigitN>(bitmask::display::C | bitmask::display::D),
		static_cast<bitmask::display::DigitN>(bitmask::display::E | bitmask::display::F),
		static_cast<bitmask::display::DigitN>(128)
	};
	
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.digitLEDsRaw(seven_segments));
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModeOFF, digitLEDsRaw$WHENOIModeIsOffTHENReturnsError) {
	bitmask::display::DigitN seven_segments[4] = {
		static_cast<bitmask::display::DigitN>(bitmask::display::A | bitmask::display::B),
		static_cast<bitmask::display::DigitN>(bitmask::display::C | bitmask::display::D),
		static_cast<bitmask::display::DigitN>(bitmask::display::E | bitmask::display::F),
		static_cast<bitmask::display::DigitN>(bitmask::display::G | bitmask::display::A)
	};
	
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.digitLEDsRaw(seven_segments));
}

TEST_F(SerialTransactionFailureOIModeFULL, digitLEDsRaw$WHENfnSerialWriteFailsTHENReturnsError) {
	bitmask::display::DigitN seven_segments[4] = {
		static_cast<bitmask::display::DigitN>(bitmask::display::A | bitmask::display::B),
		static_cast<bitmask::display::DigitN>(bitmask::display::C | bitmask::display::D),
		static_cast<bitmask::display::DigitN>(bitmask::display::E | bitmask::display::F),
		static_cast<bitmask::display::DigitN>(bitmask::display::G | bitmask::display::A)
	};
	
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.digitLEDsRaw(seven_segments));
}

TEST_F(AllSystemsGoOIModeOFF, digitLEDsRaw$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	bitmask::display::DigitN seven_segments[4] = {
		static_cast<bitmask::display::DigitN>(bitmask::display::A | bitmask::display::B),
		static_cast<bitmask::display::DigitN>(bitmask::display::C | bitmask::display::D),
		static_cast<bitmask::display::DigitN>(bitmask::display::E | bitmask::display::F),
		static_cast<bitmask::display::DigitN>(bitmask::display::G | bitmask::display::A)
	};
	
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.digitLEDsRaw(seven_segments));
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModePASSIVE, digitLEDsRaw$WHENOIModeIsPassiveTHENReturnsError) {
	bitmask::display::DigitN seven_segments[4] = {
		static_cast<bitmask::display::DigitN>(bitmask::display::A | bitmask::display::B),
		static_cast<bitmask::display::DigitN>(bitmask::display::C | bitmask::display::D),
		static_cast<bitmask::display::DigitN>(bitmask::display::E | bitmask::display::F),
		static_cast<bitmask::display::DigitN>(bitmask::display::G | bitmask::display::A)
	};
	
	ASSERT_EQ(OpenInterface::INVALID_MODE_FOR_REQUESTED_OPERATION, OI_tc.digitLEDsRaw(seven_segments));
}

TEST_F(AllSystemsGoOIModePASSIVE, digitLEDsRaw$WHENOIModeIsPassiveTHENNoDataIsWrittenToSerialBus) {
	bitmask::display::DigitN seven_segments[4] = {
		static_cast<bitmask::display::DigitN>(bitmask::display::A | bitmask::display::B),
		static_cast<bitmask::display::DigitN>(bitmask::display::C | bitmask::display::D),
		static_cast<bitmask::display::DigitN>(bitmask::display::E | bitmask::display::F),
		static_cast<bitmask::display::DigitN>(bitmask::display::G | bitmask::display::A)
	};
	
	ASSERT_EQ(OpenInterface::INVALID_MODE_FOR_REQUESTED_OPERATION, OI_tc.digitLEDsRaw(seven_segments));
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModeFULL, digitLEDsASCII$WHENCalledTHEN164AndParametersAreWrittenToTheSerialBus) {
	char seven_segments[4] = { 'Z', 'A', 'K', '!' };
	
	OI_tc.digitLEDsASCII(seven_segments);
	
	ASSERT_EQ(164, static_cast<uint8_t>(serial_bus[0]));
	EXPECT_EQ(90, static_cast<uint8_t>(serial_bus[1]));
	EXPECT_EQ(65, static_cast<uint8_t>(serial_bus[2]));
	EXPECT_EQ(75, static_cast<uint8_t>(serial_bus[3]));
	EXPECT_EQ(33, static_cast<uint8_t>(serial_bus[4]));
}

TEST_F(AllSystemsGoOIModeFULL, digitLEDsASCII$WHENDigit3IsGreaterThan126THENParameterIsInvalid) {
	char seven_segments[4] = { static_cast<char>(127), 'A', 'K', '!' };
	
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.digitLEDsASCII(seven_segments));
}

TEST_F(AllSystemsGoOIModeFULL, digitLEDsASCII$WHENDigit3IsLessThan32THENParameterIsInvalid) {
	char seven_segments[4] = { static_cast<char>(31), 'A', 'K', '!' };
	
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.digitLEDsASCII(seven_segments));
}

TEST_F(AllSystemsGoOIModeFULL, digitLEDsASCII$WHENDigit2IsGreaterThan126THENParameterIsInvalid) {
	char seven_segments[4] = { 'Z', static_cast<char>(127), 'K', '!' };
	
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.digitLEDsASCII(seven_segments));
}

TEST_F(AllSystemsGoOIModeFULL, digitLEDsASCII$WHENDigit2IsLessThan32THENParameterIsInvalid) {
	char seven_segments[4] = { 'Z', static_cast<char>(31), 'K', '!' };
	
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.digitLEDsASCII(seven_segments));
}

TEST_F(AllSystemsGoOIModeFULL, digitLEDsASCII$WHENDigit1IsGreaterThan126THENParameterIsInvalid) {
	char seven_segments[4] = { 'Z', 'A', static_cast<char>(127), '!' };
	
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.digitLEDsASCII(seven_segments));
}

TEST_F(AllSystemsGoOIModeFULL, digitLEDsASCII$WHENDigit1IsLessThan32THENParameterIsInvalid) {
	char seven_segments[4] = { 'Z', 'A', static_cast<char>(31), '!' };
	
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.digitLEDsASCII(seven_segments));
}

TEST_F(AllSystemsGoOIModeFULL, digitLEDsASCII$WHENDigit0IsGreaterThan127THENParameterIsInvalid) {
	char seven_segments[4] = { 'Z', 'A', 'K', static_cast<char>(127) };
	
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.digitLEDsASCII(seven_segments));
}

TEST_F(AllSystemsGoOIModeFULL, digitLEDsASCII$WHENDigit0IsLessThan32THENParameterIsInvalid) {
	char seven_segments[4] = { 'Z', 'A', 'K', static_cast<char>(31) };
	
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.digitLEDsASCII(seven_segments));
}

TEST_F(AllSystemsGoOIModeFULL, digitLEDsASCII$WHENParametersAreInvalidTHENNoDataIsWrittenToSerialBus) {
	char seven_segments[4] = { 'Z', 'A', 'K', static_cast<char>(31) };
	
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.digitLEDsASCII(seven_segments));
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModeOFF, digitLEDsASCII$WHENOIModeIsOffTHENReturnsError) {
	char seven_segments[4] = { 'Z', 'A', 'K', '!' };
	
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.digitLEDsASCII(seven_segments));
}

TEST_F(SerialTransactionFailureOIModeFULL, digitLEDsASCII$WHENfnSerialWriteFailsTHENReturnsError) {
	char seven_segments[4] = { 'Z', 'A', 'K', '!' };
	
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.digitLEDsASCII(seven_segments));
}

TEST_F(AllSystemsGoOIModeOFF, digitLEDsASCII$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	char seven_segments[4] = { 'Z', 'A', 'K', '!' };
	
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.digitLEDsASCII(seven_segments));
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModePASSIVE, digitLEDsASCII$WHENOIModeIsPassiveTHENReturnsError) {
	char seven_segments[4] = { 'Z', 'A', 'K', '!' };
	
	ASSERT_EQ(OpenInterface::INVALID_MODE_FOR_REQUESTED_OPERATION, OI_tc.digitLEDsASCII(seven_segments));
}

TEST_F(AllSystemsGoOIModePASSIVE, digitLEDsASCII$WHENOIModeIsPassiveTHENNoDataIsWrittenToSerialBus) {
	char seven_segments[4] = { 'Z', 'A', 'K', '!' };
	
	ASSERT_EQ(OpenInterface::INVALID_MODE_FOR_REQUESTED_OPERATION, OI_tc.digitLEDsASCII(seven_segments));
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModePASSIVE, buttons$WHENCalledTHEN165AndParametersAreWrittenToTheSerialBus) {
	OI_tc.buttons(static_cast<bitmask::Buttons>(bitmask::SPOT | bitmask::CLEAN));
	
	ASSERT_EQ(165, static_cast<uint8_t>(serial_bus[0]));
	EXPECT_EQ(3, static_cast<uint8_t>(serial_bus[1]));
}

TEST_F(AllSystemsGoOIModeOFF, buttons$WHENOIModeIsOffTHENReturnsError) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.buttons(static_cast<bitmask::Buttons>(bitmask::SPOT | bitmask::CLEAN)));
}

TEST_F(SerialTransactionFailureOIModeFULL, buttons$WHENfnSerialWriteFailsTHENReturnsError) {
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.buttons(static_cast<bitmask::Buttons>(bitmask::SPOT | bitmask::CLEAN)));
}

TEST_F(AllSystemsGoOIModeOFF, buttons$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.buttons(static_cast<bitmask::Buttons>(bitmask::SPOT | bitmask::CLEAN)));
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModePASSIVE, schedule$WHENCalledTHEN167AndParametersAreWrittenToTheSerialBus) {
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

TEST_F(AllSystemsGoOIModePASSIVE, schedule$WHENCalledWithDisableInBitMaskTHEN167AndAllZerosAreWrittenToTheSerialBus) {
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

TEST_F(AllSystemsGoOIModePASSIVE, schedule$WHENCalledWithSparseCParametersTHENFullZeroFilledSerialParametersAreWrittenToTheSerialBus) {
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

TEST_F(AllSystemsGoOIModePASSIVE, schedule$WHENCalledWithNULLArrayTHEN167AndAllZerosAreWrittenToTheSerialBus) {
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

TEST_F(AllSystemsGoOIModePASSIVE, schedule$WHENCalledWithInvalidTimeParametersTHENCorrespondingDayIsIgnored) {
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

TEST_F(AllSystemsGoOIModePASSIVE, schedule$WHENDaysMaskIsGreaterThan127THENParameterIsInvalid) {
	bitmask::Days days = static_cast<bitmask::Days>(128);
	OpenInterface::clock_time_t clk_time[2];
	clk_time[0].hour = 15;
	clk_time[0].minute = 35;
	clk_time[1].hour = 10;
	
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.schedule(days, clk_time));
}

TEST_F(AllSystemsGoOIModePASSIVE, schedule$WHENParametersAreInvalidTHENNoDataIsWrittenToSerialBus) {
	bitmask::Days days = static_cast<bitmask::Days>(128);
	OpenInterface::clock_time_t clk_time[2];
	clk_time[0].hour = 15;
	clk_time[0].minute = 35;
	clk_time[1].hour = 10;
	
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.schedule(days, clk_time));
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModeOFF, schedule$WHENOIModeIsOffTHENReturnsError) {
	bitmask::Days days = static_cast<bitmask::Days>(bitmask::WEDNESDAY | bitmask::SUNDAY);
	OpenInterface::clock_time_t clk_time[2];
	clk_time[0].hour = 15;
	clk_time[0].minute = 35;
	clk_time[1].hour = 10;
	
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.schedule(days, clk_time));
}

TEST_F(SerialTransactionFailureOIModePASSIVE, schedule$WHENfnSerialWriteFailsTHENReturnsError) {
	bitmask::Days days = static_cast<bitmask::Days>(bitmask::WEDNESDAY | bitmask::SUNDAY);
	OpenInterface::clock_time_t clk_time[2];
	clk_time[0].hour = 15;
	clk_time[0].minute = 35;
	clk_time[1].hour = 10;
	
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.schedule(days, clk_time));
}

TEST_F(AllSystemsGoOIModeOFF, schedule$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	bitmask::Days days = static_cast<bitmask::Days>(bitmask::WEDNESDAY | bitmask::SUNDAY);
	OpenInterface::clock_time_t clk_time[2];
	clk_time[0].hour = 15;
	clk_time[0].minute = 35;
	clk_time[1].hour = 10;
	
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.schedule(days, clk_time));
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModePASSIVE, setDayTime$WHENCalledTHEN168AndParametersAreWrittenToTheSerialBus) {
	OI_tc.setDayTime(TUESDAY, OpenInterface::clock_time_t(11,23));
	
	ASSERT_EQ(168, static_cast<uint8_t>(serial_bus[0]));
	EXPECT_EQ(2, static_cast<uint8_t>(serial_bus[1]));
	EXPECT_EQ(11, static_cast<uint8_t>(serial_bus[2]));
	EXPECT_EQ(23, static_cast<uint8_t>(serial_bus[3]));
}

TEST_F(AllSystemsGoOIModePASSIVE, setDayTime$WHENHourIsGreaterThan23THENTimeParameterIsInvalid) {
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.setDayTime(TUESDAY, OpenInterface::clock_time_t(24,18)));
}

TEST_F(AllSystemsGoOIModePASSIVE, setDayTime$WHENHourIsLessThanZeroTHENTimeParameterIsInvalid) {
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.setDayTime(TUESDAY, OpenInterface::clock_time_t(-1,18)));
}

TEST_F(AllSystemsGoOIModePASSIVE, setDayTime$WHENMinuteIsGreaterThan60THENTimeParameterIsInvalid) {
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.setDayTime(TUESDAY, OpenInterface::clock_time_t(22,60)));
}

TEST_F(AllSystemsGoOIModePASSIVE, setDayTime$WHENMinuteIsLessThanZeroTHENTimeParameterIsInvalid) {
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.setDayTime(TUESDAY, OpenInterface::clock_time_t(22,-1)));
}

TEST_F(AllSystemsGoOIModePASSIVE, setDayTime$WHENTimeParameterIsInvalidTHENNoDataIsWrittenToSerialBus) {
	ASSERT_EQ(OpenInterface::INVALID_PARAMETER, OI_tc.setDayTime(TUESDAY, OpenInterface::clock_time_t(31,18)));
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModeOFF, setDayTime$WHENOIModeIsOffTHENReturnsError) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.setDayTime(TUESDAY, OpenInterface::clock_time_t(11,23)));
}

TEST_F(SerialTransactionFailureOIModePASSIVE, setDayTime$WHENfnSerialWriteFailsTHENReturnsError) {
	ASSERT_EQ(OpenInterface::SERIAL_TRANSFER_FAILURE, OI_tc.setDayTime(TUESDAY, OpenInterface::clock_time_t(11,23)));
}

TEST_F(AllSystemsGoOIModeOFF, setDayTime$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	ASSERT_EQ(OpenInterface::OI_NOT_STARTED, OI_tc.setDayTime(TUESDAY, OpenInterface::clock_time_t(11,23)));
	ASSERT_EQ('\0', static_cast<uint8_t>(serial_bus[0]));
}

} // namespace

/* Created and copyrighted by Zachary J. Fields. All rights reserved. */
