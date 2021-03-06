/* Created and copyrighted by Zachary J. Fields. Offered as open source under the MIT License (MIT). */

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../open_interface.h"
#include "MOCK_serial.h"

#ifndef DISABLE_SENSORS
  #include "TEST_state.h"
#endif

#include <cstring>

//TODO: Guarantee sensor packets are not requested more than every 15ms
//TODO: Guarantee requested data bytes will fit in 15ms time quantum at selected baud rate (see: stream())

//TODO: Maintain a handle to the thread (to be created at the inital invocation of stream(), queryList() or sensors()), ensuring it can be killed and restarted if necessary.
//TODO: Ensure _parse_key is not updated on fail cases

//TODO: Move from pseudo-static class to namespace, assuming testing framework allows
//TODO: Hardware test Roomba for reaction to bad bytes and reduce API data scrubbing if possible

using namespace roomba;

  /******************/
 /* TESTABLE CLASS */
/******************/
class OpenInterface_TC : public open_interface<OI500> {};

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
		serial::mock::setSerialWriteFunc(
			[] (
				const uint_opt8_t * const,
				const size_t
			) {
				return 0;
			}
		);
		//OI_tc.connectToSerialBus(, BAUD_115200);
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
		serial::mock::setSerialWriteFunc(
			[] (
				const uint_opt8_t * const,
				const size_t
			) {
				return 0;
			}
		);
		//OI_tc.connectToSerialBus(, BAUD_115200);
		state::setOIMode(PASSIVE);
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
		serial::mock::setSerialWriteFunc(
			[] (
				const uint_opt8_t * const,
				const size_t
			) {
				return 0;
			}
		);
		//OI_tc.connectToSerialBus(, BAUD_115200);
		state::setOIMode(FULL);
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
		serial::mock::setSerialWriteFunc(
			[this] (
				const uint_opt8_t * byte_array_,
				size_t length_
			) {
				length_ = ((length_ <= 64) * length_) + ((length_ > 64) * 64);
				memcpy(serial_bus, byte_array_, length_);
				return strnlen(serial_bus, 64);
			}
		);
		//OI_tc.connectToSerialBus(, BAUD_115200);
		state::setOIMode(OFF);
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
		serial::mock::setSerialWriteFunc(
			[this] (
				const uint_opt8_t * byte_array_,
				size_t length_
			) {
				length_ = ((length_ <= 64) * length_) + ((length_ > 64) * 64);
				memcpy(serial_bus, byte_array_, length_);
				return strnlen(serial_bus, 64);
			}
		);
		//OI_tc.connectToSerialBus(, BAUD_115200);
		state::setOIMode(PASSIVE);
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
		serial::mock::setSerialWriteFunc(
			[this] (
				const uint_opt8_t * byte_array_,
				size_t length_
			) {
				length_ = ((length_ <= 64) * length_) + ((length_ > 64) * 64);
				memcpy(serial_bus, byte_array_, length_);
				return strnlen(serial_bus, 64);
			}
		);
		//OI_tc.connectToSerialBus(, BAUD_115200);
		state::setOIMode(FULL);
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
(e.g. ASSERT_EQ(_EXPECTED_, _ACTUAL_))
*/

TEST_F(ObjectInitialization, constructor$WHENInitializedTHENOIModeWillBeSetToOFF) {
	ASSERT_EQ(state::testing::getOIMode(), OFF);
}

TEST_F(AllSystemsGoOIModeOFF, start$WHENCalledTHEN128IsWrittenToTheSerialBus) {
	OI_tc.start();
	ASSERT_EQ(128, static_cast<uint_opt8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModeOFF, start$WHENCalledTHENModeIsSetToPassive) {
	OI_tc.start();
	ASSERT_EQ(PASSIVE, state::testing::getOIMode());
}

TEST_F(SerialTransactionFailureOIModeOFF, start$WHENfnSerialWriteFailsTHENReturnsError) {
	ASSERT_EQ(SERIAL_TRANSFER_FAILURE, OI_tc.start());
}

TEST_F(SerialTransactionFailureOIModeOFF, start$WHENReturnsErrorTHENModeIsUnchanged) {
	ASSERT_EQ(SERIAL_TRANSFER_FAILURE, OI_tc.start());
	ASSERT_EQ(OFF, state::testing::getOIMode());
}

TEST_F(AllSystemsGoOIModePASSIVE, baud$WHENCalledTHEN129AndParametersAreWrittenToTheSerialBus) {
	OI_tc.baud(BAUD_57600);
	ASSERT_EQ(129, static_cast<uint_opt8_t>(serial_bus[0]));
	ASSERT_EQ(10, static_cast<uint_opt8_t>(serial_bus[1]));
}
#ifdef SENSORS_ENABLED
TEST_F(AllSystemsGoOIModePASSIVE, baud$WHENCalledTHENBaudCodeIsSet) {
	OI_tc.baud(BAUD_57600);
	ASSERT_EQ(BAUD_57600, state::testing::getBaudCode());
}
#endif
TEST_F(AllSystemsGoOIModePASSIVE, baud$WHENCalledTHENBlockFor100ms) {
	std::chrono::steady_clock::time_point begin, end;
	
	begin = std::chrono::steady_clock::now();
	OI_tc.baud(BAUD_57600);
	end = std::chrono::steady_clock::now();
	
	ASSERT_LE(100, (std::chrono::duration_cast<std::chrono::milliseconds>(end - begin)).count());
}

TEST_F(AllSystemsGoOIModePASSIVE, baud$WHENBaudCodeIsGreaterThan11THENParameterIsInvalid) {
	for ( int i = 12 ; i <= 255 ; ++i ) {
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.baud(static_cast<BaudCode>(i))) << "Accepted value <" << i << ">!";
	}
}

TEST_F(AllSystemsGoOIModePASSIVE, baud$WHENParameterIsInvalidTHENNoDataIsWrittenToSerialBus) {
	for ( int i = 12 ; i <= 255 ; ++i ) {
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.baud(static_cast<BaudCode>(i)));
		ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
	}
}
#ifdef SENSORS_ENABLED
TEST_F(AllSystemsGoOIModePASSIVE, baud$WHENParameterIsInvalidTHENBaudCodeIsNotSet) {
	for ( int i = 12 ; i <= 255 ; ++i ) {
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.baud(static_cast<BaudCode>(i)));
		ASSERT_EQ(BAUD_115200, state::testing::getBaudCode());
	}
}
#endif
TEST_F(AllSystemsGoOIModeOFF, baud$WHENOIModeIsOffTHENReturnsError) {
	ASSERT_EQ(OI_NOT_STARTED, OI_tc.baud(BAUD_57600));
}
#ifdef SENSORS_ENABLED
TEST_F(AllSystemsGoOIModeOFF, baud$WHENOIModeIsOffTHENBaudCodeIsNotSet) {
	ASSERT_EQ(OI_NOT_STARTED, OI_tc.baud(BAUD_57600));
	ASSERT_EQ(BAUD_115200, state::testing::getBaudCode());
}
#endif
TEST_F(SerialTransactionFailureOIModePASSIVE, baud$WHENfnSerialWriteFailsTHENReturnsError) {
	ASSERT_EQ(SERIAL_TRANSFER_FAILURE, OI_tc.baud(BAUD_57600));
}
#ifdef SENSORS_ENABLED
TEST_F(SerialTransactionFailureOIModePASSIVE, baud$WHENfnSerialWriteFailsTHENBaudCodeIsNotSet) {
	ASSERT_EQ(SERIAL_TRANSFER_FAILURE, OI_tc.baud(BAUD_57600));
	ASSERT_EQ(BAUD_115200, state::testing::getBaudCode());
}
#endif
TEST_F(AllSystemsGoOIModeOFF, baud$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	EXPECT_EQ(OI_NOT_STARTED, OI_tc.baud(BAUD_57600));
	ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
}

