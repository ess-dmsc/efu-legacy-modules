// Copyright (C) 2022 European Spallation Source, ERIC. See LICENSE file
//===----------------------------------------------------------------------===//
///
/// \file
//===----------------------------------------------------------------------===//

#include <multiblade/MBCaenInstrument.h>
#include <common/testutils/SaveBuffer.h>
#include <common/testutils/TestBase.h>

using namespace Multiblade;


class MBCaenInstrumentTest : public TestBase {
protected:

  void SetUp() override { }
  void TearDown() override {}
};

/** Test cases below */
TEST_F(LokiInstrumentTest, Constructor) {
  MBCaenInstrument mb;
}


int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
