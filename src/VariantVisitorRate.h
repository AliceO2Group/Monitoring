namespace AliceO2 
{
// ALICE O2 Monitoring system
namespace Monitoring 
{

class VariantVisitor : public boost::static_visitor<boost::variant<int, std::string, double, int64_t>>
{
private:
  int timestampCount;
public:
  VariantVisitor(int count) {
    timestampCount = count;
  }

  boost::variant<int, std::string, double, int64_t> operator()(const std::string& a, const std::string& b) const {
    std::cout << "skipping string" << std::endl;
  }

  template<typename T>
  boost::variant<int, std::string, double, int64_t> operator()(const T& a, const T& b) const {
    return (1000*(a - b)) / timestampCount;
  }

  template<typename T, typename U>
  boost::variant<int, std::string, double, int64_t> operator()(const T& a, const U& b) const {
    throw std::invalid_argument("can't operate on different types");
  }
};

}
}