TEST_F(AllSystemsGoOIModePASSIVE, safe$WHENCalledTHEN131IsWrittenToTheSerialBus) {
	OI_tc.safe();
	ASSERT_EQ(131, static_cast<uint_opt8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModePASSIVE, safe$WHENCalledTHENModeIsSetToSafe) {
	OI_tc.safe();
	ASSERT_EQ(SAFE, state::testing::getOIMode());
}

TEST_F(AllSystemsGoOIModeOFF, safe$WHENOIModeIsOffTHENReturnsError) {
	ASSERT_EQ(OI_NOT_STARTED, OI_tc.safe());
}

TEST_F(SerialTransactionFailureOIModePASSIVE, safe$WHENfnSerialWriteFailsTHENReturnsError) {
	ASSERT_EQ(SERIAL_TRANSFER_FAILURE, OI_tc.safe());
}

TEST_F(AllSystemsGoOIModeOFF, safe$WHENReturnsOINotStartedErrorTHENModeIsUnchanged) {
	ASSERT_EQ(OI_NOT_STARTED, OI_tc.safe());
	ASSERT_EQ(OFF, state::testing::getOIMode());
}

TEST_F(SerialTransactionFailureOIModePASSIVE, safe$WHENReturnsSerialTransferFailureErrorTHENModeIsUnchanged) {
	ASSERT_EQ(SERIAL_TRANSFER_FAILURE, OI_tc.safe());
	ASSERT_EQ(PASSIVE, state::testing::getOIMode());
}

TEST_F(AllSystemsGoOIModeOFF, safe$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	EXPECT_EQ(OI_NOT_STARTED, OI_tc.safe());
	ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
}

TEST_F(AllSystemsGoOIModePASSIVE, full$WHENCalledTHEN132IsWrittenToTheSerialBus) {
	OI_tc.full();
	ASSERT_EQ(132, static_cast<uint_opt8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModePASSIVE, full$WHENCalledTHENModeIsSetToFull) {
	OI_tc.full();
	ASSERT_EQ(FULL, state::testing::getOIMode());
}

TEST_F(AllSystemsGoOIModeOFF, full$WHENOIModeIsOffTHENReturnsError) {
	ASSERT_EQ(OI_NOT_STARTED, OI_tc.full());
}

TEST_F(SerialTransactionFailureOIModePASSIVE, full$WHENfnSerialWriteFailsTHENReturnsError) {
	ASSERT_EQ(SERIAL_TRANSFER_FAILURE, OI_tc.full());
}

TEST_F(AllSystemsGoOIModeOFF, full$WHENReturnsOINotStartedErrorTHENModeIsUnchanged) {
	ASSERT_EQ(OI_NOT_STARTED, OI_tc.full());
	ASSERT_EQ(OFF, state::testing::getOIMode());
}

TEST_F(SerialTransactionFailureOIModePASSIVE, full$WHENReturnsSerialTransferFailureErrorTHENModeIsUnchanged) {
	ASSERT_EQ(SERIAL_TRANSFER_FAILURE, OI_tc.full());
	ASSERT_EQ(PASSIVE, state::testing::getOIMode());
}

TEST_F(AllSystemsGoOIModeOFF, full$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	EXPECT_EQ(OI_NOT_STARTED, OI_tc.full());
	ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
}

TEST_F(AllSystemsGoOIModePASSIVE, power$WHENCalledTHEN133IsWrittenToTheSerialBus) {
	OI_tc.power();
	ASSERT_EQ(133, static_cast<uint_opt8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModeFULL, power$WHENCalledTHENModeIsSetToPassive) {
	OI_tc.power();
	ASSERT_EQ(PASSIVE, state::testing::getOIMode());
}

TEST_F(AllSystemsGoOIModeOFF, power$WHENOIModeIsOffTHENReturnsError) {
	ASSERT_EQ(OI_NOT_STARTED, OI_tc.power());
}

TEST_F(SerialTransactionFailureOIModePASSIVE, power$WHENfnSerialWriteFailsTHENReturnsError) {
	ASSERT_EQ(SERIAL_TRANSFER_FAILURE, OI_tc.power());
}

TEST_F(AllSystemsGoOIModeOFF, power$WHENReturnsOINotStartedErrorTHENModeIsUnchanged) {
	ASSERT_EQ(OI_NOT_STARTED, OI_tc.power());
	ASSERT_EQ(OFF, state::testing::getOIMode());
}

TEST_F(SerialTransactionFailureOIModeFULL, power$WHENReturnsSerialTransferFailureErrorTHENModeIsUnchanged) {
	ASSERT_EQ(SERIAL_TRANSFER_FAILURE, OI_tc.power());
	ASSERT_EQ(FULL, state::testing::getOIMode());
}

TEST_F(AllSystemsGoOIModeOFF, power$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	EXPECT_EQ(OI_NOT_STARTED, OI_tc.power());
	ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
}

TEST_F(AllSystemsGoOIModePASSIVE, spot$WHENCalledTHEN134IsWrittenToTheSerialBus) {
	OI_tc.spot();
	ASSERT_EQ(134, static_cast<uint_opt8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModeOFF, spot$WHENOIModeIsOffTHENReturnsError) {
	ASSERT_EQ(OI_NOT_STARTED, OI_tc.spot());
}

TEST_F(AllSystemsGoOIModeFULL, spot$WHENCalledTHENModeIsSetToPassive) {
	OI_tc.spot();
	ASSERT_EQ(PASSIVE, state::testing::getOIMode());
}

TEST_F(SerialTransactionFailureOIModePASSIVE, spot$WHENfnSerialWriteFailsTHENReturnsError) {
	ASSERT_EQ(SERIAL_TRANSFER_FAILURE, OI_tc.spot());
}

TEST_F(AllSystemsGoOIModeOFF, spot$WHENReturnsOINotStartedErrorTHENModeIsUnchanged) {
	ASSERT_EQ(OI_NOT_STARTED, OI_tc.spot());
	ASSERT_EQ(OFF, state::testing::getOIMode());
}

TEST_F(SerialTransactionFailureOIModeFULL, spot$WHENReturnsSerialTransferFailureErrorTHENModeIsUnchanged) {
	ASSERT_EQ(SERIAL_TRANSFER_FAILURE, OI_tc.spot());
	ASSERT_EQ(FULL, state::testing::getOIMode());
}

TEST_F(AllSystemsGoOIModeOFF, spot$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	EXPECT_EQ(OI_NOT_STARTED, OI_tc.spot());
	ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
}

TEST_F(AllSystemsGoOIModePASSIVE, clean$WHENCalledTHEN135IsWrittenToTheSerialBus) {
	OI_tc.clean();
	ASSERT_EQ(135, static_cast<uint_opt8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModeOFF, clean$WHENOIModeIsOffTHENReturnsError) {
	ASSERT_EQ(OI_NOT_STARTED, OI_tc.clean());
}

TEST_F(AllSystemsGoOIModeFULL, clean$WHENCalledTHENModeIsSetToPassive) {
	OI_tc.clean();
	ASSERT_EQ(PASSIVE, state::testing::getOIMode());
}

TEST_F(SerialTransactionFailureOIModePASSIVE, clean$WHENfnSerialWriteFailsTHENReturnsError) {
	ASSERT_EQ(SERIAL_TRANSFER_FAILURE, OI_tc.clean());
}

TEST_F(AllSystemsGoOIModeOFF, clean$WHENReturnsOINotStartedErrorTHENModeIsUnchanged) {
	ASSERT_EQ(OI_NOT_STARTED, OI_tc.clean());
	ASSERT_EQ(OFF, state::testing::getOIMode());
}

TEST_F(SerialTransactionFailureOIModeFULL, clean$WHENReturnsSerialTransferFailureErrorTHENModeIsUnchanged) {
	ASSERT_EQ(SERIAL_TRANSFER_FAILURE, OI_tc.clean());
	ASSERT_EQ(FULL, state::testing::getOIMode());
}

TEST_F(AllSystemsGoOIModeOFF, clean$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	EXPECT_EQ(OI_NOT_STARTED, OI_tc.clean());
	ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
}

TEST_F(AllSystemsGoOIModePASSIVE, max$WHENCalledTHEN136IsWrittenToTheSerialBus) {
	OI_tc.max();
	ASSERT_EQ(136, static_cast<uint_opt8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModeOFF, max$WHENOIModeIsOffTHENReturnsError) {
	ASSERT_EQ(OI_NOT_STARTED, OI_tc.max());
}

TEST_F(AllSystemsGoOIModeFULL, max$WHENCalledTHENModeIsSetToPassive) {
	OI_tc.max();
	ASSERT_EQ(PASSIVE, state::testing::getOIMode());
}

TEST_F(SerialTransactionFailureOIModePASSIVE, max$WHENfnSerialWriteFailsTHENReturnsError) {
	ASSERT_EQ(SERIAL_TRANSFER_FAILURE, OI_tc.max());
}

TEST_F(AllSystemsGoOIModeOFF, max$WHENReturnsOINotStartedErrorTHENModeIsUnchanged) {
	ASSERT_EQ(OI_NOT_STARTED, OI_tc.max());
	ASSERT_EQ(OFF, state::testing::getOIMode());
}

TEST_F(SerialTransactionFailureOIModeFULL, max$WHENReturnsSerialTransferFailureErrorTHENModeIsUnchanged) {
	ASSERT_EQ(SERIAL_TRANSFER_FAILURE, OI_tc.max());
	ASSERT_EQ(FULL, state::testing::getOIMode());
}

TEST_F(AllSystemsGoOIModeOFF, max$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	EXPECT_EQ(OI_NOT_STARTED, OI_tc.max());
	ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
}

TEST_F(AllSystemsGoOIModeFULL, drive$WHENCalledTHEN137AndParametersAreWrittenToTheSerialBus) {
	OI_tc.drive(-487, 1998);
	
	ASSERT_EQ(137, static_cast<uint_opt8_t>(serial_bus[0]));
	EXPECT_EQ(254, static_cast<uint_opt8_t>(serial_bus[1]));
	EXPECT_EQ(25, static_cast<uint_opt8_t>(serial_bus[2]));
	EXPECT_EQ(7, static_cast<uint_opt8_t>(serial_bus[3]));
	EXPECT_EQ(206, static_cast<uint_opt8_t>(serial_bus[4]));
}

TEST_F(AllSystemsGoOIModeFULL, drive$WHENVelocityIsGreaterThan500THENParameterIsInvalid) {
	for ( int i = 501 ; i <= 32767 ; ++i ) {
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.drive(i, 1998)) << "Accepted value <" << i << ">";
	}
}

TEST_F(AllSystemsGoOIModeFULL, drive$WHENVelocityIsLessThanNegative500THENParameterIsInvalid) {
	for ( int i = -501 ; i >= -32768 ; --i ) {
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.drive(i, 1998)) << "Accepted value <" << i << ">";
	}
}

TEST_F(AllSystemsGoOIModeFULL, drive$WHENRadiusIsGreaterThan2000THENParameterIsInvalid) {
	for ( int i = 2001 ; i < 32767 ; ++i ) {
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.drive(487, i)) << "Accepted value <" << i << ">";
	}
}

TEST_F(AllSystemsGoOIModeFULL, drive$WHENRadiusIsLessThanNegative2000THENParameterIsInvalid) {
	for ( int i = -2001 ; i >= -32768 ; --i ) {
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.drive(487, i)) << "Accepted value <" << i << ">";
	}
}

TEST_F(AllSystemsGoOIModeFULL, drive$WHENVelocityIsGreaterThan500THENNoDataIsWrittenToSerialBus) {
	for ( int i = 501 ; i <= 32767 ; ++i ) {
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.drive(i, 1998));
		ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
	}
}

TEST_F(AllSystemsGoOIModeFULL, drive$WHENVelocityIsLessThanNegative500THENNoDataIsWrittenToSerialBus) {
	for ( int i = -501 ; i >= -32768 ; --i ) {
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.drive(i, 1998));
		ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
	}
}

TEST_F(AllSystemsGoOIModeFULL, drive$WHENRadiusIsGreaterThan2000THENNoDataIsWrittenToSerialBus) {
	for ( int i = 2001 ; i < 32767 ; ++i ) {
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.drive(487, i));
		ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
	}
}

TEST_F(AllSystemsGoOIModeFULL, drive$WHENRadiusIsLessThanNegative2000THENNoDataIsWrittenToSerialBus) {
	for ( int i = -2001 ; i >= -32768 ; --i ) {
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.drive(487, i));
		ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
	}
}

TEST_F(AllSystemsGoOIModeOFF, drive$WHENOIModeIsOffTHENReturnsError) {
	ASSERT_EQ(OI_NOT_STARTED, OI_tc.drive(-487, 1998));
}

TEST_F(SerialTransactionFailureOIModeFULL, drive$WHENfnSerialWriteFailsTHENReturnsError) {
	ASSERT_EQ(SERIAL_TRANSFER_FAILURE, OI_tc.drive(-487, 1998));
}

TEST_F(AllSystemsGoOIModeOFF, drive$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	EXPECT_EQ(OI_NOT_STARTED, OI_tc.drive(-487, 1998));
	ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
}

TEST_F(AllSystemsGoOIModePASSIVE, drive$WHENOIModeIsPassiveTHENReturnsError) {
	ASSERT_EQ(INVALID_MODE_FOR_REQUESTED_OPERATION, OI_tc.drive(-487, 1998));
}

TEST_F(AllSystemsGoOIModePASSIVE, drive$WHENOIModeIsPassiveTHENNoDataIsWrittenToSerialBus) {
	EXPECT_EQ(INVALID_MODE_FOR_REQUESTED_OPERATION, OI_tc.drive(-487, 1998));
	ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
}

TEST_F(AllSystemsGoOIModeFULL, drive$WHENRadiusIsEqualToSpecialValue32767THENParameterIsAllowed) {
	OI_tc.drive(-487, 32767);
	
	ASSERT_EQ(137, static_cast<uint_opt8_t>(serial_bus[0]));
	EXPECT_EQ(254, static_cast<uint_opt8_t>(serial_bus[1]));
	EXPECT_EQ(25, static_cast<uint_opt8_t>(serial_bus[2]));
	EXPECT_EQ(127, static_cast<uint_opt8_t>(serial_bus[3]));
	EXPECT_EQ(255, static_cast<uint_opt8_t>(serial_bus[4]));
}

TEST_F(AllSystemsGoOIModeFULL, motors$WHENCalledTHEN138AndParametersAreWrittenToTheSerialBus) {
	bitmask::MotorStates motor_states = static_cast<bitmask::MotorStates>(bitmask::VACUUM_ENGAGED | bitmask::SIDE_BRUSH_ENGAGED | bitmask::SIDE_BRUSH_CLOCKWISE);
	OI_tc.motors(motor_states);
	
	ASSERT_EQ(138, static_cast<uint_opt8_t>(serial_bus[0]));
	EXPECT_EQ(11, static_cast<uint_opt8_t>(serial_bus[1]));
}

TEST_F(AllSystemsGoOIModeFULL, motors$WHENMotorStateMaskHasInvalidBitsSetTHENBitsAreDiscarded) {
	OI_tc.motors(static_cast<bitmask::MotorStates>(0xFF));
	ASSERT_EQ(0x1F, serial_bus[1]);
}

TEST_F(AllSystemsGoOIModeOFF, motors$WHENOIModeIsOffTHENReturnsError) {
	bitmask::MotorStates motor_states = static_cast<bitmask::MotorStates>(bitmask::VACUUM_ENGAGED | bitmask::SIDE_BRUSH_ENGAGED | bitmask::SIDE_BRUSH_CLOCKWISE);
	ASSERT_EQ(OI_NOT_STARTED, OI_tc.motors(motor_states));
}

TEST_F(SerialTransactionFailureOIModeFULL, motors$WHENfnSerialWriteFailsTHENReturnsError) {
	bitmask::MotorStates motor_states = static_cast<bitmask::MotorStates>(bitmask::VACUUM_ENGAGED | bitmask::SIDE_BRUSH_ENGAGED | bitmask::SIDE_BRUSH_CLOCKWISE);
	ASSERT_EQ(SERIAL_TRANSFER_FAILURE, OI_tc.motors(motor_states));
}

TEST_F(AllSystemsGoOIModeOFF, motors$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	bitmask::MotorStates motor_states = static_cast<bitmask::MotorStates>(bitmask::VACUUM_ENGAGED | bitmask::SIDE_BRUSH_ENGAGED | bitmask::SIDE_BRUSH_CLOCKWISE);
	EXPECT_EQ(OI_NOT_STARTED, OI_tc.motors(motor_states));
	ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
}

TEST_F(AllSystemsGoOIModePASSIVE, motors$WHENOIModeIsPassiveTHENReturnsError) {
	bitmask::MotorStates motor_states = static_cast<bitmask::MotorStates>(bitmask::VACUUM_ENGAGED | bitmask::SIDE_BRUSH_ENGAGED | bitmask::SIDE_BRUSH_CLOCKWISE);
	ASSERT_EQ(INVALID_MODE_FOR_REQUESTED_OPERATION, OI_tc.motors(motor_states));
}

TEST_F(AllSystemsGoOIModePASSIVE, motors$WHENOIModeIsPassiveTHENNoDataIsWrittenToSerialBus) {
	bitmask::MotorStates motor_states = static_cast<bitmask::MotorStates>(bitmask::VACUUM_ENGAGED | bitmask::SIDE_BRUSH_ENGAGED | bitmask::SIDE_BRUSH_CLOCKWISE);
	EXPECT_EQ(INVALID_MODE_FOR_REQUESTED_OPERATION, OI_tc.motors(motor_states));
	ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
}

TEST_F(AllSystemsGoOIModeFULL, leds$WHENCalledTHEN139AndParametersAreWrittenToTheSerialBus) {
	OI_tc.leds(static_cast<bitmask::display::LEDs>(bitmask::display::SPOT | bitmask::display::DEBRIS), 64, 192);
	
	ASSERT_EQ(139, static_cast<uint_opt8_t>(serial_bus[0]));
	EXPECT_EQ(3, static_cast<uint_opt8_t>(serial_bus[1]));
	EXPECT_EQ(64, static_cast<uint_opt8_t>(serial_bus[2]));
	EXPECT_EQ(192, static_cast<uint_opt8_t>(serial_bus[3]));
}

