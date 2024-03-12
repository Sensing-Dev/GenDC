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

int32_t get_descriptor_size(void* header) {
  if (!header) return 0;
  ContainerHeader* container_header = reinterpret_cast<ContainerHeader*>(header);
  return container_header->getDescriptorSize();
}

int64_t get_data_size(void* header) {
  if (!header) return 0;
  ContainerHeader* container_header = reinterpret_cast<ContainerHeader*>(header);
  return container_header->getDataSize();
}

int64_t get_component_count(void* container_header){
  ContainerHeader* container_header = reinterpret_cast<ContainerHeader*>(header);
  return container_header->getComponentCount();
}
void* get_component_header(void* container_header, int64_t component_index){
   ContainerHeader* container_header = reinterpret_cast<ContainerHeader*>(header);
   ContainerHeader* component_header = NULL;
   if(!container_header && container_header->getComponentCount() > component_index){
    component_header = new ContainerHeader(container_header->getComponentHeaders().at(component_index));
   }
  return component_header;
}
void destroy_container_descriptor(void* header) {
  ContainerHeader* container_header = reinterpret_cast<ContainerHeader*>(header);
  delete container_header;
  header = nullptr;
}


bool is_component_format(char* buf) {
  ComponentHeader component_header(buf);
  return component_header.isValidHeaderType();
}
bool is_valid_component(char* buf) {
  ComponentHeader component_header(buf);
  return component_header.isValid();
}

int32_t get_component_header_size(void* header) {
  if (!header) return 0;
  ComponentHeader* ComponentHeader = reinterpret_cast<ComponentHeader*>(header);
  return ComponentHeader->getHeaderSize();
}
int64_t get_component_data_offset(void* component_header){  
  ComponentHeader* ComponentHeader = reinterpret_cast<ComponentHeader*>(header);
  if (!ComponentHeader) return 0;
  ComponentHeader->getDataOffset(0);
}

int64_t get_component_data_size(void* header) {
  if (!header) return 0;
  ComponentHeader* ComponentHeader = reinterpret_cast<ComponentHeader*>(header);
  return ComponentHeader->getDataSize();
}

int64_t get_part_count(void* header){
  ComponentHeader* component_header = reinterpret_cast<ComponentHeader*>(header);
  return component_header->getPartCount();
}
void destroy_component_header(void* header) {
  ComponentHeader* component_header = reinterpret_cast<ComponentHeader*>(header);
  delete component_header;
  header = nullptr;
}

bool is_part_format(char* buf) {
  PartHeader part_header(buf);
  // TODO
  return part_header.isValidHeaderType();
}
bool is_part_component(char* buf) {
  // TODO
  PartHeader part_header(buf);
  return part_header.isValid();
}

void* create_part_header(char* buf) {
  PartHeader* header = new PartHeader(buf);
  return reinterpret_cast<void*>(header);
}
int32_t get_part_header_size(void* header) {
  if (!header) return 0;
  PartHeader* container_header = reinterpret_cast<PartHeader*>(header);
  return container_header->getHeaderSize();
}
int64_t get_part_data_size(void* header) {
  if (!header) return 0;
  PartHeader* container_header = reinterpret_cast<PartHeader*>(header);
  return container_header->getDataSize();
}
void destroy_part_header(void* header) {
  PartHeader* component_header = reinterpret_cast<PartHeader*>(header);
  delete component_header;
  header = nullptr;

}

} // extern "C"
