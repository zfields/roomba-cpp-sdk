/* Created and copyrighted by Zachary J. Fields. All rights reserved. */

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../OIDecoder.h"

using namespace roomba::series500::oi;

  /******************/
 /* TESTABLE CLASS */
/******************/
class OIDecoder_TC : public OIDecoder {
  public:
  // insert protected members here
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
	
	OIDecoder_TC Decoder_tc;
};

/*
The first argument is the name of the test case (or fixture), and the
second argument is the test's name within the test case. Both names must
be valid C++ identifiers, and they should not contain underscore (_). A
test's full name consists of its containing test case and its individual
name. Tests from different test cases can have the same individual name.
*/

TEST_F(ObjectInitialization, constructor$WHENInitializedTHEN_) {
	
}

} // namespace

/* Created and copyrighted by Zachary J. Fields. All rights reserved. */