TEST_F(AllSystemsGoOIModeFULL, leds$WHENLedMaskHasInvalidBitsSetTHENBitsAreDiscarded) {
	OI_tc.leds(static_cast<bitmask::display::LEDs>(0xFF), 64, 192);
	ASSERT_EQ(0x0F, serial_bus[1]);
}

TEST_F(AllSystemsGoOIModeOFF, leds$WHENOIModeIsOffTHENReturnsError) {
	ASSERT_EQ(OI_NOT_STARTED, OI_tc.leds(static_cast<bitmask::display::LEDs>(bitmask::display::SPOT | bitmask::display::DEBRIS), 64, 192));
}

TEST_F(SerialTransactionFailureOIModeFULL, leds$WHENfnSerialWriteFailsTHENReturnsError) {
	ASSERT_EQ(SERIAL_TRANSFER_FAILURE, OI_tc.leds(static_cast<bitmask::display::LEDs>(bitmask::display::SPOT | bitmask::display::DEBRIS), 64, 192));
}

TEST_F(AllSystemsGoOIModeOFF, leds$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	EXPECT_EQ(OI_NOT_STARTED, OI_tc.leds(static_cast<bitmask::display::LEDs>(bitmask::display::SPOT | bitmask::display::DEBRIS), 64, 192));
	ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
}

TEST_F(AllSystemsGoOIModePASSIVE, leds$WHENOIModeIsPassiveTHENReturnsError) {
	ASSERT_EQ(INVALID_MODE_FOR_REQUESTED_OPERATION, OI_tc.leds(static_cast<bitmask::display::LEDs>(bitmask::display::SPOT | bitmask::display::DEBRIS), 64, 192));
}

TEST_F(AllSystemsGoOIModePASSIVE, leds$WHENOIModeIsPassiveTHENNoDataIsWrittenToSerialBus) {
	EXPECT_EQ(INVALID_MODE_FOR_REQUESTED_OPERATION, OI_tc.leds(static_cast<bitmask::display::LEDs>(bitmask::display::SPOT | bitmask::display::DEBRIS), 64, 192));
	ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
}

TEST_F(AllSystemsGoOIModePASSIVE, song$WHENCalledTHEN140AndParametersAreWrittenToTheSerialBus) {
	std::vector<note_t> fur_elise = { {E_4, 11}, {D_SHARP_4, 11}, {E_4, 11}, {D_SHARP_4, 11}, {E_4, 11}, {B_3, 11}, {D_4, 11}, {C_4, 11}, {A_3, 32} };
	
	OI_tc.song(1, fur_elise.data(), fur_elise.size());
	
	ASSERT_EQ(140, static_cast<uint_opt8_t>(serial_bus[0]));
	EXPECT_EQ(1, static_cast<uint_opt8_t>(serial_bus[1]));
	EXPECT_EQ(9, static_cast<uint_opt8_t>(serial_bus[2]));
	EXPECT_EQ(64, static_cast<uint_opt8_t>(serial_bus[3]));
	EXPECT_EQ(11, static_cast<uint_opt8_t>(serial_bus[4]));
	EXPECT_EQ(63, static_cast<uint_opt8_t>(serial_bus[5]));
	EXPECT_EQ(11, static_cast<uint_opt8_t>(serial_bus[6]));
	EXPECT_EQ(64, static_cast<uint_opt8_t>(serial_bus[7]));
	EXPECT_EQ(11, static_cast<uint_opt8_t>(serial_bus[8]));
	EXPECT_EQ(63, static_cast<uint_opt8_t>(serial_bus[9]));
	EXPECT_EQ(11, static_cast<uint_opt8_t>(serial_bus[10]));
	EXPECT_EQ(64, static_cast<uint_opt8_t>(serial_bus[11]));
	EXPECT_EQ(11, static_cast<uint_opt8_t>(serial_bus[12]));
	EXPECT_EQ(59, static_cast<uint_opt8_t>(serial_bus[13]));
	EXPECT_EQ(11, static_cast<uint_opt8_t>(serial_bus[14]));
	EXPECT_EQ(62, static_cast<uint_opt8_t>(serial_bus[15]));
	EXPECT_EQ(11, static_cast<uint_opt8_t>(serial_bus[16]));
	EXPECT_EQ(60, static_cast<uint_opt8_t>(serial_bus[17]));
	EXPECT_EQ(11, static_cast<uint_opt8_t>(serial_bus[18]));
	EXPECT_EQ(57, static_cast<uint_opt8_t>(serial_bus[19]));
	EXPECT_EQ(32, static_cast<uint_opt8_t>(serial_bus[20]));
}

TEST_F(AllSystemsGoOIModePASSIVE, song$WHENSongNumberIsGreaterThan4THENParameterIsInvalid) {
	std::vector<note_t> fur_elise = { {E_4, 11}, {D_SHARP_4, 11}, {E_4, 11}, {D_SHARP_4, 11}, {E_4, 11}, {B_3, 11}, {D_4, 11}, {C_4, 11}, {A_3, 32} };
	
	for ( int i = 5 ; i <= 255 ; ++i ) {
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.song(i, fur_elise.data(), fur_elise.size())) << "Accepted value <" << i << ">";
	}
}

TEST_F(AllSystemsGoOIModePASSIVE, song$WHENSongNumberIsGreaterThan4THENNoDataIsWrittenToSerialBus) {
	std::vector<note_t> fur_elise = { {E_4, 11}, {D_SHARP_4, 11}, {E_4, 11}, {D_SHARP_4, 11}, {E_4, 11}, {B_3, 11}, {D_4, 11}, {C_4, 11}, {A_3, 32} };
	
	for ( int i = 5 ; i <= 255 ; ++i ) {
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.song(i, fur_elise.data(), fur_elise.size()));
		ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
	}
}

TEST_F(AllSystemsGoOIModePASSIVE, song$WHENSongIsNullTHENParameterIsInvalid) {
	note_t * no_song = nullptr;
	
	ASSERT_EQ(INVALID_PARAMETER, OI_tc.song(1, no_song, 1));
}

TEST_F(AllSystemsGoOIModePASSIVE, song$WHENSongIsNullTHENNoDataIsWrittenToSerialBus) {
	note_t * no_song = nullptr;
	
	EXPECT_EQ(INVALID_PARAMETER, OI_tc.song(1, no_song, 1));
	ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
}

TEST_F(AllSystemsGoOIModePASSIVE, song$WHENSongIsZeroNotesTHENParameterIsInvalid) {
	std::vector<note_t> no_song;
	
	ASSERT_EQ(INVALID_PARAMETER, OI_tc.song(1, no_song.data(), no_song.size()));
}

TEST_F(AllSystemsGoOIModePASSIVE, song$WHENSongIsZeroNotesTHENNoDataIsWrittenToSerialBus) {
	note_t * no_song = nullptr;
	
	EXPECT_EQ(INVALID_PARAMETER, OI_tc.song(1, no_song, 0));
	ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
}

TEST_F(AllSystemsGoOIModePASSIVE, song$WHENSongIsMoreThan16NotesTHENParameterIsInvalid) {
	std::vector<note_t> fur_elise_ep = { {E_4, 11}, {D_SHARP_4, 11}, {E_4, 11}, {D_SHARP_4, 11}, {E_4, 11}, {B_3, 11}, {D_4, 11}, {C_4, 11}, {A_3, 32}, {A_3, 21}, {A_3, 11}, {B_3, 32}, {B_3, 21}, {B_3, 11}, {C_4, 43}, {E_4, 11}, {D_SHARP_4, 11} };
	
	ASSERT_EQ(INVALID_PARAMETER, OI_tc.song(1, fur_elise_ep.data(), fur_elise_ep.size()));
}

TEST_F(AllSystemsGoOIModePASSIVE, song$WHENSongIsMoreThan16NotesTHENNoDataIsWrittenToSerialBus) {
	std::vector<note_t> fur_elise_ep = { {E_4, 11}, {D_SHARP_4, 11}, {E_4, 11}, {D_SHARP_4, 11}, {E_4, 11}, {B_3, 11}, {D_4, 11}, {C_4, 11}, {A_3, 32}, {A_3, 21}, {A_3, 11}, {B_3, 32}, {B_3, 21}, {B_3, 11}, {C_4, 43}, {E_4, 11}, {D_SHARP_4, 11} };
	
	EXPECT_EQ(INVALID_PARAMETER, OI_tc.song(1, fur_elise_ep.data(), fur_elise_ep.size()));
	ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
}

TEST_F(AllSystemsGoOIModeOFF, song$WHENOIModeIsOffTHENReturnsError) {
	std::vector<note_t> fur_elise = { {E_4, 11}, {D_SHARP_4, 11}, {E_4, 11}, {D_SHARP_4, 11}, {E_4, 11}, {B_3, 11}, {D_4, 11}, {C_4, 11}, {A_3, 32} };
	
	ASSERT_EQ(OI_NOT_STARTED, OI_tc.song(1, fur_elise.data(), fur_elise.size()));
}

TEST_F(SerialTransactionFailureOIModePASSIVE, song$WHENfnSerialWriteFailsTHENReturnsError) {
	std::vector<note_t> fur_elise = { {E_4, 11}, {D_SHARP_4, 11}, {E_4, 11}, {D_SHARP_4, 11}, {E_4, 11}, {B_3, 11}, {D_4, 11}, {C_4, 11}, {A_3, 32} };
	
	ASSERT_EQ(SERIAL_TRANSFER_FAILURE, OI_tc.song(1, fur_elise.data(), fur_elise.size()));
}

TEST_F(AllSystemsGoOIModeOFF, song$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	std::vector<note_t> fur_elise = { {E_4, 11}, {D_SHARP_4, 11}, {E_4, 11}, {D_SHARP_4, 11}, {E_4, 11}, {B_3, 11}, {D_4, 11}, {C_4, 11}, {A_3, 32} };
	
	EXPECT_EQ(OI_NOT_STARTED, OI_tc.song(1, fur_elise.data(), fur_elise.size()));
	ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
}

TEST_F(AllSystemsGoOIModeFULL, play$WHENCalledTHEN141AndParametersAreWrittenToTheSerialBus) {
	OI_tc.play(1);
	
	ASSERT_EQ(141, static_cast<uint_opt8_t>(serial_bus[0]));
	EXPECT_EQ(1, static_cast<uint_opt8_t>(serial_bus[1]));
}

TEST_F(AllSystemsGoOIModeFULL, play$WHENSongNumberIsGreaterThan4THENParameterIsInvalid) {
	for ( int i = 5 ; i <= 255 ; ++i ) {
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.play(i)) << "Accepted value <" << i << ">";
	}
}

TEST_F(AllSystemsGoOIModeFULL, play$WHENSongNumberIsGreaterThan4THENNoDataIsWrittenToSerialBus) {
	for ( int i = 5 ; i <= 255 ; ++i ) {
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.play(i));
		ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
	}
}

TEST_F(AllSystemsGoOIModeOFF, play$WHENOIModeIsOffTHENReturnsError) {
	ASSERT_EQ(OI_NOT_STARTED, OI_tc.play(1));
}

TEST_F(SerialTransactionFailureOIModeFULL, play$WHENfnSerialWriteFailsTHENReturnsError) {
	ASSERT_EQ(SERIAL_TRANSFER_FAILURE, OI_tc.play(1));
}

TEST_F(AllSystemsGoOIModeOFF, play$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	EXPECT_EQ(OI_NOT_STARTED, OI_tc.play(1));
	ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
}

TEST_F(AllSystemsGoOIModePASSIVE, play$WHENOIModeIsPassiveTHENReturnsError) {
	ASSERT_EQ(INVALID_MODE_FOR_REQUESTED_OPERATION, OI_tc.play(1));
}

TEST_F(AllSystemsGoOIModePASSIVE, play$WHENOIModeIsPassiveTHENNoDataIsWrittenToSerialBus) {
	EXPECT_EQ(INVALID_MODE_FOR_REQUESTED_OPERATION, OI_tc.play(1));
	ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
}

TEST_F(AllSystemsGoOIModeFULL, sensors$WHENCalledTHEN142AndParametersAreWrittenToTheSerialBus) {
	OI_tc.sensors(sensor::DIRT_DETECT);
	
	ASSERT_EQ(142, static_cast<uint_opt8_t>(serial_bus[0]));
	EXPECT_EQ(15, static_cast<uint_opt8_t>(serial_bus[1]));
}

TEST_F(AllSystemsGoOIModeFULL, sensors$WHENSensorNumberIsBetween0And58InclusiveTHENParameterIsValid) {
	for ( int i = 0 ; i <= 58 ; ++i ) {
		EXPECT_EQ(SUCCESS, OI_tc.sensors(static_cast<sensor::PacketId>(i))) << "Rejected value <" << i << ">";
	}
}

