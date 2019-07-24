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
/// \file ProcessDetails.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_PROCESSDETAILS_H
#define ALICEO2_MONITORING_PROCESSDETAILS_H

#include <string>

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{

/// \brief Collects process and machine related details such as PID, process name, hostname
class ProcessDetails
{
 public:
  /// Generates all data
  ProcessDetails();

  /// PID getter
  unsigned int getPid() const;

  /// Process name getter
  std::string getProcessName() const;

  /// Hostname getter
  std::string getHostname() const;

 private:
  /// PID
  unsigned int mPid;

  /// Hostname
  std::string mHostname;

  /// Process name
  std::string mProcessName;

  /// Retrieves PID
  void generatePid();

  /// Retrieves process name
  void generateProcessName();

  /// Retrieves hostname
  void generateHostname();
};

} // namespace monitoring
} // namespace o2

#endif // ALICEO2_MONITORING_PROCESSDETAILS_H
