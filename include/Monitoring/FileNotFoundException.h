/// \file    FileNotFoundException.h
/// \brief   File not found exception
///
/// \author  Vasco Barroso, CERN

#ifndef ALICEO2_MONITORING_CORE_FILENOTFOUNDEXCEPTION_H
#define ALICEO2_MONITORING_CORE_FILENOTFOUNDEXCEPTION_H

#include <stdexcept>

namespace AliceO2 {
namespace Monitoring {
namespace Core {
/// File not found Exception
class FileNotFoundException: public std::runtime_error
{
  public:
    /// Standard constructor
    /// \param  file  File path
    FileNotFoundException(std::string file)
        : std::runtime_error(file), mFilePath(file)
    {
    }

    /// Destructor
    virtual ~FileNotFoundException() throw ()
    {
    }

    /// Get exception message
    /// \return  Return error message
    const char* what() const throw ()
    {
      return ("File not found: " + mFilePath).c_str();
    }

    /// Get file path
    /// \return  Return file path
    const std::string& getFilePath() const
    {
      return mFilePath;
    }

  private:
    std::string mFilePath;  ///< File path
};

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_CORE_FILENOTFOUNDEXCEPTION_H