TEST_F(AllSystemsGoOIModeFULL, sensors$WHENSensorNumberIsBetween59And99InclusiveTHENParameterIsInvalid) {
	for ( int i = 59 ; i <= 99 ; ++i ) {
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.sensors(static_cast<sensor::PacketId>(i))) << "Accepted value <" << i << ">";
	}
}

TEST_F(AllSystemsGoOIModeFULL, sensors$WHENSensorNumberIsBetween100And107InclusiveTHENParameterIsValid) {
	for ( int i = 100 ; i <= 107 ; ++i ) {
		EXPECT_EQ(SUCCESS, OI_tc.sensors(static_cast<sensor::PacketId>(i))) << "Rejected value <" << i << ">";
	}
}

TEST_F(AllSystemsGoOIModeFULL, sensors$WHENSensorNumberIsGreaterThan107THENParameterIsInvalid) {
	for ( int i = 108 ; i <= 255 ; ++i ) {
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.sensors(static_cast<sensor::PacketId>(i))) << "Accepted value <" << i << ">";
	}
}

TEST_F(AllSystemsGoOIModeFULL, sensors$WHENSensorNumberIsBetween59And99InclusiveTHENNoDataIsWrittenToSerialBus) {
	for ( int i = 59 ; i <= 99 ; ++i ) {
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.sensors(static_cast<sensor::PacketId>(i)));
		ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
	}
}

TEST_F(AllSystemsGoOIModeFULL, sensors$WHENSensorNumberIsGreaterThan107InclusiveTHENNoDataIsWrittenToSerialBus) {
	for ( int i = 108 ; i <= 255 ; ++i ) {
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.sensors(static_cast<sensor::PacketId>(i)));
		ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
	}
}

TEST_F(AllSystemsGoOIModeOFF, sensors$WHENOIModeIsOffTHENReturnsError) {
	ASSERT_EQ(OI_NOT_STARTED, OI_tc.sensors(sensor::DIRT_DETECT));
}

TEST_F(SerialTransactionFailureOIModeFULL, sensors$WHENfnSerialWriteFailsTHENReturnsError) {
	ASSERT_EQ(SERIAL_TRANSFER_FAILURE, OI_tc.sensors(sensor::DIRT_DETECT));
}

TEST_F(AllSystemsGoOIModeOFF, sensors$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	EXPECT_EQ(OI_NOT_STARTED, OI_tc.sensors(sensor::DIRT_DETECT));
	ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
}

TEST_F(AllSystemsGoOIModePASSIVE, seekDock$WHENCalledTHEN143IsWrittenToTheSerialBus) {
	OI_tc.seekDock();
	ASSERT_EQ(143, static_cast<uint_opt8_t>(serial_bus[0]));
}

TEST_F(AllSystemsGoOIModeOFF, seekDock$WHENOIModeIsOffTHENReturnsError) {
	ASSERT_EQ(OI_NOT_STARTED, OI_tc.seekDock());
}

TEST_F(AllSystemsGoOIModeFULL, seekDock$WHENCalledTHENModeIsSetToPassive) {
	OI_tc.seekDock();
	ASSERT_EQ(PASSIVE, state::testing::getOIMode());
}

TEST_F(SerialTransactionFailureOIModePASSIVE, seekDock$WHENfnSerialWriteFailsTHENReturnsError) {
	ASSERT_EQ(SERIAL_TRANSFER_FAILURE, OI_tc.seekDock());
}

TEST_F(AllSystemsGoOIModeOFF, seekDock$WHENReturnsOINotStartedErrorTHENModeIsUnchanged) {
	ASSERT_EQ(OI_NOT_STARTED, OI_tc.seekDock());
	ASSERT_EQ(OFF, state::testing::getOIMode());
}

TEST_F(SerialTransactionFailureOIModeFULL, seekDock$WHENReturnsSerialTransferFailureErrorTHENModeIsUnchanged) {
	ASSERT_EQ(SERIAL_TRANSFER_FAILURE, OI_tc.seekDock());
	ASSERT_EQ(FULL, state::testing::getOIMode());
}

TEST_F(AllSystemsGoOIModeOFF, seekDock$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	EXPECT_EQ(OI_NOT_STARTED, OI_tc.seekDock());
	ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
}

TEST_F(AllSystemsGoOIModeFULL, pwmMotors$WHENCalledTHEN144AndParametersAreWrittenToTheSerialBus) {
	OI_tc.pwmMotors(96, -64, 127);
	
	ASSERT_EQ(144, static_cast<uint_opt8_t>(serial_bus[0]));
	EXPECT_EQ(96, static_cast<uint_opt8_t>(serial_bus[1]));
	EXPECT_EQ(192, static_cast<uint_opt8_t>(serial_bus[2]));
	EXPECT_EQ(127, static_cast<uint_opt8_t>(serial_bus[3]));
}

TEST_F(AllSystemsGoOIModeFULL, pwmMotors$WHENMainBrushIsNegative128THENParameterIsInvalid) {
	ASSERT_EQ(INVALID_PARAMETER, OI_tc.pwmMotors(-128, -127, 0));
}

TEST_F(AllSystemsGoOIModeFULL, pwmMotors$WHENSideBrushIsNegative128THENParameterIsInvalid) {
	ASSERT_EQ(INVALID_PARAMETER, OI_tc.pwmMotors(-127, -128, 0));
}

TEST_F(AllSystemsGoOIModeFULL, pwmMotors$WHENVacuumIsLessThanZeroTHENParameterIsInvalid) {
	for ( int i = -1 ; i >= -128 ; --i ) {
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.pwmMotors(-127, -127, i)) << "Accepted value <" << i << ">";
	}
}

TEST_F(AllSystemsGoOIModeFULL, pwmMotors$WHENParametersAreInvalidTHENNoDataIsWrittenToSerialBus) {
	EXPECT_EQ(INVALID_PARAMETER, OI_tc.pwmMotors(-127, -128, 0));
	ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";

	EXPECT_EQ(INVALID_PARAMETER, OI_tc.pwmMotors(-128, -127, 0));
	ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
	
	for ( int i = -1 ; i >= -128 ; --i ) {
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.pwmMotors(-127, -127, i));
		ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
	}
}

TEST_F(AllSystemsGoOIModeOFF, pwmMotors$WHENOIModeIsOffTHENReturnsError) {
	ASSERT_EQ(OI_NOT_STARTED, OI_tc.pwmMotors(96, -64, 127));
}

TEST_F(SerialTransactionFailureOIModeFULL, pwmMotors$WHENfnSerialWriteFailsTHENReturnsError) {
	ASSERT_EQ(SERIAL_TRANSFER_FAILURE, OI_tc.pwmMotors(96, -64, 127));
}

TEST_F(AllSystemsGoOIModeOFF, pwmMotors$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	EXPECT_EQ(OI_NOT_STARTED, OI_tc.pwmMotors(96, -64, 127));
	ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
}

TEST_F(AllSystemsGoOIModePASSIVE, pwmMotors$WHENOIModeIsPassiveTHENReturnsError) {
	ASSERT_EQ(INVALID_MODE_FOR_REQUESTED_OPERATION, OI_tc.pwmMotors(96, -64, 127));
}

TEST_F(AllSystemsGoOIModePASSIVE, pwmMotors$WHENOIModeIsPassiveTHENNoDataIsWrittenToSerialBus) {
	EXPECT_EQ(INVALID_MODE_FOR_REQUESTED_OPERATION, OI_tc.pwmMotors(96, -64, 127));
	ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
}

TEST_F(AllSystemsGoOIModeFULL, driveDirect$WHENCalledTHEN145AndParametersAreWrittenToTheSerialBus) {
	OI_tc.driveDirect(-150, 150);
	
	ASSERT_EQ(145, static_cast<uint_opt8_t>(serial_bus[0]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[1]));
	EXPECT_EQ(150, static_cast<uint_opt8_t>(serial_bus[2]));
	EXPECT_EQ(255, static_cast<uint_opt8_t>(serial_bus[3]));
	EXPECT_EQ(106, static_cast<uint_opt8_t>(serial_bus[4]));
}

TEST_F(AllSystemsGoOIModeFULL, driveDirect$WHENLeftWheelVelocityIsGreaterThan500THENParameterIsInvalid) {
	for ( int i = 501 ; i <= 32767 ; ++i ) {
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.driveDirect(i, 500)) << "Accepted value <" << i << ">";
	}
}

TEST_F(AllSystemsGoOIModeFULL, driveDirect$WHENLeftWheelVelocityIsLessThanNegative500THENParameterIsInvalid) {
	for ( int i = -501 ; i >= -32768 ; --i ) {
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.driveDirect(i, -500)) << "Accepted value <" << i << ">";
	}
}

TEST_F(AllSystemsGoOIModeFULL, driveDirect$WHENRightWheelVelocityIsGreaterThan500THENParameterIsInvalid) {
	for ( int i = 501 ; i <= 32767 ; ++i ) {
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.driveDirect(500, i)) << "Accepted value <" << i << ">";
	}
}

TEST_F(AllSystemsGoOIModeFULL, driveDirect$WHENRightWheelVelocityIsLessThanNegative500THENParameterIsInvalid) {
	for ( int i = -501 ; i >= -32768 ; --i ) {
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.driveDirect(-500, i)) << "Accepted value <" << i << ">";
	}
}

TEST_F(AllSystemsGoOIModeFULL, driveDirect$WHENLeftWheelVelocityIsGreaterThan500THENNoDataIsWrittenToSerialBus) {
	for ( int i = 501 ; i <= 32767 ; ++i ) {
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.driveDirect(i, 500));
		ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
	}
}

TEST_F(AllSystemsGoOIModeFULL, driveDirect$WHENLeftWheelVelocityIsLessThanNegative500THENNoDataIsWrittenToSerialBus) {
	for ( int i = -501 ; i >= -32768 ; --i ) {
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.driveDirect(i, -500));
		ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
	}
}

TEST_F(AllSystemsGoOIModeFULL, driveDirect$WHENRightWheelVelocityIsGreaterThan500THENNoDataIsWrittenToSerialBus) {
	for ( int i = 501 ; i <= 32767 ; ++i ) {
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.driveDirect(500, i));
		ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
	}
}

TEST_F(AllSystemsGoOIModeFULL, driveDirect$WHENRightWheelVelocityIsLessThanNegative500THENNoDataIsWrittenToSerialBus) {
	for ( int i = -501 ; i >= -32768 ; --i ) {
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.driveDirect(-500, i));
		ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
	}
}

TEST_F(AllSystemsGoOIModeOFF, driveDirect$WHENOIModeIsOffTHENReturnsError) {
	ASSERT_EQ(OI_NOT_STARTED, OI_tc.driveDirect(-150, 150));
}

TEST_F(SerialTransactionFailureOIModeFULL, driveDirect$WHENfnSerialWriteFailsTHENReturnsError) {
	ASSERT_EQ(SERIAL_TRANSFER_FAILURE, OI_tc.driveDirect(-150, 150));
}

TEST_F(AllSystemsGoOIModeOFF, driveDirect$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	EXPECT_EQ(OI_NOT_STARTED, OI_tc.driveDirect(-150, 150));
	ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
}

TEST_F(AllSystemsGoOIModePASSIVE, driveDirect$WHENOIModeIsPassiveTHENReturnsError) {
	ASSERT_EQ(INVALID_MODE_FOR_REQUESTED_OPERATION, OI_tc.driveDirect(-150, 150));
}

TEST_F(AllSystemsGoOIModePASSIVE, driveDirect$WHENOIModeIsPassiveTHENNoDataIsWrittenToSerialBus) {
	EXPECT_EQ(INVALID_MODE_FOR_REQUESTED_OPERATION, OI_tc.driveDirect(-150, 150));
	ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
}

TEST_F(AllSystemsGoOIModeFULL, drivePWM$WHENCalledTHEN146AndParametersAreWrittenToTheSerialBus) {
	OI_tc.drivePWM(-32, 32);
	
	ASSERT_EQ(146, static_cast<uint_opt8_t>(serial_bus[0]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[1]));
	EXPECT_EQ(32, static_cast<uint_opt8_t>(serial_bus[2]));
	EXPECT_EQ(255, static_cast<uint_opt8_t>(serial_bus[3]));
	EXPECT_EQ(224, static_cast<uint_opt8_t>(serial_bus[4]));
}

TEST_F(AllSystemsGoOIModeFULL, drivePWM$WHENLeftWheelVelocityIsGreaterThan255THENParameterIsInvalid) {
	for ( int i = 256 ; i <= 32767 ; ++i ) {
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.drivePWM(i, 255)) << "Accepted value <" << i << ">";
	}
}

TEST_F(AllSystemsGoOIModeFULL, drivePWM$WHENLeftWheelVelocityIsLessThanNegative255THENParameterIsInvalid) {
	for ( int i = -256 ; i >= -32768 ; --i ) {
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.drivePWM(i, -255)) << "Accepted value <" << i << ">";
	}
}

