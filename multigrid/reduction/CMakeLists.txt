#=============================================================================
# Classes for Multi-grid event reduction pipeline
#=============================================================================

set(mg_reduction_obj_SRC
  ${ESS_COMMON_DIR}/reduction/multigrid/ModuleGeometry.cpp
  EventProcessingStats.cpp
  ModulePipeline.cpp
  Reduction.cpp
  )

set(mg_reduction_obj_INC
  ${ESS_COMMON_DIR}/reduction/multigrid/ModuleGeometry.h
  EventProcessingStats.h
  ModulePipeline.h
  Reduction.h
  )

add_library(MgReductionLib STATIC
  ${mg_reduction_obj_SRC}
  ${mg_reduction_obj_INC}
  )

enable_coverage_flags(MgReductionLib)
