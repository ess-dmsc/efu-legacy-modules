// Copyright (C) 2022 European Spallation Source, see LICENSE file
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Main entry for Bifrost
//===----------------------------------------------------------------------===//

#include <efu/MainProg.h>
#include <MBCaenBase.h>

int main(int argc, char * argv[]) {
  MainProg Main("mbcaen", argc, argv);

  auto Detector = new Multiblade::CAENBase(Main.DetectorSettings);

  return Main.run(Detector);
}
