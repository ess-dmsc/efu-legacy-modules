
add_subdirectory(test)

include_directories(.)

#=============================================================================
# jalousie detector module
#=============================================================================

set(jalousie_common_inc Config.h Readout.h SumoMappings.h rapidcsv.h)
set(jalousie_common_src Config.cpp Readout.cpp SumoMappings.cpp)

set(jalousie_INC
  ${jalousie_common_inc}
  JalousieBase.h
  )
set(jalousie_SRC
  ${jalousie_common_src}
  JalousieBase.cpp
  main.cpp
  )
set(jalousie_LIB efu_reduction)
create_executable(jalousie)
add_dependencies(jalousie efu_reduction)
