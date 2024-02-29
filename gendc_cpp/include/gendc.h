#ifndef GENDC_HEADER
#define GENDC_HEADER


#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

// C wrapper function prototypes
bool is_gendc_format(char* buf);
bool is_valid_gendc(char* buf);
void get_gendc_version(char* buf, int8_t version[3]);
  
void* create_container_descriptor(char* buf);
int32_t get_descriptor_size(void* container_header);
int64_t get_data_size(void* container_header);
void destroy_container_descriptor(void* header);

bool is_component_format(char* buf);
bool is_valid_component(char* buf);

void* create_component_header(char* buf);
int32_t get_component_header_size(void* component_header);
int64_t get_component_data_size(void* component_header);
void destroy_component_header(void* header);


bool is_part_format(char* buf);
bool is_part_component(char* buf);

void* create_part_header(char* buf);
int32_t get_part_header_size(void* part_header);
int64_t get_part_data_size(void* part_header);
void destroy_part_header(void* header);
#ifdef __cplusplus
}
#endif

#endif /* GENDC_HEADER */
