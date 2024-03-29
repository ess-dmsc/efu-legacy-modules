set(TEST_DATA_PATH "${REFDATA}/multiblade/2020")
if(EXISTS ${TEST_DATA_PATH})
  add_definitions("-DHAS_REFDATA")
  message(STATUS "ECDC: HAS multiblade reference data, using large datasets")
else()
  message(STATUS "ECDC: No multiblade reference data, skipping large datasets")
endif()

add_subdirectory(caen)
add_subdirectory(generators)
add_subdirectory(clustering)

include_directories(.)

#=============================================================================
# multiblade detector module and base test
#=============================================================================
set(caen_common_inc
  caen/AMORGeometry.h
  caen/Config.h
  caen/DataParser.h
  caen/Readout.h
  clustering/EventBuilder.h
  MBCaenBase.h
  MBCaenInstrument.h
  )
set(caen_common_src
  MBCaenBase.cpp
  MBCaenInstrument.cpp
  caen/Readout.cpp
  caen/DataParser.cpp
  clustering/EventBuilder.cpp
  caen/Config.cpp
  )

set(mbcaen_INC ${caen_common_inc})
set(mbcaen_SRC
  ${caen_common_src}
  main.cpp
  )
set(mbcaen_LIB efu_reduction)
create_executable(mbcaen)
add_dependencies(mbcaen efu_reduction)

#
set(MBCAENBaseTest_INC
  ${caen_common_inc}
)
set(MBCAENBaseTest_SRC
  MBCaenBaseTest.cpp
  ${caen_common_src}
)
create_test_executable(MBCAENBaseTest)