TEST_F(AllSystemsGoOIModeFULL, drivePWM$WHENRightWheelVelocityIsGreaterThan255THENParameterIsInvalid) {
	for ( int i = 256 ; i <= 32767 ; ++i ) {
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.drivePWM(255, i)) << "Accepted value <" << i << ">";
	}
}

TEST_F(AllSystemsGoOIModeFULL, drivePWM$WHENRightWheelVelocityIsLessThanNegative255THENParameterIsInvalid) {
	for ( int i = -256 ; i >= -32768 ; --i ) {
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.drivePWM(-255, i)) << "Accepted value <" << i << ">";
	}
}

TEST_F(AllSystemsGoOIModeFULL, drivePWM$WHENLeftWheelVelocityIsGreaterThan255THENNoDataIsWrittenToSerialBus) {
	for ( int i = 256 ; i <= 32767 ; ++i ) {
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.drivePWM(i, 255));
		ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
	}
}

TEST_F(AllSystemsGoOIModeFULL, drivePWM$WHENLeftWheelVelocityIsLessThanNegative255THENNoDataIsWrittenToSerialBus) {
	for ( int i = -256 ; i >= -32767 ; --i ) {
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.drivePWM(i, -255));
		ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
	}
}

TEST_F(AllSystemsGoOIModeFULL, drivePWM$WHENRightWheelVelocityIsGreaterThan255THENNoDataIsWrittenToSerialBus) {
	for ( int i = 256 ; i <= 32767 ; ++i ) {
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.drivePWM(255, i));
		ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
	}
}

TEST_F(AllSystemsGoOIModeFULL, drivePWM$WHENRightWheelVelocityIsLessThanNegative255THENNoDataIsWrittenToSerialBus) {
	for ( int i = -256 ; i >= -32767 ; --i ) {
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.drivePWM(-255, i));
		ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
	}
}

TEST_F(AllSystemsGoOIModeOFF, drivePWM$WHENOIModeIsOffTHENReturnsError) {
	ASSERT_EQ(OI_NOT_STARTED, OI_tc.drivePWM(-32, 32));
}

TEST_F(SerialTransactionFailureOIModeFULL, drivePWM$WHENfnSerialWriteFailsTHENReturnsError) {
	ASSERT_EQ(SERIAL_TRANSFER_FAILURE, OI_tc.drivePWM(-32, 32));
}

TEST_F(AllSystemsGoOIModeOFF, drivePWM$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	EXPECT_EQ(OI_NOT_STARTED, OI_tc.drivePWM(-32, 32));
	ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
}

TEST_F(AllSystemsGoOIModePASSIVE, drivePWM$WHENOIModeIsPassiveTHENReturnsError) {
	ASSERT_EQ(INVALID_MODE_FOR_REQUESTED_OPERATION, OI_tc.drivePWM(-32, 32));
}

TEST_F(AllSystemsGoOIModePASSIVE, drivePWM$WHENOIModeIsPassiveTHENNoDataIsWrittenToSerialBus) {
	EXPECT_EQ(INVALID_MODE_FOR_REQUESTED_OPERATION, OI_tc.drivePWM(-32, 32));
	ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
}

TEST_F(AllSystemsGoOIModeFULL, stream$WHENCalledTHEN148AndParametersAreWrittenToTheSerialBus) {
	std::vector<sensor::PacketId> sensor_list = { sensor::CLIFF_FRONT_LEFT_SIGNAL, sensor::VIRTUAL_WALL };
	OI_tc.stream(sensor_list.data(), sensor_list.size());
	
	ASSERT_EQ(148, static_cast<uint_opt8_t>(serial_bus[0]));
	EXPECT_EQ(2, static_cast<uint_opt8_t>(serial_bus[1]));
	EXPECT_EQ(29, static_cast<uint_opt8_t>(serial_bus[2]));
	EXPECT_EQ(13, static_cast<uint_opt8_t>(serial_bus[3]));
}

TEST_F(AllSystemsGoOIModeFULL, stream$WHENSensorListIsNullTHENParameterIsInvalid) {
	sensor::PacketId * sensor_list = nullptr;
	ASSERT_EQ(INVALID_PARAMETER, OI_tc.stream(sensor_list, 0));
}

TEST_F(AllSystemsGoOIModeFULL, stream$WHENSensorListIsNullTHENNoDataIsWrittenToSerialBus) {
	sensor::PacketId * sensor_list = nullptr;
	EXPECT_EQ(INVALID_PARAMETER, OI_tc.stream(sensor_list, 0));
	ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
}

TEST_F(AllSystemsGoOIModeFULL, stream$WHENSensorListIsEmptyTHENParameterIsInvalid) {
	std::vector<sensor::PacketId> sensor_list;
	ASSERT_EQ(INVALID_PARAMETER, OI_tc.stream(sensor_list.data(), sensor_list.size()));
}

TEST_F(AllSystemsGoOIModeFULL, stream$WHENSensorListIsEmptyTHENNoDataIsWrittenToSerialBus) {
	std::vector<sensor::PacketId> sensor_list;
	EXPECT_EQ(INVALID_PARAMETER, OI_tc.stream(sensor_list.data(), sensor_list.size()));
	ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
}

TEST_F(AllSystemsGoOIModeFULL, stream$WHENSensorNumberIsBetween0And58InclusiveTHENValueIsAccepted) {
	std::vector<sensor::PacketId> sensor_list = { sensor::CLIFF_FRONT_LEFT_SIGNAL };
	for ( int i = 0 ; i <= 58 ; ++i ) {
		sensor_list.push_back(static_cast<sensor::PacketId>(i));
		sensor_list.push_back(sensor::VIRTUAL_WALL);
		OI_tc.stream(sensor_list.data(), sensor_list.size());
		EXPECT_EQ(29, static_cast<uint_opt8_t>(serial_bus[2]));
		EXPECT_EQ(i, static_cast<uint_opt8_t>(serial_bus[3])) << "Rejected value <" << i << ">";
		EXPECT_EQ(13, static_cast<uint_opt8_t>(serial_bus[4]));
		sensor_list.pop_back();
		sensor_list.pop_back();
	}
}

TEST_F(AllSystemsGoOIModeFULL, stream$WHENSensorNumberIsBetween59And99InclusiveTHENValueIsIgnored) {
	std::vector<sensor::PacketId> sensor_list = { sensor::CLIFF_FRONT_LEFT_SIGNAL };
	for ( int i = 59 ; i <= 99 ; ++i ) {
		sensor_list.push_back(static_cast<sensor::PacketId>(i));
		sensor_list.push_back(sensor::VIRTUAL_WALL);
		OI_tc.stream(sensor_list.data(), sensor_list.size());
		EXPECT_EQ(29, static_cast<uint_opt8_t>(serial_bus[2]));
		EXPECT_EQ(13, static_cast<uint_opt8_t>(serial_bus[3]));
		sensor_list.pop_back();
		sensor_list.pop_back();
	}
}

TEST_F(AllSystemsGoOIModeFULL, stream$WHENSensorNumberIsBetween100And107InclusiveTHENValueIsAccepted) {
	std::vector<sensor::PacketId> sensor_list = { sensor::CLIFF_FRONT_LEFT_SIGNAL };
	for ( int i = 100 ; i <= 107 ; ++i ) {
		sensor_list.push_back(static_cast<sensor::PacketId>(i));
		sensor_list.push_back(sensor::VIRTUAL_WALL);
		OI_tc.stream(sensor_list.data(), sensor_list.size());
		EXPECT_EQ(29, static_cast<uint_opt8_t>(serial_bus[2]));
		EXPECT_EQ(i, static_cast<uint_opt8_t>(serial_bus[3])) << "Rejected value <" << i << ">";
		EXPECT_EQ(13, static_cast<uint_opt8_t>(serial_bus[4]));
		sensor_list.pop_back();
		sensor_list.pop_back();
	}
}

TEST_F(AllSystemsGoOIModeFULL, stream$WHENSensorNumberIsBetween108And255InclusiveTHENValueIsIgnored) {
	std::vector<sensor::PacketId> sensor_list = { sensor::CLIFF_FRONT_LEFT_SIGNAL };
	for ( int i = 108 ; i <= 255 ; ++i ) {
		sensor_list.push_back(static_cast<sensor::PacketId>(i));
		sensor_list.push_back(sensor::VIRTUAL_WALL);
		OI_tc.stream(sensor_list.data(), sensor_list.size());
		EXPECT_EQ(29, static_cast<uint_opt8_t>(serial_bus[2]));
		EXPECT_EQ(13, static_cast<uint_opt8_t>(serial_bus[3]));
		sensor_list.pop_back();
		sensor_list.pop_back();
	}
}

TEST_F(AllSystemsGoOIModeFULL, stream$WHENAllSensorsAreIgnoredTHENParameterIsInvalid) {
	std::vector<sensor::PacketId> sensor_list = { static_cast<sensor::PacketId>(69), static_cast<sensor::PacketId>(70) };
	ASSERT_EQ(INVALID_PARAMETER, OI_tc.stream(sensor_list.data(), sensor_list.size()));
}

TEST_F(AllSystemsGoOIModeFULL, stream$WHENAllSensorsAreIgnoredTHENNoDataIsWrittenToSerialBus) {
	std::vector<sensor::PacketId> sensor_list = { static_cast<sensor::PacketId>(69), static_cast<sensor::PacketId>(70) };
	EXPECT_EQ(INVALID_PARAMETER, OI_tc.stream(sensor_list.data(), sensor_list.size()));
	ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
}

TEST_F(AllSystemsGoOIModeOFF, stream$WHENOIModeIsOffTHENReturnsError) {
	std::vector<sensor::PacketId> sensor_list = { sensor::CLIFF_FRONT_LEFT_SIGNAL, sensor::VIRTUAL_WALL };
	ASSERT_EQ(OI_NOT_STARTED, OI_tc.stream(sensor_list.data(), sensor_list.size()));
}

TEST_F(SerialTransactionFailureOIModeFULL, stream$WHENfnSerialWriteFailsTHENReturnsError) {
	std::vector<sensor::PacketId> sensor_list = { sensor::CLIFF_FRONT_LEFT_SIGNAL, sensor::VIRTUAL_WALL };
	ASSERT_EQ(SERIAL_TRANSFER_FAILURE, OI_tc.stream(sensor_list.data(), sensor_list.size()));
}

TEST_F(AllSystemsGoOIModeOFF, stream$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	std::vector<sensor::PacketId> sensor_list = { sensor::CLIFF_FRONT_LEFT_SIGNAL, sensor::VIRTUAL_WALL };
	EXPECT_EQ(OI_NOT_STARTED, OI_tc.stream(sensor_list.data(), sensor_list.size()));
	ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
}

TEST_F(AllSystemsGoOIModeFULL, queryList$WHENCalledTHEN149AndParametersAreWrittenToTheSerialBus) {
	std::vector<sensor::PacketId> sensor_list = { sensor::CLIFF_FRONT_LEFT_SIGNAL, sensor::VIRTUAL_WALL };
	OI_tc.queryList(sensor_list.data(), sensor_list.size());
	
	ASSERT_EQ(149, static_cast<uint_opt8_t>(serial_bus[0]));
	EXPECT_EQ(2, static_cast<uint_opt8_t>(serial_bus[1]));
	EXPECT_EQ(29, static_cast<uint_opt8_t>(serial_bus[2]));
	EXPECT_EQ(13, static_cast<uint_opt8_t>(serial_bus[3]));
}

TEST_F(AllSystemsGoOIModeFULL, queryList$WHENSensorListIsNullTHENParameterIsInvalid) {
	sensor::PacketId * sensor_list = nullptr;
	ASSERT_EQ(INVALID_PARAMETER, OI_tc.queryList(sensor_list, 0));
}

TEST_F(AllSystemsGoOIModeFULL, queryList$WHENSensorListIsNullTHENNoDataIsWrittenToSerialBus) {
	sensor::PacketId * sensor_list = nullptr;
	EXPECT_EQ(INVALID_PARAMETER, OI_tc.queryList(sensor_list, 0));
	ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
}

TEST_F(AllSystemsGoOIModeFULL, queryList$WHENSensorListIsEmptyTHENParameterIsInvalid) {
	std::vector<sensor::PacketId> sensor_list;
	ASSERT_EQ(INVALID_PARAMETER, OI_tc.queryList(sensor_list.data(), sensor_list.size()));
}

TEST_F(AllSystemsGoOIModeFULL, queryList$WHENSensorListIsEmptyTHENNoDataIsWrittenToSerialBus) {
	std::vector<sensor::PacketId> sensor_list;
	EXPECT_EQ(INVALID_PARAMETER, OI_tc.queryList(sensor_list.data(), sensor_list.size()));
	ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
}

