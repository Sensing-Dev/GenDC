#ifndef PARTHEADER_H
#define PARTHEADER_H
#include <iostream>
#include <vector>
#include <array>

// for std::setw
#include <iomanip>

#include "Descriptor.h"

int getByteInFormat(int format){
    switch (format){
        case Mono12:
            return 2;
        case Data8:
            return 1;
        case Data16:
            return 2;
        case Data32:
            return 4;
        case Data32f:
            return 4;
        default:
            throw std::invalid_argument("wrong format\n");
    }
}
// namespace {

class PartHeader : public Header{
public: 
    PartHeader(){}
    // constructor with existing header info
    PartHeader(char* header_info, size_t offset = 0){

        offset_ = offset;
        part_ = header_info;

        size_t total_size = 0;
        offset += read(header_info, offset, HeaderType_);
        offset += read(header_info, offset, Flags_);
        offset += read(header_info, offset, HeaderSize_);
        offset += read(header_info, offset, Format_);
        offset += sizeof(Reserved_);
        offset += read(header_info, offset, FlowId_);
        offset += read(header_info, offset, FlowOffset_);
        offset += read(header_info, offset, DataSize_);
        offset += read(header_info, offset, DataOffset_);

        // get number of typespecific fields from HeaderSize_
        num_typespecific_ = getNumTypeSpecific(HeaderSize_);

        if (num_typespecific_ > 0){
            offset += read(header_info, offset, Dimension_[0]);
            offset += read(header_info, offset, Dimension_[1]);
            TypeSpecific_.push_back((static_cast<int64_t>(Dimension_[0]) << 32) + static_cast<int64_t>(Dimension_[1]));
        }
        if (num_typespecific_ > 1){
            offset += read(header_info, offset, Padding_[0]);
            offset += read(header_info, offset, Padding_[1]);
            TypeSpecific_.push_back((static_cast<int64_t>(Padding_[0]) << 16) + static_cast<int64_t>(Padding_[1]));
        }
        if (num_typespecific_ > 2){
            offset += sizeof(InfoReserved_);
            TypeSpecific_.push_back(static_cast<int64_t>(InfoReserved_));
        }
        int64_t typespecific_item;
        for (int i = 0; i < num_typespecific_ - 2; ++i){
            offset += read(header_info, offset, typespecific_item);
            TypeSpecific_.push_back(typespecific_item);
        }

//        TypeSpecific: [dimension, padding, inforeserved, framecount, ....]

    }

    PartHeader& operator=(const PartHeader& src) {
        HeaderType_ = src.HeaderType_;
        Flags_= src.Flags_;
        HeaderSize_= src.HeaderSize_;
        Format_= src.Format_;
        // Reserved_ = 0;
        FlowId_= src.FlowId_;
        FlowOffset_= src.FlowOffset_;
        DataSize_= src.DataSize_;
        DataOffset_= src.DataOffset_;

        Dimension_= src.Dimension_;
        Padding_= src.Padding_;
        TypeSpecific_= src.TypeSpecific_;
        return *this;
    }

    size_t generateDescriptor(char* ptr, size_t offset=0){
        offset = generateHeader(ptr, offset);
        return offset;
    }

    bool isData2DImage(){
        return HeaderType_ == 0x4200;
    }

    bool isData1DImage(){
        return HeaderType_ == 0x4100;
    }

    int64_t getDataOffset(){
        return DataOffset_;
    }

    int64_t getDataSize(){
        return DataSize_;
    }

    int16_t getHeaderType(){
        return HeaderType_;
    }

    std::vector<int32_t> getDimension(){
        std::vector<int32_t> ret;

        if (num_typespecific_ == 0){
            ret.push_back(-1);
            return ret;
        }

        switch(HeaderType_ & 0xFF00){
            case 0x4000:
                //metadata
            case 0x4100:{
                // 1D image
                int64_t first = static_cast<int64_t>(Dimension_[0]) << 32;
                int64_t second = static_cast<int64_t>(Dimension_[1]);
                ret.push_back(static_cast<int32_t>(first + second));
                return ret;
            }
            case 0x4200:{
                //2D image
                ret.push_back(Dimension_[0]);
                ret.push_back(Dimension_[1]);
                return ret;
            }
            default:{
                ret.push_back(-1);
                return ret;
            }

        }
    }

