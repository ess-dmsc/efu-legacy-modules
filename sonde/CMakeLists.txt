
include_directories(.)

#=============================================================================
# sonde detector module and base test
#=============================================================================

set(sonde_common_inc SoNDeBase.h ideas/Data.h)
set(sonde_common_src SoNDeBase.cpp ideas/Data.cpp)

set(sonde_INC ${sonde_common_inc})
set(sonde_SRC main.cpp ${sonde_common_src})
set(sonde_LIB MonitorLib)
create_executable(sonde)
add_dependencies(sonde MonitorLib)

#
set(SoNDeBaseTest_INC
  ${sonde_common_inc}
)
set(SoNDeBaseTest_SRC
  SoNDeBaseTest.cpp
  ${sonde_common_src}
)
create_test_executable(SoNDeBaseTest)



#=============================================================================
# unit tests
#=============================================================================

set(IDEASDataTest_SRC
  ideas/DataTest.cpp
  ideas/Data.cpp
  )
set(IDEASDataTest_INC
  ideas/Data.h
  ideas/TestData.h
  )
create_test_executable(IDEASDataTest)

#
set(IDEASGeometryTest_SRC ideas/GeometryTest.cpp)
set(IDEASGeometryTest_INC Geometry.h)
create_test_executable(IDEASGeometryTest)