TEST_F(AllSystemsGoOIModeFULL, queryList$WHENSensorNumberIsBetween0And58InclusiveTHENValueIsAccepted) {
	std::vector<sensor::PacketId> sensor_list = { sensor::CLIFF_FRONT_LEFT_SIGNAL };
	for ( int i = 0 ; i <= 58 ; ++i ) {
		sensor_list.push_back(static_cast<sensor::PacketId>(i));
		sensor_list.push_back(sensor::VIRTUAL_WALL);
		OI_tc.queryList(sensor_list.data(), sensor_list.size());
		EXPECT_EQ(29, static_cast<uint_opt8_t>(serial_bus[2]));
		EXPECT_EQ(i, static_cast<uint_opt8_t>(serial_bus[3])) << "Rejected value <" << i << ">";
		EXPECT_EQ(13, static_cast<uint_opt8_t>(serial_bus[4]));
		sensor_list.pop_back();
		sensor_list.pop_back();
	}
}

TEST_F(AllSystemsGoOIModeFULL, queryList$WHENSensorNumberIsBetween59And99InclusiveTHENValueIsIgnored) {
	std::vector<sensor::PacketId> sensor_list = { sensor::CLIFF_FRONT_LEFT_SIGNAL };
	for ( int i = 59 ; i <= 99 ; ++i ) {
		sensor_list.push_back(static_cast<sensor::PacketId>(i));
		sensor_list.push_back(sensor::VIRTUAL_WALL);
		OI_tc.queryList(sensor_list.data(), sensor_list.size());
		EXPECT_EQ(29, static_cast<uint_opt8_t>(serial_bus[2]));
		EXPECT_EQ(13, static_cast<uint_opt8_t>(serial_bus[3]));
		sensor_list.pop_back();
		sensor_list.pop_back();
	}
}

TEST_F(AllSystemsGoOIModeFULL, queryList$WHENSensorNumberIsBetween100And107InclusiveTHENValueIsAccepted) {
	std::vector<sensor::PacketId> sensor_list = { sensor::CLIFF_FRONT_LEFT_SIGNAL };
	for ( int i = 100 ; i <= 107 ; ++i ) {
		sensor_list.push_back(static_cast<sensor::PacketId>(i));
		sensor_list.push_back(sensor::VIRTUAL_WALL);
		OI_tc.queryList(sensor_list.data(), sensor_list.size());
		EXPECT_EQ(29, static_cast<uint_opt8_t>(serial_bus[2]));
		EXPECT_EQ(i, static_cast<uint_opt8_t>(serial_bus[3])) << "Rejected value <" << i << ">";
		EXPECT_EQ(13, static_cast<uint_opt8_t>(serial_bus[4]));
		sensor_list.pop_back();
		sensor_list.pop_back();
	}
}

TEST_F(AllSystemsGoOIModeFULL, queryList$WHENSensorNumberIsBetween108And255InclusiveTHENValueIsIgnored) {
	std::vector<sensor::PacketId> sensor_list = { sensor::CLIFF_FRONT_LEFT_SIGNAL };
	for ( int i = 108 ; i <= 255 ; ++i ) {
		sensor_list.push_back(static_cast<sensor::PacketId>(i));
		sensor_list.push_back(sensor::VIRTUAL_WALL);
		OI_tc.queryList(sensor_list.data(), sensor_list.size());
		EXPECT_EQ(29, static_cast<uint_opt8_t>(serial_bus[2]));
		EXPECT_EQ(13, static_cast<uint_opt8_t>(serial_bus[3]));
		sensor_list.pop_back();
		sensor_list.pop_back();
	}
}

TEST_F(AllSystemsGoOIModeFULL, queryList$WHENAllSensorsAreIgnoredTHENParameterIsInvalid) {
	std::vector<sensor::PacketId> sensor_list = { static_cast<sensor::PacketId>(69), static_cast<sensor::PacketId>(70) };
	ASSERT_EQ(INVALID_PARAMETER, OI_tc.queryList(sensor_list.data(), sensor_list.size()));
}

TEST_F(AllSystemsGoOIModeFULL, queryList$WHENAllSensorsAreIgnoredTHENNoDataIsWrittenToSerialBus) {
	std::vector<sensor::PacketId> sensor_list = { static_cast<sensor::PacketId>(69), static_cast<sensor::PacketId>(70) };
	EXPECT_EQ(INVALID_PARAMETER, OI_tc.queryList(sensor_list.data(), sensor_list.size()));
	ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
}

TEST_F(AllSystemsGoOIModeOFF, queryList$WHENOIModeIsOffTHENReturnsError) {
	std::vector<sensor::PacketId> sensor_list = { sensor::CLIFF_FRONT_LEFT_SIGNAL, sensor::VIRTUAL_WALL };
	ASSERT_EQ(OI_NOT_STARTED, OI_tc.queryList(sensor_list.data(), sensor_list.size()));
}

TEST_F(SerialTransactionFailureOIModeFULL, queryList$WHENfnSerialWriteFailsTHENReturnsError) {
	std::vector<sensor::PacketId> sensor_list = { sensor::CLIFF_FRONT_LEFT_SIGNAL, sensor::VIRTUAL_WALL };
	ASSERT_EQ(SERIAL_TRANSFER_FAILURE, OI_tc.queryList(sensor_list.data(), sensor_list.size()));
}

TEST_F(AllSystemsGoOIModeOFF, queryList$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	std::vector<sensor::PacketId> sensor_list = { sensor::CLIFF_FRONT_LEFT_SIGNAL, sensor::VIRTUAL_WALL };
	EXPECT_EQ(OI_NOT_STARTED, OI_tc.queryList(sensor_list.data(), sensor_list.size()));
	ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
}

TEST_F(AllSystemsGoOIModeFULL, pauseResumeStream$WHENCalledTHEN141AndParametersAreWrittenToTheSerialBus) {
	OI_tc.pauseResumeStream(false);
	
	ASSERT_EQ(150, static_cast<uint_opt8_t>(serial_bus[0]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[1]));
}

TEST_F(AllSystemsGoOIModeFULL, pauseResumeStream$WHENParameterIsNonZeroTHEN1WrittenToTheSerialBus) {
	OI_tc.pauseResumeStream(46);
	
	ASSERT_EQ(150, static_cast<uint_opt8_t>(serial_bus[0]));
	EXPECT_EQ(1, static_cast<uint_opt8_t>(serial_bus[1]));
}

TEST_F(AllSystemsGoOIModeOFF, pauseResumeStream$WHENOIModeIsOffTHENReturnsError) {
	ASSERT_EQ(OI_NOT_STARTED, OI_tc.pauseResumeStream(true));
}

TEST_F(SerialTransactionFailureOIModeFULL, pauseResumeStream$WHENfnSerialWriteFailsTHENReturnsError) {
	ASSERT_EQ(SERIAL_TRANSFER_FAILURE, OI_tc.pauseResumeStream(true));
}

TEST_F(AllSystemsGoOIModeOFF, pauseResumeStream$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	EXPECT_EQ(OI_NOT_STARTED, OI_tc.pauseResumeStream(true));
	ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
}

TEST_F(AllSystemsGoOIModeFULL, schedulingLEDs$WHENCalledTHEN162AndParametersAreWrittenToTheSerialBus) {
	OI_tc.schedulingLEDs(static_cast<bitmask::Days>(bitmask::TUESDAY | bitmask::SATURDAY), static_cast<bitmask::display::SchedulingLEDs>(bitmask::display::CLOCK | bitmask::display::COLON | bitmask::display::PM));
	
	ASSERT_EQ(162, static_cast<uint_opt8_t>(serial_bus[0]));
	EXPECT_EQ(68, static_cast<uint_opt8_t>(serial_bus[1]));
	EXPECT_EQ(11, static_cast<uint_opt8_t>(serial_bus[2]));
}

TEST_F(AllSystemsGoOIModeFULL, schedulingLEDs$WHENDayMaskHasInvalidBitsSetTHENBitsAreDiscarded) {
	OI_tc.schedulingLEDs(static_cast<bitmask::Days>(0xFF), static_cast<bitmask::display::SchedulingLEDs>(bitmask::display::CLOCK | bitmask::display::COLON | bitmask::display::PM));
	ASSERT_EQ(0x7F, serial_bus[1]);
}

TEST_F(AllSystemsGoOIModeFULL, schedulingLEDs$WHENDisplayMaskHasInvalidBitsSetTHENBitsAreDiscarded) {
	OI_tc.schedulingLEDs(static_cast<bitmask::Days>(bitmask::TUESDAY | bitmask::SATURDAY), static_cast<bitmask::display::SchedulingLEDs>(0xFF));
	ASSERT_EQ(0x1F, serial_bus[2]);
}

TEST_F(AllSystemsGoOIModeOFF, schedulingLEDs$WHENOIModeIsOffTHENReturnsError) {
	ASSERT_EQ(OI_NOT_STARTED, OI_tc.schedulingLEDs(static_cast<bitmask::Days>(bitmask::TUESDAY | bitmask::SATURDAY), static_cast<bitmask::display::SchedulingLEDs>(bitmask::display::CLOCK | bitmask::display::COLON | bitmask::display::PM)));
}

TEST_F(SerialTransactionFailureOIModeFULL, schedulingLEDs$WHENfnSerialWriteFailsTHENReturnsError) {
	ASSERT_EQ(SERIAL_TRANSFER_FAILURE, OI_tc.schedulingLEDs(static_cast<bitmask::Days>(bitmask::TUESDAY | bitmask::SATURDAY), static_cast<bitmask::display::SchedulingLEDs>(bitmask::display::CLOCK | bitmask::display::COLON | bitmask::display::PM)));
}

TEST_F(AllSystemsGoOIModeOFF, schedulingLEDs$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	EXPECT_EQ(OI_NOT_STARTED, OI_tc.schedulingLEDs(static_cast<bitmask::Days>(bitmask::TUESDAY | bitmask::SATURDAY), static_cast<bitmask::display::SchedulingLEDs>(bitmask::display::CLOCK | bitmask::display::COLON | bitmask::display::PM)));
	ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
}

TEST_F(AllSystemsGoOIModePASSIVE, schedulingLEDs$WHENOIModeIsPassiveTHENReturnsError) {
	ASSERT_EQ(INVALID_MODE_FOR_REQUESTED_OPERATION, OI_tc.schedulingLEDs(static_cast<bitmask::Days>(bitmask::TUESDAY | bitmask::SATURDAY), static_cast<bitmask::display::SchedulingLEDs>(bitmask::display::CLOCK | bitmask::display::COLON | bitmask::display::PM)));
}

TEST_F(AllSystemsGoOIModePASSIVE, schedulingLEDs$WHENOIModeIsPassiveTHENNoDataIsWrittenToSerialBus) {
	EXPECT_EQ(INVALID_MODE_FOR_REQUESTED_OPERATION, OI_tc.schedulingLEDs(static_cast<bitmask::Days>(bitmask::TUESDAY | bitmask::SATURDAY), static_cast<bitmask::display::SchedulingLEDs>(bitmask::display::CLOCK | bitmask::display::COLON | bitmask::display::PM)));
	ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
}

TEST_F(AllSystemsGoOIModeFULL, digitLEDsRaw$WHENCalledTHEN163AndParametersAreWrittenToTheSerialBus) {
	bitmask::display::DigitN seven_segments[4] = {
		static_cast<bitmask::display::DigitN>(bitmask::display::A | bitmask::display::B),
		static_cast<bitmask::display::DigitN>(bitmask::display::C | bitmask::display::D),
		static_cast<bitmask::display::DigitN>(bitmask::display::E | bitmask::display::F),
		static_cast<bitmask::display::DigitN>(bitmask::display::G | bitmask::display::A)
	};
	
	OI_tc.digitLEDsRaw(seven_segments);
	
	ASSERT_EQ(163, static_cast<uint_opt8_t>(serial_bus[0]));
	EXPECT_EQ(3, static_cast<uint_opt8_t>(serial_bus[1]));
	EXPECT_EQ(12, static_cast<uint_opt8_t>(serial_bus[2]));
	EXPECT_EQ(48, static_cast<uint_opt8_t>(serial_bus[3]));
	EXPECT_EQ(65, static_cast<uint_opt8_t>(serial_bus[4]));
}

TEST_F(AllSystemsGoOIModeFULL, digitLEDsRaw$WHENDigit3MaskHasInvalidBitsSetTHENBitsAreDiscarded) {
	bitmask::display::DigitN seven_segments[4] = {
		static_cast<bitmask::display::DigitN>(0xFF),
		static_cast<bitmask::display::DigitN>(bitmask::display::C | bitmask::display::D),
		static_cast<bitmask::display::DigitN>(bitmask::display::E | bitmask::display::F),
		static_cast<bitmask::display::DigitN>(bitmask::display::G | bitmask::display::A)
	};
	
	OI_tc.digitLEDsRaw(seven_segments);
	ASSERT_EQ(0x7F, serial_bus[1]);
}

