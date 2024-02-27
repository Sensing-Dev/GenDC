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
int32_t get_descriptor_size(ContainerHeader* header);
int64_t get_data_size(ContainerHeader* header);

void destroy_component_header(void* header);
#ifdef __cplusplus
}
#endif

#endif /* GENDC_HEADER */
