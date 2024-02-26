#ifndef TOOLS_WRAPPER_H
#define TOOLS_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

// C wrapper function prototypes
bool is_gendc_format(char* buf);
bool is_valid_gendc(char* buf);
void get_gendc_version(char* buf, int8_t version[3]);
int32_t get_descriptor_size(char* buf, int container_version, int8_t version[3]);

#ifdef __cplusplus
}
#endif

#endif /* TOOLS_WRAPPER_H */
