namespace o2
{
// ALICE O2 Monitoring system
namespace monitoring
{
/// \brief Subtracts boost variants in order to calculate rate
class VariantVisitorRate : public boost::static_visitor<boost::variant<int, std::string, double, uint64_t>>
{
private:
  /// Timestamp difference in milliseconds
  int timestampCount;

public:
  /// Creates variant visitor functor
  /// \param count  timestamp difference in milliseconds
  VariantVisitorRate(int count) : timestampCount(count) {
  }

  /// Calculates rate only when two arguments of the same type are passed
  /// \return calculated rate in Hz
  template<
    typename T,
    typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type
  >
  double operator()(const T& a, const T& b) const {
    return (1000*(static_cast<double>(a) - b)) / timestampCount;
  }

  /// If arguments have different type an exception is raised
  /// \throws MonitoringException
  template<typename T, typename U>
  double operator()(const T&, const U&) const {
    throw MonitoringException("DerivedMetrics/Visitor", "Cannot operate on different or non-numeric types");
  }
};

} // namespace monitoring
} // namespace o2
