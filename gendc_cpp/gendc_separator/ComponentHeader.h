#ifndef GENDC_SEPARATOR_COMPONENTHEADER_H
#define GENDC_SEPARATOR_COMPONENTHEADER_H

#include "PartHeader.h"
namespace gendc{
class ComponentHeader : public Header{
public:
    ComponentHeader(){}

    ComponentHeader(char* header_info, size_t offset = 0){

        container_ = header_info;
 
        int16_t header_type;
        offset += read(header_info, offset, header_type);
        if (header_type != HeaderType_){
            std::cerr << "wrong header type in component header" << std::endl;
        }

        offset += read(header_info, offset, Flags_);
        offset += read(header_info, offset, HeaderSize_);
        offset += sizeof(Reserved_);
        offset += read(header_info, offset, GroupId_);
        offset += read(header_info, offset, SourceId_);
        offset += read(header_info, offset, RegionId_);
        offset += read(header_info, offset, RegionOffsetX_);
        offset += read(header_info, offset, RegionOffsetY_);
        offset += read(header_info, offset, Timestamp_);
        offset += read(header_info, offset, TypeId_);
        offset += read(header_info, offset, Format_);
        offset += sizeof(Reserved2_);
        offset += read(header_info, offset, PartCount_);

        for (int i = 0; i < PartCount_; ++i){
            int64_t single_part_offset;
            offset += read(header_info, offset, single_part_offset);
            PartOffset_.push_back(single_part_offset);
        }

        for (int64_t & po : PartOffset_){
            partheader_.push_back(PartHeader(header_info, po));
        }

    }

    ComponentHeader& operator=(const ComponentHeader& src) {
        partheader_ = src.partheader_;
    
        // HeaderType_ = 0x2000;
        Flags_ = src.Flags_;
        HeaderSize_ = src.HeaderSize_;
        // Reserved_ = 0;
        GroupId_ = src.GroupId_;
        SourceId_ = src.SourceId_;
        RegionId_ = src.RegionId_;
        RegionOffsetX_ = src.RegionOffsetX_;
        RegionOffsetY_ = src.RegionOffsetY_;
        Timestamp_ = src.Timestamp_;
        TypeId_ = src.TypeId_;
        Format_ = src.Format_;
        // Reserved2_ = 0;
        PartCount_ = src.PartCount_;
        PartOffset_ = src.PartOffset_;
        return *this;
    }


    bool isValidComponent(){
        return Flags_ == 0;
    }

    int64_t getTypeId(){
        return TypeId_;
    }

    int16_t getSourceId(){
        return SourceId_;
    }

    int32_t getFormat(){
        return Format_;
    }

    int16_t getPartCount(){
        return PartCount_;
    }

    PartHeader getPartByIndex(int jth_part_index){
        return partheader_[jth_part_index];
    }

    void displayHeaderInfo(){
        int total_size = 0;
        std::cout << "\nCOMPONENT HEADER" << std::endl;
        total_size += displayItemInfo("HeaderType_", HeaderType_, 2, true);
        total_size += displayItemInfo("Flags_", Flags_, 2, true);
        total_size += displayItemInfo("HeaderSize_", HeaderSize_, 2);
        total_size += displayItemInfo("Reserved_", Reserved_, 2, true);
        total_size += displayItemInfo("GroupId_", GroupId_, 2, true);
        total_size += displayItemInfo("SourceId_", SourceId_, 2, true);
        total_size += displayItemInfo("RegionId_", RegionId_, 2, true);
        total_size += displayItemInfo("RegionOffsetX_", RegionOffsetX_, 2);
        total_size += displayItemInfo("RegionOffsetY_", RegionOffsetY_, 2);
        total_size += displayItemInfo("Timestamp_", Timestamp_, 2);
        total_size += displayItemInfo("TypeId_", TypeId_, 2, true);
        total_size += displayItemInfo("Format_", Format_, 2, true);
        total_size += displayItemInfo("Reserved2_", Reserved2_, 2, true);
        total_size += displayItemInfo("PartCount_", PartCount_, 2);

        total_size += displayContainer("PartOffset_", PartOffset_, 2);
    }

private:
    size_t generateHeader(char* ptr, size_t offset=0){
        // modify the order/items only when the structure is changed.
        // when you change this, don't forget to change copy constructor.
        size_t cpy_offset = offset;
        offset += write(ptr, offset, HeaderType_);
        offset += write(ptr, offset, Flags_);
        offset += write(ptr, offset, HeaderSize_);
        offset += write(ptr, offset, Reserved_);
        offset += write(ptr, offset, GroupId_);
        offset += write(ptr, offset, SourceId_);
        offset += write(ptr, offset, RegionId_);
        offset += write(ptr, offset, RegionOffsetX_);
        offset += write(ptr, offset, RegionOffsetY_);
        offset += write(ptr, offset, Timestamp_);
        offset += write(ptr, offset, TypeId_);
        offset += write(ptr, offset, Format_);
        offset += write(ptr, offset, Reserved2_);
        offset += write(ptr, offset, PartCount_);
        
        offset += writeContainer(ptr, offset, PartOffset_);

        if ((offset - cpy_offset) != HeaderSize_){
            std::cerr << "Component header size is wrong" << HeaderSize_ << " != " << offset - cpy_offset << std::endl;
        }
        return offset;
    }

    std::vector<PartHeader> partheader_;
    
    const int16_t HeaderType_ = 0x2000;
    int16_t Flags_;
    // int32_t HeaderSize_;
    const int16_t Reserved_ = 0;
    int16_t GroupId_;
    int16_t SourceId_;
    int16_t RegionId_;
    int32_t RegionOffsetX_;
    int32_t RegionOffsetY_;
    int64_t Timestamp_;
    int64_t TypeId_;
    int32_t Format_;
    const int16_t Reserved2_ = 0;
    int16_t PartCount_;
    std::vector<int64_t> PartOffset_;

    char* container_;
};
}

#endif /*GENDC_SEPARATOR_COMPONENTHEADER_H*/