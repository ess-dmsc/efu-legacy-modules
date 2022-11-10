// Copyright (C) 2022 European Spallation Source, see LICENSE file
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Main entry for SoNDe
//===----------------------------------------------------------------------===//

#include <efu/MainProg.h>
#include <SoNDeBase.h>

int main(int argc, char * argv[]) {
  MainProg Main("gdgem", argc, argv);

  auto Detector = new SONDEIDEABase(Main.DetectorSettings);

  return Main.run(Detector);
}
