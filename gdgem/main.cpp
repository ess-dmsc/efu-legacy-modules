// Copyright (C) 2022 European Spallation Source, see LICENSE file
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Main entry for GdGem
//===----------------------------------------------------------------------===//

#include <efu/MainProg.h>
#include <GdGemBase.h>

int main(int argc, char * argv[]) {
  MainProg Main("gdgem", argc, argv);

  auto Detector = new GdGemBase(Main.DetectorSettings);

  return Main.run(Detector);
}
