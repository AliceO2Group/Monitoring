// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

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
enum class Key : unsigned short int { Hostname,
                                      Rolename,
                                      Name,
                                      Detector,
                                      Subsystem,
                                      CRU,
                                      FLP,
                                      EPN,
                                      Unit };

/// Tag keys array
static constexpr std::array<std::string_view, 9> TAG_KEY = {
  "hostname"sv, "rolenane"sv, "name"sv, "detector"sv, "subsystem"sv, "CRU"sv, "FLP"sv, "EPN"sv, "unit"sv
};

// Tag values
enum class Value : unsigned short int { ACO,
                                        AD,
                                        CPV,
                                        EMC,
                                        FMD,
                                        HMP,
                                        MCH,
                                        MTR,
                                        PHS,
                                        PMD,
                                        ITS,
                                        T0,
                                        TOF,
                                        TPC,
                                        TRD,
                                        V0,
                                        QC,
                                        Readout,
                                        DPL,
                                        CRU,
                                        Bytes,
                                        Megabytes,
                                        Gigabytes,
                                        Bits,
                                        Megabits,
                                        Gigabits,
                                        bps,
                                        Mbps,
                                        Gbps,
                                        Nanoseconds,
                                        Microseconds,
                                        Milliseconds,
                                        Seconds };

// Tag value array
static constexpr std::array<std::string_view, 33> TAG_VALUE = { {
  "ACO"sv,
  "AD"sv,      // 1
  "CPV"sv,     // 2
  "EMC"sv,     // 3
  "FMD"sv,     // 4
  "HMP"sv,     // 5
  "MCH"sv,     // 6
  "MTR"sv,     // 7
  "PHS"sv,     // 8
  "PHS"sv,     // 9
  "PMD"sv,     // 10
  "ITS"sv,     // 11
  "TOF"sv,     // 12
  "TPC"sv,     // 13
  "TRD"sv,     // 14
  "V0"sv,      // 15
  "QC"sv,      // 16
  "Readout"sv, //17
  "DPL"sv,     // 18
  "CRU"sv,     // 19
  "B"sv,       // 20
  "MB"sv,      // 21
  "GB"sv,      // 22
  "b"sv,       // 23
  "Mb"sv,      // 24
  "Gb"sv,      // 25
  "bps"sv,     // 26
  "Mbps"sv,    // 27
  "Gbps"sv,    //28
  "ns"sv,      //29
  "us"sv,      //30
  "ms"sv       //31
  "s"sv        // 32
} };
static constexpr std::string_view GetValue(const int value)
{
  return value >= 0 ? TAG_VALUE[value] : std::to_string(0 - value);
}
} // namespace tags

} // namespace monitoring
} // namespace o2

#endif // ALICEO2_MONITORING_TAG_H
