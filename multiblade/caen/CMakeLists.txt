set(MBReadoutTest_SRC
  Readout.cpp
  ReadoutTest.cpp
  )
set(MBReadoutTest_INC
  Readout.h
  )
create_test_executable(MBReadoutTest SKIP_MEMGRIND)

# GOOGLE BENCHMARK
set(MBDataParserBenchmarkTest_INC)
set(MBDataParserBenchmarkTest_SRC
  DataParser.cpp
  DataParserBenchmarkTest.cpp)
create_benchmark_executable(MBDataParserBenchmarkTest)
if(GOOGLE_BENCHMARK)
  target_link_libraries(MBDataParserBenchmarkTest h5cpp)
endif()

# Create excecutable for the test of geometry
set(MBDataParserTest_INC
  DataParser.h)
set(MBDataParserTest_SRC
  DataParser.cpp
  DataParserTest.cpp)
create_test_executable(MBDataParserTest)

# Create excecutable for the test of digitizer mapping
set(DigitizerMappingTest_INC
  DigitizerMapping.h)
set(DigitizerMappingTest_SRC
  DigitizerMappingTest.cpp)
create_test_executable(DigitizerMappingTest)

# Create excecutable for the AMOR geometry
set(AMORGeometryTest_INC
  AMORGeometry.h)
set(AMORGeometryTest_SRC
  AMORGeometryTest.cpp)
create_test_executable(AMORGeometryTest)

# Create excecutable for test of configuration
set(MBConfigTest_INC
  Config.h
  ${ESS_SOURCE_DIR}/common/testutils/SaveBuffer.h)
set(MBConfigTest_SRC
  Config.cpp
  ConfigTest.cpp
  )
create_test_executable(MBConfigTest)

#
# Tests relating to event building and benchmarks
#

# EventBuilderBenchmarkTest.cpp
set(MBEventBuilderBenchmarkTest_INC
  ../clustering/EventBuilder.h
)
set(MBEventBuilderBenchmarkTest_SRC
  ../clustering/EventBuilder.cpp
EventBuilderBenchmarkTest.cpp)
create_benchmark_executable(MBEventBuilderBenchmarkTest)
