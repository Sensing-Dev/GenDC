#ifndef PFNC_CONVERTER_H
#define PFNC_CONVERTER_H

#include "data_int_key.h"
#include "data_str_key.h"

int32_t convertPixelFormat(std::string name){
    return data_str_key[name];
}

std::string convertPixelFormat(int32_t val){
    return data_int_key[val];
}

#endif /*PFNC_CONVERTER_H*/