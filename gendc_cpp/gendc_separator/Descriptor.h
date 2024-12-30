#ifndef GENDC_SEPARATOR_DESCRIPTOR_H
#define GENDC_SEPARATOR_DESCRIPTOR_H
#include <cstring>
#include <iostream>
#include <vector>

#include <iomanip>
#include <map>
#include <tuple>

namespace gendc{

const int32_t GENDC_SIGNATURE = 0x43444E47;

// *****************************************************************************
// const size and offset v1.0.1
// *****************************************************************************
const int32_t DEFAULT_CONT_HEADER_SIZE = 56;
const int32_t DEFAULT_COMP_HEADER_SIZE = 48;
const int32_t DEFAULT_PART_HEADER_SIZE = 40;

namespace offset{
const size_t DESCRIPTOR_SIZE = 48;
const size_t DATASIZE = 32;
const size_t DATA_OFFSET = 40;
}
// *****************************************************************************
// GenDC Part Header Types
// *****************************************************************************
namespace part_header_type{
const int16_t GDC_META = 0x4000;
const int16_t GDC_1D = 0x4100;
const int16_t GDC_2D = 0x4200;
}

// *****************************************************************************
// for container version and descriptor offset
// *****************************************************************************
namespace offset{
const size_t SIGNATURE = 0;
const size_t VERSION = 4;
}

// *****************************************************************************
// dispaly
// *****************************************************************************
namespace display{

#define DISPLAY_ITEM_WIDTH 16
#define DISPLAY_SIZE_WIDTH 4
#define DISPLAY_VALUE_WIDTH 10

enum level{
    default_display,
    container_header,
    component_header,
    part_header,
};

std::string display_indent(int level=default_display){
    std::string ret="";
    for (int i = 0; i < level; ++i){
        ret += "\t";
    }
    return ret;
} 

} // genicam::display





class Header{
public:
    size_t getHeaderSize(){
        return HeaderSize_;
    }

protected:
    template <typename T>
    void displayItem(T item, bool hex_format){
        if(sizeof(item) == sizeof(char)){
            displayItem(static_cast<int>(item), hex_format);
        }else{
            std::cout << std::right << std::setw(DISPLAY_VALUE_WIDTH);
            if (hex_format){
                std::cout << std::hex << "0x" << item << std::endl;
            }else{
                std::cout << std::dec << item << std::endl;
            }
        }

    }

    template <typename T>
    int displayItemInfo(std::string item_name, T item, int level=display::default_display, bool hex_format=false){
        std::string indent = display::display_indent(level);
        int sizeof_item = sizeof(item);
        std::cout << indent << std::right << std::setw(DISPLAY_ITEM_WIDTH) << item_name;
        std::cout << std::right << std::setw(DISPLAY_SIZE_WIDTH)  << " (" << sizeof_item << "):";
        displayItem<T>(item, hex_format);
        return sizeof_item;
    }

    template <typename T>
    int displayContainer(std::string container_name, const std::vector<T>&container, int level=display::default_display, bool hex=false){
        int total_size = 0;
        if (container.size() > 0){
            std::string key = container_name;
            for(int i=0; i < container.size(); ++i){
                total_size += displayItemInfo(i > 0 ? "" : key, container.at(i), level, hex);
            }
        }else{
            std::cout << display::display_indent(level) << std::right << std::setw(DISPLAY_ITEM_WIDTH) << container_name;
            std::cout << std::right << std::setw(DISPLAY_SIZE_WIDTH)  << " (" << 0 << "):\n";
        }
        return total_size;
    }

    template <typename T, size_t N>
    int displayContainer(std::string container_name, const std::array<T, N>&container, int level=display::default_display, bool hex=false){
        int total_size = 0;
        if (container.size() > 0){
            std::string key = container_name;
            for(int i=0; i < container.size(); ++i){
                total_size += displayItemInfo(i > 0 ? "" : key, container.at(i), level, hex);
            }
        }else{
            std::cout << display::display_indent(level) << std::right << std::setw(DISPLAY_ITEM_WIDTH) << container_name;
            std::cout << std::right << std::setw(DISPLAY_SIZE_WIDTH)  << " (" << 0 << "):\n";
        }
        return total_size;
    }
    

    template <typename T>
    size_t read(char* ptr, size_t offset, T& item){
        memcpy(&item, ptr+static_cast<int>(offset), sizeof(item));
        return sizeof(item);
    }

    template <typename T>
    size_t write(char* ptr, size_t offset, T item){
        memcpy(ptr+static_cast<int>(offset), &item, sizeof(item));
        return sizeof(item);
    }

    template <typename T>
    size_t writeContainer(char* ptr, size_t offset, std::vector<T>&container){
        size_t container_offset = 0;
        for (T& item : container){
            container_offset += write(ptr, offset + container_offset, item);
        }
        return container_offset;
    }

    template <typename T, size_t N>
    size_t writeContainer(char* ptr, size_t offset, std::array<T, N>&container){
        size_t container_offset = 0;
        for (T& item : container){
            container_offset += write(ptr, offset + container_offset, item);
        }
        return container_offset;
    }

protected:
    int32_t HeaderSize_ = 0;
};
}

#endif /*GENDC_SEPARATOR_DESCRIPTOR_H*/