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

  // Detector tag indexes
  static constexpr std::string_view detectorTag = "detector";
  enum class Detector : short { ACO = 0 , AD, CPV, EMC, FMD, HMP, MCH, MTR, PHS, PMD, ITS, T0, TOF, TPC, TRD, V0};

  // Subsystem tag indexes
  static constexpr std::string_view subsystemTag = "subsystem";
  enum class Subsystem : short { QC = 16, Readout, DPL, CRU };

  // Single tag array
  static constexpr std::array<std::pair<std::string_view, std::string_view>, 20> TAG_ARRAY = {{
    {detectorTag, "ACO"sv}, // 0
    {detectorTag, "AD"sv},  // 1
    {detectorTag, "CPV"sv}, // 2
    {detectorTag, "EMC"sv}, // 3
    {detectorTag, "FMD"sv}, // 4
    {detectorTag, "HMP"sv}, // 5
    {detectorTag, "MCH"sv}, // 6
    {detectorTag, "MTR"sv}, // 7
    {detectorTag, "PHS"sv}, // 8
    {detectorTag, "PHS"sv}, // 9
    {detectorTag, "PMD"sv}, // 10
    {detectorTag, "ITS"sv}, // 11
    {detectorTag, "TOF"sv}, // 12
    {detectorTag, "TPC"sv}, // 13
    {detectorTag, "TRD"sv}, // 14
    {detectorTag, "V0"sv},  // 15
    {subsystemTag, "QC"sv},
    {subsystemTag, "Readout"sv},
    {subsystemTag, "DLP"sv},
    {subsystemTag, "CRU"sv}
  }};
}

} // namespace monitoring
} // namespace o2

#endif // ALICEO2_MONITORING_TAG_H
