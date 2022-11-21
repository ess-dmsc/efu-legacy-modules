/** Copyright (C) 2016, 2017 European Spallation Source ERIC */

#include <gdgem/NMXConfig.h>
#include <common/testutils/TestBase.h>

std::string NoCalibration{""};

using namespace Gem;

class GdGEMConfigTest : public TestBase {
protected:
  std::string TestJsonPath {TEST_JSON_PATH};
};

// \todo improve everything about this

/** Test cases below */
TEST_F(GdGEMConfigTest, ConstructorDefaults) {
  NMXConfig NmxConfig;
  EXPECT_TRUE(NmxConfig.builder_type.empty());
  EXPECT_EQ(NmxConfig.calfile, nullptr);
}

TEST_F(GdGEMConfigTest, EventFilter) {
  EventFilter Filter;
  Event E; // use empty Event
  Filter.enforce_minimum_hits = false;
  EXPECT_TRUE(Filter.valid(E));

  Filter.enforce_minimum_hits = true;
  EXPECT_FALSE(Filter.valid(E));
}


TEST_F(GdGEMConfigTest, NoConfigFile) {
  EXPECT_THROW(NMXConfig NmxConfig("file_does_not_exist", NoCalibration);, std::runtime_error);
}

TEST_F(GdGEMConfigTest, DebugPrint) {
  MESSAGE() << "This is NOT a test, but simply exercises the debug print code" << "\n";
  NMXConfig NmxConfig;
  NmxConfig.filter.enforce_minimum_hits = true;
  auto Str = NmxConfig.debug();
  MESSAGE() << Str << "\n";
}

TEST_F(GdGEMConfigTest, JsonConfig) {
  NMXConfig NmxConfig(TestJsonPath + "vmm3.json", NoCalibration);
  EXPECT_EQ(100, NmxConfig.time_config.tac_slope_ns()); // Parsed from json
  EXPECT_EQ(20, NmxConfig.time_config.bc_clock_MHz());
  EXPECT_EQ(384, NmxConfig.geometry.nx());
  EXPECT_EQ(384, NmxConfig.geometry.ny());
  EXPECT_EQ(500, NmxConfig.matcher_max_delta_time);
  EXPECT_EQ("CenterMatcher", NmxConfig.matcher_name);
  EXPECT_EQ("GapClusterer", NmxConfig.clusterer_name);
  EXPECT_EQ("EventAnalyzer", NmxConfig.analyzer_name);
  EXPECT_EQ("center-of-mass", NmxConfig.time_algorithm);
  MESSAGE() << "\n" << NmxConfig.debug() << "\n";
}

TEST_F(GdGEMConfigTest, JsonConfigMG) {
  NMXConfig NmxConfig(TestJsonPath + "vmm3_mg.json", NoCalibration);
  MESSAGE() << "\n" << NmxConfig.debug() << "\n";
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
