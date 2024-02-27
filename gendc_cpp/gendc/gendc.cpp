#include "gendc.h" // C wrapper header

#include "ComponentHeader.h"
#include "ContainerHeader.h"
#include "Descriptor.h"
#include "PartHeader.h"
#include "tools.h" // Original C++ header

extern "C" {

bool is_gendc_format(char* buf) {
  return isGenDC(buf);
}
bool is_valid_container(char* buf) {
  bool ret = false;
  ContainerHeader container_header(buf);

  if (0 < container_header.getComponentCount()) {
    ret = true;
  }

  return ret;
}
void get_gendc_version(char* buf, int8_t version[3]) {
  std::array<int8_t, 3> v = getGenDCVersion(buf);
  std::copy(v.begin(), v.end(), version);
}

void* create_container_descriptor(char* buf) {
  ContainerHeader* header = new ContainerHeader(buf);
  return reinterpret_cast<void*>(header);
}

int32_t get_descriptor_size(ContainerHeader* header) {
  return header->getDescriptorSize();
}

int64_t get_data_size(ContainerHeader* header) {
  return header->getDataSize();
}

void destroy_component_header(void* header) {
  ComponentHeader* component_header = reinterpret_cast<ComponentHeader*>(header);
  delete component_header;
  header = nullptr;
}

} // extern "C"