TEST_F(AllSystemsGoOIModeFULL, digitLEDsRaw$WHENDigit2MaskHasInvalidBitsSetTHENBitsAreDiscarded) {
	bitmask::display::DigitN seven_segments[4] = {
		static_cast<bitmask::display::DigitN>(bitmask::display::A | bitmask::display::B),
		static_cast<bitmask::display::DigitN>(0xFF),
		static_cast<bitmask::display::DigitN>(bitmask::display::E | bitmask::display::F),
		static_cast<bitmask::display::DigitN>(bitmask::display::G | bitmask::display::A)
	};
	
	OI_tc.digitLEDsRaw(seven_segments);
	ASSERT_EQ(0x7F, serial_bus[2]);
}

TEST_F(AllSystemsGoOIModeFULL, digitLEDsRaw$WHENDigit1MaskHasInvalidBitsSetTHENBitsAreDiscarded) {
	bitmask::display::DigitN seven_segments[4] = {
		static_cast<bitmask::display::DigitN>(bitmask::display::A | bitmask::display::B),
		static_cast<bitmask::display::DigitN>(bitmask::display::C | bitmask::display::D),
		static_cast<bitmask::display::DigitN>(0xFF),
		static_cast<bitmask::display::DigitN>(bitmask::display::G | bitmask::display::A)
	};
	
	OI_tc.digitLEDsRaw(seven_segments);
	ASSERT_EQ(0x7F, serial_bus[3]);
}

TEST_F(AllSystemsGoOIModeFULL, digitLEDsRaw$WHENDigit0MaskHasInvalidBitsSetTHENBitsAreDiscarded) {
	bitmask::display::DigitN seven_segments[4] = {
		static_cast<bitmask::display::DigitN>(bitmask::display::A | bitmask::display::B),
		static_cast<bitmask::display::DigitN>(bitmask::display::C | bitmask::display::D),
		static_cast<bitmask::display::DigitN>(bitmask::display::E | bitmask::display::F),
		static_cast<bitmask::display::DigitN>(0xFF)
	};
	
	OI_tc.digitLEDsRaw(seven_segments);
	ASSERT_EQ(0x7F, serial_bus[4]);
}

TEST_F(AllSystemsGoOIModeOFF, digitLEDsRaw$WHENOIModeIsOffTHENReturnsError) {
	bitmask::display::DigitN seven_segments[4] = {
		static_cast<bitmask::display::DigitN>(bitmask::display::A | bitmask::display::B),
		static_cast<bitmask::display::DigitN>(bitmask::display::C | bitmask::display::D),
		static_cast<bitmask::display::DigitN>(bitmask::display::E | bitmask::display::F),
		static_cast<bitmask::display::DigitN>(bitmask::display::G | bitmask::display::A)
	};
	
	ASSERT_EQ(OI_NOT_STARTED, OI_tc.digitLEDsRaw(seven_segments));
}

TEST_F(SerialTransactionFailureOIModeFULL, digitLEDsRaw$WHENfnSerialWriteFailsTHENReturnsError) {
	bitmask::display::DigitN seven_segments[4] = {
		static_cast<bitmask::display::DigitN>(bitmask::display::A | bitmask::display::B),
		static_cast<bitmask::display::DigitN>(bitmask::display::C | bitmask::display::D),
		static_cast<bitmask::display::DigitN>(bitmask::display::E | bitmask::display::F),
		static_cast<bitmask::display::DigitN>(bitmask::display::G | bitmask::display::A)
	};
	
	ASSERT_EQ(SERIAL_TRANSFER_FAILURE, OI_tc.digitLEDsRaw(seven_segments));
}

TEST_F(AllSystemsGoOIModeOFF, digitLEDsRaw$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	bitmask::display::DigitN seven_segments[4] = {
		static_cast<bitmask::display::DigitN>(bitmask::display::A | bitmask::display::B),
		static_cast<bitmask::display::DigitN>(bitmask::display::C | bitmask::display::D),
		static_cast<bitmask::display::DigitN>(bitmask::display::E | bitmask::display::F),
		static_cast<bitmask::display::DigitN>(bitmask::display::G | bitmask::display::A)
	};
	
	EXPECT_EQ(OI_NOT_STARTED, OI_tc.digitLEDsRaw(seven_segments));
	ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
}

TEST_F(AllSystemsGoOIModePASSIVE, digitLEDsRaw$WHENOIModeIsPassiveTHENReturnsError) {
	bitmask::display::DigitN seven_segments[4] = {
		static_cast<bitmask::display::DigitN>(bitmask::display::A | bitmask::display::B),
		static_cast<bitmask::display::DigitN>(bitmask::display::C | bitmask::display::D),
		static_cast<bitmask::display::DigitN>(bitmask::display::E | bitmask::display::F),
		static_cast<bitmask::display::DigitN>(bitmask::display::G | bitmask::display::A)
	};
	
	ASSERT_EQ(INVALID_MODE_FOR_REQUESTED_OPERATION, OI_tc.digitLEDsRaw(seven_segments));
}

TEST_F(AllSystemsGoOIModePASSIVE, digitLEDsRaw$WHENOIModeIsPassiveTHENNoDataIsWrittenToSerialBus) {
	bitmask::display::DigitN seven_segments[4] = {
		static_cast<bitmask::display::DigitN>(bitmask::display::A | bitmask::display::B),
		static_cast<bitmask::display::DigitN>(bitmask::display::C | bitmask::display::D),
		static_cast<bitmask::display::DigitN>(bitmask::display::E | bitmask::display::F),
		static_cast<bitmask::display::DigitN>(bitmask::display::G | bitmask::display::A)
	};
	
	EXPECT_EQ(INVALID_MODE_FOR_REQUESTED_OPERATION, OI_tc.digitLEDsRaw(seven_segments));
	ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
}

TEST_F(AllSystemsGoOIModeFULL, digitLEDsASCII$WHENCalledTHEN164AndParametersAreWrittenToTheSerialBus) {
	char seven_segments[4] = { 'Z', 'A', 'K', '!' };
	
	OI_tc.digitLEDsASCII(seven_segments);
	
	ASSERT_EQ(164, static_cast<uint_opt8_t>(serial_bus[0]));
	EXPECT_EQ(90, static_cast<uint_opt8_t>(serial_bus[1]));
	EXPECT_EQ(65, static_cast<uint_opt8_t>(serial_bus[2]));
	EXPECT_EQ(75, static_cast<uint_opt8_t>(serial_bus[3]));
	EXPECT_EQ(33, static_cast<uint_opt8_t>(serial_bus[4]));
}

TEST_F(AllSystemsGoOIModeFULL, digitLEDsASCII$WHENDigit3IsOutsideTheRange32To126InclusiveTHENParameterIsInvalid) {
	char seven_segments[4] = { 0, 'A', 'K', '!' };
	
	for ( int i = 0 ; i <= 31 ; ++i) {
		seven_segments[0] = static_cast<char>(i);
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.digitLEDsASCII(seven_segments)) << "Accepted value <" << i << ">";
	}
	for ( int i = 127 ; i <= 255 ; ++i) {
		seven_segments[0] = static_cast<char>(i);
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.digitLEDsASCII(seven_segments)) << "Accepted value <" << i << ">";
	}
}

TEST_F(AllSystemsGoOIModeFULL, digitLEDsASCII$WHENDigit2IsOutsideTheRange32To126InclusiveTHENParameterIsInvalid) {
	char seven_segments[4] = { 'Z', 0, 'K', '!' };
	
	for ( int i = 0 ; i <= 31 ; ++i) {
		seven_segments[1] = static_cast<char>(i);
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.digitLEDsASCII(seven_segments)) << "Accepted value <" << i << ">";
	}
	for ( int i = 127 ; i <= 255 ; ++i) {
		seven_segments[1] = static_cast<char>(i);
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.digitLEDsASCII(seven_segments)) << "Accepted value <" << i << ">";
	}
}

TEST_F(AllSystemsGoOIModeFULL, digitLEDsASCII$WHENDigit1IsOutsideTheRange32To126InclusiveTHENParameterIsInvalid) {
	char seven_segments[4] = { 'Z', 'A', 0, '!' };
	
	for ( int i = 0 ; i <= 31 ; ++i) {
		seven_segments[2] = static_cast<char>(i);
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.digitLEDsASCII(seven_segments)) << "Accepted value <" << i << ">";
	}
	for ( int i = 127 ; i <= 255 ; ++i) {
		seven_segments[2] = static_cast<char>(i);
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.digitLEDsASCII(seven_segments)) << "Accepted value <" << i << ">";
	}
}

TEST_F(AllSystemsGoOIModeFULL, digitLEDsASCII$WHENDigit0IsOutsideTheRange32To126InclusiveTHENParameterIsInvalid) {
	char seven_segments[4] = { 'Z', 'A', 'K', 0 };
	
	for ( int i = 0 ; i <= 31 ; ++i) {
		seven_segments[3] = static_cast<char>(i);
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.digitLEDsASCII(seven_segments)) << "Accepted value <" << i << ">";
	}
	for ( int i = 127 ; i <= 255 ; ++i) {
		seven_segments[3] = static_cast<char>(i);
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.digitLEDsASCII(seven_segments)) << "Accepted value <" << i << ">";
	}
}

TEST_F(AllSystemsGoOIModeFULL, digitLEDsASCII$WHENDigit3IsOutsideTheRange32To126InclusiveTHENNoDataIsWrittenToSerialBus) {
	char seven_segments[4] = { 0, 'A', 'K', '!' };
	
	for ( int i = 0 ; i <= 31 ; ++i) {
		seven_segments[0] = static_cast<char>(i);
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.digitLEDsASCII(seven_segments));
		ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
	}
	for ( int i = 127 ; i <= 255 ; ++i) {
		seven_segments[0] = static_cast<char>(i);
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.digitLEDsASCII(seven_segments));
		ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
	}
}

TEST_F(AllSystemsGoOIModeFULL, digitLEDsASCII$WHENDigit2IsOutsideTheRange32To126InclusiveTHENNoDataIsWrittenToSerialBus) {
	char seven_segments[4] = { 'Z', 0, 'K', '!' };
	
	for ( int i = 0 ; i <= 31 ; ++i) {
		seven_segments[1] = static_cast<char>(i);
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.digitLEDsASCII(seven_segments));
		ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[1])) << "Bus: [" << serial_bus << "]";
	}
	for ( int i = 127 ; i <= 255 ; ++i) {
		seven_segments[1] = static_cast<char>(i);
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.digitLEDsASCII(seven_segments));
		ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[1])) << "Bus: [" << serial_bus << "]";
	}
}

TEST_F(AllSystemsGoOIModeFULL, digitLEDsASCII$WHENDigit1IsOutsideTheRange32To126InclusiveTHENNoDataIsWrittenToSerialBus) {
	char seven_segments[4] = { 'Z', 'A', 0, '!' };
	
	for ( int i = 0 ; i <= 31 ; ++i) {
		seven_segments[2] = static_cast<char>(i);
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.digitLEDsASCII(seven_segments));
		ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[2])) << "Bus: [" << serial_bus << "]";
	}
	for ( int i = 127 ; i <= 255 ; ++i) {
		seven_segments[2] = static_cast<char>(i);
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.digitLEDsASCII(seven_segments));
		ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[2])) << "Bus: [" << serial_bus << "]";
	}
}

TEST_F(AllSystemsGoOIModeFULL, digitLEDsASCII$WHENDigit0IsOutsideTheRange32To126InclusiveTHENNoDataIsWrittenToSerialBus) {
	char seven_segments[4] = { 'Z', 'A', 'K', 0 };
	
	for ( int i = 0 ; i <= 31 ; ++i) {
		seven_segments[3] = static_cast<char>(i);
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.digitLEDsASCII(seven_segments));
		ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[3])) << "Bus: [" << serial_bus << "]";
	}
	for ( int i = 127 ; i <= 255 ; ++i) {
		seven_segments[3] = static_cast<char>(i);
		EXPECT_EQ(INVALID_PARAMETER, OI_tc.digitLEDsASCII(seven_segments));
		ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[3])) << "Bus: [" << serial_bus << "]";
	}
}

TEST_F(AllSystemsGoOIModeOFF, digitLEDsASCII$WHENOIModeIsOffTHENReturnsError) {
	char seven_segments[4] = { 'Z', 'A', 'K', '!' };
	
	ASSERT_EQ(OI_NOT_STARTED, OI_tc.digitLEDsASCII(seven_segments));
}

TEST_F(SerialTransactionFailureOIModeFULL, digitLEDsASCII$WHENfnSerialWriteFailsTHENReturnsError) {
	char seven_segments[4] = { 'Z', 'A', 'K', '!' };
	
	ASSERT_EQ(SERIAL_TRANSFER_FAILURE, OI_tc.digitLEDsASCII(seven_segments));
}

TEST_F(AllSystemsGoOIModeOFF, digitLEDsASCII$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	char seven_segments[4] = { 'Z', 'A', 'K', '!' };
	
	EXPECT_EQ(OI_NOT_STARTED, OI_tc.digitLEDsASCII(seven_segments));
	ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
}

