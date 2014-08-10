/* Created and copyrighted by Zachary J. Fields. All rights reserved. */

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../OI.h"

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
	):
		SCENARIO_VAR(90.0)
	{}
	
	//virtual ~Initialization() {}
	//virtual void SetUp() {}
	//virtual void TearDown() {}
	
	const float SCENARIO_VAR;
	OpenInterface_TC OI_tc;
};

/*
The first argument is the name of the test case (or fixture), and the
second argument is the test's name within the test case. Both names must
be valid C++ identifiers, and they should not contain underscore (_). A
test's full name consists of its containing test case and its individual
name. Tests from different test cases can have the same individual name.
*/

TEST_F(DefaultInitialization, WHENInitializedTHENModeIsSetToOFF) {
	ASSERT_EQ(NULL, OI_tc._mode);
}

} // namespace

/* Created and copyrighted by Zachary J. Fields. All rights reserved. */
