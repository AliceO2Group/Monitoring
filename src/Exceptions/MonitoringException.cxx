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
/// \file MonitoringException.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "MonitoringException.h"
#include "../MonLogger.h"

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{

MonitoringException::MonitoringException(const std::string& source, const std::string& message) : MonitoringException(-1, source, message) {}

MonitoringException::MonitoringException(int code, const std::string& source, const std::string& message)
  : message(message)
{
  MonLogger::Get(Severity::Warn) << "MonitoringException["
                                 << source << "] (" << code << "): " << message << MonLogger::End();
}

const char* MonitoringException::what() const throw()
{
  return message.c_str();
}

} // namespace monitoring
} // namespace o2
