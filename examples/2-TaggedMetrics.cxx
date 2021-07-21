///
/// \file 2-TaggedMetrics.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/MonitoringFactory.h"

using namespace o2::monitoring;

int main()
{

  // Configure monitoring
  // Pass string with list of URLs as parameter
  auto monitoring = MonitoringFactory::Get("influxdbv2://localhost:8086/?bucket=adam&org=adam&token=v0SbCG4TRvHbHk82lckOT-T6iYY5VbGlXqOUnQlyaJNlT43eRnK_U8MllQT2kctwPFNwIqTO3HK4mnmGDCXk9g==");

  /// Add global tags
  monitoring->addGlobalTag("name", "test");
  monitoring->addGlobalTag(tags::Key::Subsystem, tags::Value::DPL);

  // now send an application specific metric with additional tags
  // 10 is the value
  // myMetric is the name of the metric
  // then add predefined tag
  monitoring->send(Metric{10, "myMetric"}.addTag(tags::Key::Detector, tags::Value::ACO));
  monitoring->send(Metric{10, "myMetric"}.addTag(tags::Key::CRU, 0));
  monitoring->send(Metric{10, "myMetric"}.addTag(tags::Key::CRU, 12));
  monitoring->send(Metric{10, "myMetric"}.addTag(tags::Key::CRU, 1234));
}
