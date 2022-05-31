// Copyright (C) 2022 European Spallation Source ERIC, see LICENSE file
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief Unit test for AMORGeometry
///
//===----------------------------------------------------------------------===//

#include <multiblade/caen/AMORGeometry.h>
#include <common/testutils/TestBase.h>

//using namespace Multiblade;

#undef TRC_LEVEL
#define TRC_LEVEL TRC_L_DEB

class AMORGeometryTest : public TestBase {
protected:
  AMORGeometry g;
  void SetUp() override {}
  void TearDown() override {}
};

TEST_F(AMORGeometryTest, Constructor) {
  AMORGeometry g;
  ASSERT_TRUE(g.is1DDetector(0));
  ASSERT_TRUE(g.is1DDetector(9));
  ASSERT_FALSE(g.is1DDetector(10));
}

TEST_F(AMORGeometryTest, CheckIfXYCoordinates) {
  for (int cassette = 0; cassette < 10; cassette++) {
    for (int ch = 0; ch < 64; ch++) {
      ASSERT_TRUE(g.isYCoord(cassette, ch));
      ASSERT_FALSE(g.isXCoord(cassette, ch));
    }
  }

  for (int ch = 0; ch < 32; ch++) {
    ASSERT_TRUE(g.isYCoord(10, ch));
    ASSERT_FALSE(g.isXCoord(10, ch));
  }

  for (int ch = 32; ch < 64; ch++) {
    ASSERT_FALSE(g.isYCoord(10, ch));
    ASSERT_TRUE(g.isXCoord(10, ch));
  }
}

TEST_F(AMORGeometryTest, Cassette0) {
  for (int ch = 32; ch < 64; ch++) {
    ASSERT_TRUE(g.getYCoord(0, ch) >= 0);
  }
}

TEST_F(AMORGeometryTest, Cassette1) {
  for (int ch = 0; ch < 32; ch++) {
    ASSERT_TRUE(g.getYCoord(1, ch) >= 0);
  }
}

TEST_F(AMORGeometryTest, Cassette10Ch0Bug) {
    ASSERT_TRUE(g.getYCoord(10, 0) <= 351);
}




int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
