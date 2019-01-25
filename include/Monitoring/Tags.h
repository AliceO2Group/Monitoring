///
/// \file Tags.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_TAG_H
#define ALICEO2_MONITORING_TAG_H

#include <string>

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{

namespace tags
{
  using namespace std::string_view_literals; 

  // Tag keys
  enum class Key : unsigned short int { Hostname, Rolename, Name, Detector, Subsystem, CRU, FLP, EPN };

  /// Tag keys array
  static constexpr std::array<std::string_view, 8> TAG_KEY = {
    "hostname"sv, "rolenane"sv, "name"sv, "detector"sv, "subsystem"sv, "CRU"sv, "FLP"sv, "EPN"sv
  };

  // Tag values
  enum class Value : unsigned short int { ACO, AD, CPV, EMC, FMD, HMP, MCH, MTR, PHS, PMD, ITS, T0, TOF, TPC, TRD, V0, QC, Readout, DPL, CRU};

  // Tag value array
  static constexpr std::array<std::string_view, 20> TAG_VALUE = {{
    "ACO"sv,
    "AD"sv,  // 1
    "CPV"sv, // 2
    "EMC"sv, // 3
    "FMD"sv, // 4
    "HMP"sv, // 5
    "MCH"sv, // 6
    "MTR"sv, // 7
    "PHS"sv, // 8
    "PHS"sv, // 9
    "PMD"sv, // 10
    "ITS"sv, // 11
    "TOF"sv, // 12
    "TPC"sv, // 13
    "TRD"sv, // 14
    "V0"sv,  // 15
    "QC"sv,
    "Readout"sv,
    "DPL"sv,
    "CRU"sv
  }};
  static constexpr std::string_view GetValue(const int value) {
    return value >= 0 ? TAG_VALUE[value] : std::to_string(0 - value);
  }
}

} // namespace monitoring
} // namespace o2

#endif // ALICEO2_MONITORING_TAG_H
