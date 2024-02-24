#include "tools.h"      // Original C++ header
#include "gendc.h"  // C wrapper header
#include "ContainerHeader.h"
#include "Descriptor.h"
#include "PartHeader.h"
#include "ComponentHeader.h"


extern "C" {

  bool is_gendc_format(char* buf) {
    return isGenDC(buf);
  }
  bool is_valid_gendc(char* buf) {
    bool ret = false;
    ContainerHeader container_header(buf);

    if (0 < container_header.getComponentCount())
    {
      ret = true;
    }

    return ret;
  }
  void get_gendc_version(char* buf, int8_t version[3]) {
    std::array<int8_t, 3> v = getGenDCVersion(buf);
    std::copy(v.begin(), v.end(), version);
  }

  int32_t get_descriptor_size(char* buf, int container_version, int8_t v[3]) {
    std::array<int8_t, 3> versionArray;
    std::copy(v, v + 3, versionArray.begin());
    return getDescriptorSize(buf, container_version, versionArray);
  }

} // extern "C"
