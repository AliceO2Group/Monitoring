///
/// @file    Version.cxx
/// @brief   Version Information
/// @author  Barthelemy von Haller
///

#include <Core/Version.h>
#include <sstream>

namespace AliceO2 {
namespace Monitoring {
namespace Core {

int Version::getMajor()
{
  return DATA_COLLECTOR_VERSION_MAJOR;
}

int Version::getMinor()
{
  return DATA_COLLECTOR_VERSION_MINOR;
}

int Version::getPatch()
{
  return DATA_COLLECTOR_VERSION_PATCH;
}

std::string Version::getString()
{
  std::ostringstream version;
  version << DATA_COLLECTOR_VERSION_MAJOR << '.' << DATA_COLLECTOR_VERSION_MINOR << '.' << DATA_COLLECTOR_VERSION_PATCH;
  return version.str();
}

std::string Version::getRevision()
{
  return DATA_COLLECTOR_VCS_REVISION;
}

std::string Version::getRevString()
{
  std::ostringstream version;
  version << getString() << '.' << DATA_COLLECTOR_VCS_REVISION;
  return version.str();
}

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2