    void getData(char* dst){
        int64_t part_data_offset = getDataOffset();
        int64_t part_data_size = getDataSize();
        std::memcpy(dst, part_+part_data_offset, part_data_size);
    }

    int32_t getOffsetofTypeSpecific(int32_t kth_typespecific, int32_t typespecific_offset = 0){
        return offset_ +  DEFAULT_PART_HEADER_SIZE + 8 * (kth_typespecific - 1) + typespecific_offset;
    }

    int64_t getTypeSpecificByIndex(int32_t kth_typespecific ){
        return TypeSpecific_[kth_typespecific];
    }

    void displayHeaderInfo(){
        int total_size = 0;
        std::cout << "\nPART HEADER" << std::endl;
        total_size += displayItemInfo("HeaderType_", HeaderType_, 3, true);
        total_size += displayItemInfo("Flags_", Flags_, 3, true);
        total_size += displayItemInfo("HeaderSize_", HeaderSize_, 3);
        total_size += displayItemInfo("Format_", Format_, 3, true);
        total_size += displayItemInfo("Reserved_", Reserved_, 3, true);
        total_size += displayItemInfo("FlowId_", FlowId_, 3);
        total_size += displayItemInfo("FlowOffset_", FlowOffset_, 3);
        total_size += displayItemInfo("DataSize_", DataSize_, 3);
        total_size += displayItemInfo("DataOffset_", DataOffset_, 3);

        total_size += displayContainer("Dimension_", Dimension_, 3);
        total_size += displayContainer("Padding_", Padding_, 3);
        total_size += displayItemInfo("InfoReserved_", InfoReserved_, 3);

        total_size += displayContainer("TypeSpecific_", TypeSpecific_, 3);
    }

private:
    // you need parameters to create the object

    int getNumTypeSpecific(size_t header_size){
        return static_cast<int>(( header_size - 40 ) / 8);
    }

    size_t generateHeader(char* ptr, size_t offset = 0){
        // modify the order/items only when the structure is changed.
        // when you change this, don't forget to change copy constructor.
        size_t cpy_offset = offset;
        offset += write(ptr, offset, HeaderType_);
        offset += write(ptr, offset, Flags_);
        offset += write(ptr, offset, HeaderSize_);
        offset += write(ptr, offset, Format_);
        offset += write(ptr, offset, Reserved_);
        offset += write(ptr, offset, FlowId_);
        offset += write(ptr, offset, FlowOffset_);
        offset += write(ptr, offset, DataSize_);
        offset += write(ptr, offset, DataOffset_);
        offset += write(ptr, offset, Dimension_);
        offset += write(ptr, offset, Padding_);
        offset += write(ptr, offset, InfoReserved_);
        offset += write(ptr, offset, TypeSpecific_);

        if ((offset - cpy_offset) != HeaderSize_){
            std::cerr << "Part header size is wrong" << HeaderSize_ << " != " << offset - cpy_offset << std::endl;
        }
        return offset;
    }

    int16_t HeaderType_;
    int16_t Flags_;
    // int32_t HeaderSize_; 
    int32_t Format_;
    const int16_t Reserved_ = 0;
    int16_t FlowId_;
    int64_t FlowOffset_;
    int64_t DataSize_;
    int64_t DataOffset_;

    // optional
    std::array<int32_t, 2> Dimension_;
    std::array<int16_t, 2> Padding_;
    const int32_t InfoReserved_ = 0;
    std::vector<int64_t> TypeSpecific_;


    int32_t num_typespecific_;
    int32_t offset_ = 0; // currently part header offset
    char * part_;
};
// }
#endif /*PARTHEADER_H*/