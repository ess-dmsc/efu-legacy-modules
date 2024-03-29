
add_subdirectory(nmx)
add_subdirectory(srs)
add_subdirectory(tests)
add_subdirectory(generators)

get_filename_component(TEST_JSON_PATH "configs/vmm3.json" DIRECTORY)

include_directories(.)

#=============================================================================
# gdgem detector module and base test
#=============================================================================

set(pipeline_common_inc
  GdGemBase.h
  NMXConfig.h
  NMXStats.h
  nmx/Readout.h
  nmx/TrackSerializer.h
  nmx/AbstractBuilder.h
  srs/SRSMappings.h
  srs/SRSTime.h
  srs/ParserVMM3.h
  srs/BuilderVMM3.h
  srs/CalibrationFile.h # also works for VMM2
  ${ESS_COMMON_DIR}/reduction/multigrid/ModuleGeometry.h
  generators/BuilderReadouts.h
  generators/BuilderHits.h
  )
set(pipeline_common_src
  GdGemBase.cpp
  NMXConfig.cpp
  nmx/TrackSerializer.cpp
  srs/SRSMappings.cpp
  srs/SRSTime.cpp
  srs/ParserVMM3.cpp
  srs/BuilderVMM3.cpp
  srs/CalibrationFile.cpp  # Also works for VMM2
  ${ESS_COMMON_DIR}/reduction/multigrid/ModuleGeometry.cpp
  generators/BuilderReadouts.cpp
  generators/BuilderHits.cpp
  )

set(gdgem_SRC main.cpp ${GdGemBase_SRC}
  ${pipeline_common_src})
set(gdgem_INC ${pipeline_common_inc})
set(gdgem_LIB
  efu_reduction
  efu_essreadout
  )
create_executable(gdgem)
add_dependencies(gdgem efu_reduction efu_essreadout)
#
set(GdGemBaseTest_INC
  ${pipeline_common_inc}
  ${ESS_SOURCE_DIR}/common/testutils/TestUDPServer.h
  )
set(GdGemBaseTest_SRC
  GdGemBaseTest.cpp
  ${pipeline_common_src}
  )
create_test_executable(GdGemBaseTest)
target_compile_definitions(GdGemBaseTest
  PRIVATE TEST_JSON_PATH="${CMAKE_CURRENT_SOURCE_DIR}/${TEST_JSON_PATH}/")

#=============================================================================
# unit tests
#=============================================================================

set(GdGEMConfigTest_INC
  NMXConfig.h
  srs/SRSTime.h
  srs/SRSMappings.h
  srs/CalibrationFile.cpp
  ${ESS_COMMON_DIR}/reduction/multigrid/ModuleGeometry.h
  )
set(GdGEMConfigTest_SRC
  GdGemConfigTest.cpp
  NMXConfig.cpp
  srs/SRSTime.cpp
  srs/SRSMappings.cpp
  srs/CalibrationFile.cpp
  ${ESS_COMMON_DIR}/reduction/multigrid/ModuleGeometry.cpp
  )
set(GdGEMConfigTest_LIB efu_reduction)
create_test_executable(GdGEMConfigTest)
target_compile_definitions(GdGEMConfigTest
  PRIVATE TEST_JSON_PATH="${CMAKE_CURRENT_SOURCE_DIR}/${TEST_JSON_PATH}/")
