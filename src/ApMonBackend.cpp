#include <iostream>
#include <sstream>
#include "Monitoring/ApMonBackend.h"

namespace AliceO2 {
namespace Monitoring {
namespace Core {

ApMonBackend::ApMonBackend(const std::string configurationFile)
{
	setProcessUniqueId();
	try {
		mApMon = new ApMon(const_cast<char *>(configurationFile.c_str()));
	} catch (std::runtime_error& e) {
		//throw FileNotFoundException(configurationFile);
	}
}

ApMonBackend::~ApMonBackend()
{
	delete mApMon;
}

void ApMonBackend::setProcessUniqueId()
{
	char hostname[255];
	gethostname(hostname, 255);
	
	std::ostringstream format;
        format << hostname << "." << getpid();
	
	mProcessUniqueId = format.str();
}

void ApMonBackend::send(const int value, const std::string name, const std::string entity, const unsigned long timestamp)
{
	mApMon->sendTimedParameter(const_cast<char *>(mProcessUniqueId.c_str()), const_cast<char *>(entity.c_str()),
		const_cast<char *>(name.c_str()), XDR_INT32, (char *) &value, timestamp);
}
void ApMonBackend::send(const double value, const std::string name, const std::string entity, const unsigned long timestamp)
{
	mApMon->sendTimedParameter(const_cast<char *>(mProcessUniqueId.c_str()), const_cast<char *>(entity.c_str()), 
		const_cast<char *>(name.c_str()), XDR_REAL64, (char *) &value, timestamp);
}
void ApMonBackend::send(const std::string value, const std::string name, const std::string entity, const unsigned long timestamp)
{
	mApMon->sendTimedParameter(const_cast<char *>(mProcessUniqueId.c_str()), const_cast<char *>(entity.c_str()),
		const_cast<char *>(name.c_str()), XDR_STRING, const_cast<char *>(value.c_str()), timestamp);
}
void ApMonBackend::send(const uint32_t value, const std::string name, const std::string entity, const unsigned long timestamp)
{
        mApMon->sendTimedParameter(const_cast<char *>(mProcessUniqueId.c_str()), const_cast<char *>(entity.c_str()),
                const_cast<char *>(name.c_str()), XDR_INT32, (char *) &value, timestamp);
}
} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