TEST_F(AllSystemsGoOIModePASSIVE, digitLEDsASCII$WHENOIModeIsPassiveTHENReturnsError) {
	char seven_segments[4] = { 'Z', 'A', 'K', '!' };
	
	ASSERT_EQ(INVALID_MODE_FOR_REQUESTED_OPERATION, OI_tc.digitLEDsASCII(seven_segments));
}

TEST_F(AllSystemsGoOIModePASSIVE, digitLEDsASCII$WHENOIModeIsPassiveTHENNoDataIsWrittenToSerialBus) {
	char seven_segments[4] = { 'Z', 'A', 'K', '!' };
	
	EXPECT_EQ(INVALID_MODE_FOR_REQUESTED_OPERATION, OI_tc.digitLEDsASCII(seven_segments));
	ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
}

TEST_F(AllSystemsGoOIModePASSIVE, buttons$WHENCalledTHEN165AndParametersAreWrittenToTheSerialBus) {
	OI_tc.buttons(static_cast<bitmask::Buttons>(bitmask::SPOT | bitmask::CLEAN));
	
	ASSERT_EQ(165, static_cast<uint_opt8_t>(serial_bus[0]));
	EXPECT_EQ(3, static_cast<uint_opt8_t>(serial_bus[1]));
}

TEST_F(AllSystemsGoOIModeOFF, buttons$WHENOIModeIsOffTHENReturnsError) {
	ASSERT_EQ(OI_NOT_STARTED, OI_tc.buttons(static_cast<bitmask::Buttons>(bitmask::SPOT | bitmask::CLEAN)));
}

TEST_F(SerialTransactionFailureOIModeFULL, buttons$WHENfnSerialWriteFailsTHENReturnsError) {
	ASSERT_EQ(SERIAL_TRANSFER_FAILURE, OI_tc.buttons(static_cast<bitmask::Buttons>(bitmask::SPOT | bitmask::CLEAN)));
}

TEST_F(AllSystemsGoOIModeOFF, buttons$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	EXPECT_EQ(OI_NOT_STARTED, OI_tc.buttons(static_cast<bitmask::Buttons>(bitmask::SPOT | bitmask::CLEAN)));
	ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
}

TEST_F(AllSystemsGoOIModePASSIVE, schedule$WHENCalledTHEN167AndParametersAreWrittenToTheSerialBus) {
	bitmask::Days days = static_cast<bitmask::Days>(bitmask::SUNDAY | bitmask::MONDAY | bitmask::TUESDAY | bitmask::WEDNESDAY | bitmask::THURSDAY | bitmask::FRIDAY | bitmask::SATURDAY);
	clock_time_t clk_time[7] = { clock_time_t(15, 35), clock_time_t(10, 15), clock_time_t(16, 40), clock_time_t(8, 45), clock_time_t(9, 30), clock_time_t(14, 10), clock_time_t(11, 55) };
	
	OI_tc.schedule(days, clk_time);
	
	ASSERT_EQ(167, static_cast<uint_opt8_t>(serial_bus[0]));
	EXPECT_EQ(127, static_cast<uint_opt8_t>(serial_bus[1]));
	EXPECT_EQ(15, static_cast<uint_opt8_t>(serial_bus[2]));
	EXPECT_EQ(35, static_cast<uint_opt8_t>(serial_bus[3]));
	EXPECT_EQ(10, static_cast<uint_opt8_t>(serial_bus[4]));
	EXPECT_EQ(15, static_cast<uint_opt8_t>(serial_bus[5]));
	EXPECT_EQ(16, static_cast<uint_opt8_t>(serial_bus[6]));
	EXPECT_EQ(40, static_cast<uint_opt8_t>(serial_bus[7]));
	EXPECT_EQ(8, static_cast<uint_opt8_t>(serial_bus[8]));
	EXPECT_EQ(45, static_cast<uint_opt8_t>(serial_bus[9]));
	EXPECT_EQ(9, static_cast<uint_opt8_t>(serial_bus[10]));
	EXPECT_EQ(30, static_cast<uint_opt8_t>(serial_bus[11]));
	EXPECT_EQ(14, static_cast<uint_opt8_t>(serial_bus[12]));
	EXPECT_EQ(10, static_cast<uint_opt8_t>(serial_bus[13]));
	EXPECT_EQ(11, static_cast<uint_opt8_t>(serial_bus[14]));
	EXPECT_EQ(55, static_cast<uint_opt8_t>(serial_bus[15]));
}

TEST_F(AllSystemsGoOIModePASSIVE, schedule$WHENCalledWithDisableInBitMaskTHEN167AndAllZerosAreWrittenToTheSerialBus) {
	clock_time_t clk_time[7] = { clock_time_t(15, 35), clock_time_t(10, 15), clock_time_t(16, 40), clock_time_t(8, 45), clock_time_t(9, 30), clock_time_t(14, 10), clock_time_t(11, 55) };

	OI_tc.schedule(bitmask::DISABLE, clk_time);
	
	ASSERT_EQ(167, static_cast<uint_opt8_t>(serial_bus[0]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[1]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[2]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[3]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[4]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[5]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[6]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[7]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[8]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[9]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[10]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[11]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[12]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[13]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[14]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[15]));
}

TEST_F(AllSystemsGoOIModePASSIVE, schedule$WHENCalledWithSparseCParametersTHENFullZeroFilledSerialParametersAreWrittenToTheSerialBus) {
	bitmask::Days days = static_cast<bitmask::Days>(bitmask::WEDNESDAY | bitmask::SUNDAY);
	clock_time_t clk_time[2] = { clock_time_t(15, 35), clock_time_t(10) };
	
	OI_tc.schedule(days, clk_time);
	
	ASSERT_EQ(167, static_cast<uint_opt8_t>(serial_bus[0]));
	EXPECT_EQ(9, static_cast<uint_opt8_t>(serial_bus[1]));
	EXPECT_EQ(15, static_cast<uint_opt8_t>(serial_bus[2]));
	EXPECT_EQ(35, static_cast<uint_opt8_t>(serial_bus[3]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[4]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[5]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[6]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[7]));
	EXPECT_EQ(10, static_cast<uint_opt8_t>(serial_bus[8]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[9]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[10]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[11]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[12]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[13]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[14]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[15]));
}

TEST_F(AllSystemsGoOIModePASSIVE, schedule$WHENCalledWithNULLArrayTHEN167AndAllZerosAreWrittenToTheSerialBus) {
	bitmask::Days days = static_cast<bitmask::Days>(bitmask::WEDNESDAY | bitmask::SUNDAY);
	
	OI_tc.schedule(days, NULL);
	
	ASSERT_EQ(167, static_cast<uint_opt8_t>(serial_bus[0]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[1]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[2]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[3]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[4]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[5]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[6]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[7]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[8]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[9]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[10]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[11]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[12]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[13]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[14]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[15]));
}

TEST_F(AllSystemsGoOIModePASSIVE, schedule$WHENCalledWithInvalidTimeParametersTHENCorrespondingDayIsIgnored) {
	bitmask::Days days = static_cast<bitmask::Days>(bitmask::WEDNESDAY | bitmask::SUNDAY);
	clock_time_t clk_time[2] = { clock_time_t(15, 35) };
	
	for ( int i = 24 ; i <= 255 ; ++i ) {
		for ( int j = 60 ; j <= 255 ; ++j ) {
			clk_time[1] = clock_time_t(i, j);
			
			OI_tc.schedule(days, clk_time);
			
			ASSERT_EQ(167, static_cast<uint_opt8_t>(serial_bus[0]));
			EXPECT_EQ(1, static_cast<uint_opt8_t>(serial_bus[1]));
			EXPECT_EQ(15, static_cast<uint_opt8_t>(serial_bus[2]));
			EXPECT_EQ(35, static_cast<uint_opt8_t>(serial_bus[3]));
			EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[4]));
			EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[5]));
			EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[6]));
			EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[7]));
			EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[8]));
			EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[9]));
			EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[10]));
			EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[11]));
			EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[12]));
			EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[13]));
			EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[14]));
			EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[15]));
		}
	}
}

TEST_F(AllSystemsGoOIModePASSIVE, schedule$WHENDaysMaskHasInvalidBitsSetTHENBitsAreDiscarded) {
	clock_time_t clk_time[7] = { clock_time_t(15, 35), clock_time_t(10) };
	
	OI_tc.schedule(static_cast<bitmask::Days>(0xFF), clk_time);

	ASSERT_EQ(167, static_cast<uint_opt8_t>(serial_bus[0]));
	ASSERT_EQ(0x7F, serial_bus[1]);
	EXPECT_EQ(15, static_cast<uint_opt8_t>(serial_bus[2]));
	EXPECT_EQ(35, static_cast<uint_opt8_t>(serial_bus[3]));
	EXPECT_EQ(10, static_cast<uint_opt8_t>(serial_bus[4]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[5]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[6]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[7]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[8]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[9]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[10]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[11]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[12]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[13]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[14]));
	EXPECT_EQ(0, static_cast<uint_opt8_t>(serial_bus[15]));
}

TEST_F(AllSystemsGoOIModeOFF, schedule$WHENOIModeIsOffTHENReturnsError) {
	bitmask::Days days = static_cast<bitmask::Days>(bitmask::WEDNESDAY | bitmask::SUNDAY);
	clock_time_t clk_time[2] = { clock_time_t(15, 35), clock_time_t(10) };
	
	ASSERT_EQ(OI_NOT_STARTED, OI_tc.schedule(days, clk_time));
}

TEST_F(SerialTransactionFailureOIModePASSIVE, schedule$WHENfnSerialWriteFailsTHENReturnsError) {
	bitmask::Days days = static_cast<bitmask::Days>(bitmask::WEDNESDAY | bitmask::SUNDAY);
	clock_time_t clk_time[2] = { clock_time_t(15, 35), clock_time_t(10) };
	
	ASSERT_EQ(SERIAL_TRANSFER_FAILURE, OI_tc.schedule(days, clk_time));
}

TEST_F(AllSystemsGoOIModeOFF, schedule$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	bitmask::Days days = static_cast<bitmask::Days>(bitmask::WEDNESDAY | bitmask::SUNDAY);
	clock_time_t clk_time[2] = { clock_time_t(15, 35), clock_time_t(10) };
	
	EXPECT_EQ(OI_NOT_STARTED, OI_tc.schedule(days, clk_time));
	ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
}

TEST_F(AllSystemsGoOIModePASSIVE, setDayTime$WHENCalledTHEN168AndParametersAreWrittenToTheSerialBus) {
	OI_tc.setDayTime(TUESDAY, clock_time_t(11,23));
	
	ASSERT_EQ(168, static_cast<uint_opt8_t>(serial_bus[0]));
	EXPECT_EQ(2, static_cast<uint_opt8_t>(serial_bus[1]));
	EXPECT_EQ(11, static_cast<uint_opt8_t>(serial_bus[2]));
	EXPECT_EQ(23, static_cast<uint_opt8_t>(serial_bus[3]));
}

TEST_F(AllSystemsGoOIModePASSIVE, setDayTime$WHENCalledWithInvalidTimeParametersTHENCorrespondingDayIsIgnored) {
	for ( int i = 24 ; i <= 255 ; ++i ) {
		for ( int j = 60 ; j <= 255 ; ++j ) {
			EXPECT_EQ(INVALID_PARAMETER, OI_tc.setDayTime(TUESDAY, clock_time_t(i, j))) << "Accepted time H:" << i << " M:" << j;
		}
	}
}

TEST_F(AllSystemsGoOIModePASSIVE, setDayTime$WHENTimeParameterIsInvalidTHENNoDataIsWrittenToSerialBus) {
	EXPECT_EQ(INVALID_PARAMETER, OI_tc.setDayTime(TUESDAY, clock_time_t(31,18)));
	ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
}

TEST_F(AllSystemsGoOIModeOFF, setDayTime$WHENOIModeIsOffTHENReturnsError) {
	ASSERT_EQ(OI_NOT_STARTED, OI_tc.setDayTime(TUESDAY, clock_time_t(11,23)));
}

TEST_F(SerialTransactionFailureOIModePASSIVE, setDayTime$WHENfnSerialWriteFailsTHENReturnsError) {
	ASSERT_EQ(SERIAL_TRANSFER_FAILURE, OI_tc.setDayTime(TUESDAY, clock_time_t(11,23)));
}

TEST_F(AllSystemsGoOIModeOFF, setDayTime$WHENOIModeIsOffTHENNoDataIsWrittenToSerialBus) {
	EXPECT_EQ(OI_NOT_STARTED, OI_tc.setDayTime(TUESDAY, clock_time_t(11,23)));
	ASSERT_EQ('\0', static_cast<uint_opt8_t>(serial_bus[0])) << "Bus: [" << serial_bus << "]";
}

} // namespace

/* Created and copyrighted by Zachary J. Fields. Offered as open source under the MIT License (MIT). */
