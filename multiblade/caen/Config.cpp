// Copyright (C) 2018 - 2022 European Spallation Source, ERIC. See LICENSE file
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief using nlohmann json parser to read configurations from file
//===----------------------------------------------------------------------===//

#include <common/debug/Log.h>
#include <fstream>
#include <multiblade/caen/Config.h>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include <nlohmann/json.hpp>
#pragma GCC diagnostic pop
#include <common/debug/Trace.h>

namespace Multiblade {

///
Config::Config(std::string jsonfile) : ConfigFile(jsonfile) {

  loadConfigFile();

  if (!isConfigLoaded()) {
    throw std::runtime_error("Unable to load configuration file.");
  }

  Mappings = std::make_shared<DigitizerMapping>(Digitisers);

  assert(Mappings != nullptr);
}

///
void Config::loadConfigFile() {
  nlohmann::json root;

  if (ConfigFile.empty()) {
    LOG(INIT, Sev::Info, "JSON config - no config file specified, using default configuration");
    throw std::runtime_error("No config file provided.");
  }

  LOG(INIT, Sev::Info, "JSON config - loading configuration from file {}", ConfigFile);
  std::ifstream t(ConfigFile);
  std::string jsonstring((std::istreambuf_iterator<char>(t)),
                         std::istreambuf_iterator<char>());

  if (!t.good()) {
    XTRACE(INIT, ERR, "Invalid Json file: %s", ConfigFile.c_str());
    throw std::runtime_error("Caen config file error - requested file unavailable.");
  }

  try {
    root = nlohmann::json::parse(jsonstring);

  /// extract config parameters below

    auto det = root["Detector"].get<std::string>();
    if (det.compare("MB18") != 0) {
      LOG(INIT, Sev::Warning, "JSON config error - Expected MB18 detector");
      return;
    }

    auto instr = root["InstrumentGeometry"].get<std::string>();
    if (instr != "AMOR B") {
      LOG(INIT, Sev::Warning, "Caen config file error - 'AMOR B' geometry expected");
    }

    NWires  = root["wires"].get<unsigned int>();
    NStrips = root["strips"].get<unsigned int>();

    if ((NWires == 0) or (NStrips == 0)) {
      LOG(INIT, Sev::Warning, "JSON config - error: invalid geometry");
      return;
    }

    auto digitisers = root["DigitizerConfig"];
    for (auto &digitiser : digitisers) {
      struct DigitizerMapping::Digitiser digit;
      digit.index = digitiser["index"].get<unsigned int>();
      digit.digid = digitiser["id"].get<unsigned int>();
      Digitisers.push_back(digit);
      LOG(INIT, Sev::Info, "JSON config - Digitiser {}, offset {}", digit.digid, digit.index);
    }

    NCass = Digitisers.size() * 2 - 1;
    printf("Mixed1D2DMode: %zu digitizers -> %u cassettes\n", Digitisers.size(), NCass);

    MaxGapWire = root["MaxGapWire"].get<int>();
    MaxGapStrip = root["MaxGapStrip"].get<int>();
    MaxValidADC = root["MaxValidADC"].get<int>();

    TimeTickNS = root["TimeTickNS"].get<uint32_t>();
    MaxTofNS = root["MaxTofNS"].get<uint32_t>();
    assert(TimeTickNS != 0);
  }
  catch (...) {
    LOG(INIT, Sev::Error, "JSON config - error: Invalid Json file: {}", ConfigFile);
    return;
  }

  IsConfigLoaded = true;
}

}
