// Copyright (C) 2022 European Spallation Source, see LICENSE file
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Main entry for Multiblade
//===----------------------------------------------------------------------===//

#include <efu/MainProg.h>
#include <JalousieBase.h>

int main(int argc, char * argv[]) {
  MainProg Main("jalousie", argc, argv);

  auto Detector = new Jalousie::JalousieBase(Main.DetectorSettings);

  return Main.run(Detector);
}
