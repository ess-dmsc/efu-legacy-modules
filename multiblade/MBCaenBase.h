// Copyright (C) 2018-2020 European Spallation Source, see LICENSE file
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief MBCAEN detector base plugin interface definition
///
//===----------------------------------------------------------------------===//

#pragma once

#include <common/detector/Detector.h>
#include <multiblade/Counters.h>

namespace Multiblade {

// struct CAENSettings {
//   std::string FilePrefix{""};
//   std::string ConfigFile{""};
//   uint32_t H5SplitTime{0}; // split files every N seconds (0 is inactive)
//   bool Alignment{false}; // operations mode (1D) or alignment mode (2D)
// };



class CAENBase : public Detector {
public:
  CAENBase(BaseSettings const &settings);
  ~CAENBase() = default;

  void input_thread();
  void processing_thread();

protected:
  struct Counters Counters;
};

}
