#ifndef GENIMCAM_UTIL_H
#define GENIMCAM_UTIL_H

#include "genicam/pfnc_mapping.h"
#include "gendc_separator/ContainerHeader.h"

namespace gendc{
namespace pfnc{
    int32_t convert_pixelformat(std::string name){
        return pfnc_data_str_key[name];
    }

    std::string convert_pixelformat(int32_t val){
        return pfnc_data_int_key[val];
    }

    
} // namespace pfnc
} // namespace gendc

#endif /*GENIMCAM_UTIL_H*/