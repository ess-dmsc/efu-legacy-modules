/** Copyright (C) 2016, 2017 European Spallation Source ERIC */

#include <multiblade/caen/DigitizerMapping.h>
#include <common/testutils/TestBase.h>

class DigitizerMappingTest : public TestBase {};

std::vector<struct Multiblade::DigitizerMapping::Digitiser> digits {
  {0, 137} , {1, 143}, {2, 142}, {3, 31}, {4, 33}, {5, 34}
};

/** Test cases below */
TEST_F(DigitizerMappingTest, CassetteValid) {
  Multiblade::DigitizerMapping mbg(digits);

  ASSERT_EQ(0, mbg.digitiserIndex(137));
  ASSERT_EQ(1, mbg.digitiserIndex(143));
  ASSERT_EQ(2, mbg.digitiserIndex(142));
  ASSERT_EQ(3, mbg.digitiserIndex(31));
  ASSERT_EQ(4, mbg.digitiserIndex(33));
  ASSERT_EQ(5, mbg.digitiserIndex(34));
}

TEST_F(DigitizerMappingTest, CassetteInValid) {
  Multiblade::DigitizerMapping mbg(digits);

  ASSERT_EQ(-1, mbg.digitiserIndex(-1));
  ASSERT_EQ(-1, mbg.digitiserIndex(0));
  ASSERT_EQ(-1, mbg.digitiserIndex(30));
  ASSERT_EQ(-1, mbg.digitiserIndex(32));
  ASSERT_EQ(-1, mbg.digitiserIndex(35));
  ASSERT_EQ(-1, mbg.digitiserIndex(136));
  ASSERT_EQ(-1, mbg.digitiserIndex(138));
  ASSERT_EQ(-1, mbg.digitiserIndex(141));
  ASSERT_EQ(-1, mbg.digitiserIndex(144));
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
