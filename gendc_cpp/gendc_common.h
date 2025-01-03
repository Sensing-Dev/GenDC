#ifndef GENDC_COMMON_H
#define GENDC_COMMON_H

#include <sstream>

#include "genicam/pfnc_mapping.h"
#include "gendc_separator/ContainerHeader.h"
#include "gendc_separator/Descriptor.h"

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

namespace gendc{

bool isGenDC(char* buf){
    int32_t signature;
    std::memcpy(&signature, buf + offset::SIGNATURE, sizeof(int32_t));

    if (signature != GENDC_SIGNATURE){
        std::cout << "[LOG ion-kit(gendc-separator)] The data is not genDC format" << std::endl;
        return false;
    }
    return true;
}

std::array<int8_t, 3> getVersion(char* buf){
    std::array<int8_t, 3> version;
    for (int i = 0; i < version.size(); ++i){
        std::memcpy(&version.at(i), buf + offset::VERSION + sizeof(int8_t)*i, sizeof(int8_t));
    }
    return version;
}

int32_t getDescriptorSize(char* buf, const int container_version, std::array<int8_t, 3>& v){
    int8_t hex_offset = 0x00;
    int32_t descriptor_size;

    try{
        std::memcpy(&descriptor_size, buf + offset::DESCRIPTOR_SIZE, sizeof(int32_t));
    }catch (std::out_of_range& e){
        std::stringstream ss;
        ss << "ERROR\t" << e.what() << ": "
            << "The version of container " 
            << v.at(0) - hex_offset << "."
            << v.at(1) - hex_offset << "."
            << v.at(2) - hex_offset << " is not supported.";
        const std::string error_message = ss.str();
        throw std::out_of_range(error_message);
    }catch(std::exception& e){
        throw e;
    } 
    return descriptor_size;
}

} // namespace gendc

#endif /*GENDC_COMMON_H*/