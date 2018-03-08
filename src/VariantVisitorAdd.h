namespace o2
{
// ALICE O2 Monitoring system
namespace monitoring
{

class VariantVisitorAdd : public boost::static_visitor<boost::variant<int, std::string, double, uint64_t>>
{
public:
  /// Overloads operator() to avoid operating on strings
  /// \throws MonitoringInternalException
  double operator()(const std::string&, const std::string&) const {
    throw MonitoringInternalException("DerivedMetrics/VariantRateVisitor", "Cannot operate on string values");
  }

  /// Calculates rate only when two arguments of the same type are passed
  /// \return calculated rate in Hz
  int operator()(const int& a, const int& b) const {
    return a + b;
  }

  double operator()(const double& a, const double& b) const {
    return a + b;
  }

  uint64_t operator()(const uint64_t& a, const uint64_t& b) const {
    return a + b;
  }

  /// If arguments have different type an exception is raised
  /// \throws MonitoringInternalException
  template<typename T, typename U>
  double operator()(const T&, const U&) const {
    throw MonitoringInternalException("DerivedMetrics/VariantRateVisitor", "Cannot operate on different types");
  }
};

} // namespace monitoring
} // namespace o2
