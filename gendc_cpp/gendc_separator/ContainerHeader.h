#ifndef CONTAINERHEADER_H
#define CONTAINERHEADER_H

#include "ComponentHeader.h"

class ContainerHeader : public Header{
public:

    ContainerHeader(){}

    ContainerHeader(char* descriptor){
        container_ = descriptor;

        size_t offset = 0;
        int32_t signature;
        int16_t header_type;

        // check if the container is GenDC
        offset += read(descriptor, offset, signature);
        if (signature != Signature_){
            std::cerr << "This ptr does NOT hace GenDC Signature" << std::endl;
        }

        for (int i = 0 ; i < Version_.size(); i++){
            int8_t v;
            offset += read(descriptor, offset, v);
            Version_.at(i) = v;
        }
        offset += sizeof(Reserved_);

        offset += read(descriptor, offset, header_type);
        if (header_type != HeaderType_){
            std::cerr << "wrong header type in container header" << std::endl;
        }
        offset += read(descriptor, offset, Flags_);
        offset += read(descriptor, offset, HeaderSize_);

        offset += read(descriptor, offset, Id_);
        offset += read(descriptor, offset, VariableFields_);
        offset += read(descriptor, offset, DataSize_);
        offset += read(descriptor, offset, DataOffset_);
        offset += read(descriptor, offset, DescriptorSize_);
        offset += read(descriptor, offset, ComponentCount_);

        for (int i = 0; i < ComponentCount_; ++i){
            int64_t single_component_offset;
            offset += read(descriptor, offset, single_component_offset);
            ComponentOffset_.push_back(single_component_offset);
        }

        for (int64_t & co : ComponentOffset_){
            component_header_.push_back(ComponentHeader(descriptor, co));
        }
    }

    ContainerHeader& operator=(const ContainerHeader& src) {
        component_header_ = src.component_header_;

        // Signature_ = 0x43444E47;
        Version_ = src.Version_;
        // Reserved_ = 0;
        // HeaderType_ = 0x1000;
        Flags_ = src.Flags_;
        HeaderSize_ = src.HeaderSize_;
        Id_ = src.Id_;
        VariableFields_ = src.VariableFields_;
        DataSize_ = src.DataSize_;
        DataOffset_ = src.DataOffset_;
        DescriptorSize_ = src.DescriptorSize_;
        ComponentCount_ = src.ComponentCount_;
        ComponentOffset_ = src.ComponentOffset_;

        return *this;
    }

    int32_t getDescriptorSize(){
        return DescriptorSize_;
    }

    int64_t getContainerSize(){
        return DescriptorSize_ + DataSize_;
    }

    int64_t getDataSize(){
        return DataSize_;
    }

    int32_t getComponentCount(){
        return ComponentCount_;
    }

    int32_t getFirstComponentIndexByTypeID(int64_t type_id){
        int cnt = 0;
        for (ComponentHeader &ch : component_header_){
            if (ch.isValidComponent()){
                if (type_id==ch.getTypeId()){
                    return cnt;
                }
            }
            ++cnt;
        }
        return -1;
    }

    int32_t getFirstComponentIndexBySourceId(int16_t source_id){
        int cnt = 0;
        for (ComponentHeader &ch : component_header_){
            if (ch.isValidComponent()){
                if (source_id==ch.getSourceId()){
                    return cnt;
                }
            }
            ++cnt;
        }
        return -1;
    }

    ComponentHeader getComponentByIndex(int ith_component_index){
        return component_header_[ith_component_index];
    }

    void displayHeaderInfo(){
        int total_size = 0;
        std::cout << "\nCONTAINER HEADER" << std::endl;
        total_size += displayItemInfo("Signature_", Signature_, 1, true);
        total_size += displayContainer("Version_", Version_, 1, true);
        total_size += displayItemInfo("Reserved_", Reserved_, 1);
        total_size += displayItemInfo("HeaderType_", HeaderType_, 1, true);
        total_size += displayItemInfo("Flags_", Flags_, 1, true);
        total_size += displayItemInfo("HeaderSize_", HeaderSize_, 1);
        total_size += displayItemInfo("Id_", Id_, 1);
        total_size += displayItemInfo("VariableFields_", VariableFields_, 1, true);
        total_size += displayItemInfo("DataSize_", DataSize_, 1);
        total_size += displayItemInfo("DataOffset_", DataOffset_, 1);
        total_size += displayItemInfo("DescriptorSize_", DescriptorSize_, 1);
        total_size += displayItemInfo("ComponentCount_", ComponentCount_, 1);

        total_size += displayContainer("ComponentOffset_", ComponentOffset_, 1);

    }

private:
    size_t generateHeader(char* ptr){
        // modify the order/items only when the structure is changed.
        // when you change this, don't forget to change copy constructor.
        size_t offset = 0;
        offset += write(ptr, offset, Signature_);
        offset += write(ptr, offset, Version_);
        offset += write(ptr, offset, Reserved_);
        offset += write(ptr, offset, HeaderType_);
        offset += write(ptr, offset, Flags_);
        offset += write(ptr, offset, HeaderSize_);
        offset += write(ptr, offset, Id_);
        offset += write(ptr, offset, VariableFields_);
        offset += write(ptr, offset, DataSize_);
        offset += write(ptr, offset, DataOffset_);
        offset += write(ptr, offset, DescriptorSize_);
        offset += write(ptr, offset, ComponentCount_);
        offset += write(ptr, offset, ComponentOffset_);

        if ( offset != HeaderSize_){
            std::cerr << "Container header size is wrong" << HeaderSize_ << " != " << offset << std::endl;
        }
        return offset;
    }

    // variables to interpret user input
    std::vector<ComponentHeader> component_header_;

    // member variables
    const int32_t Signature_ = 0x43444E47;
    std::array<int8_t, 3> Version_;
    const int8_t Reserved_ = 0;
    const int16_t HeaderType_ = 0x1000;
    int16_t Flags_;
    // int32_t HeaderSize_;
    int64_t Id_;
    int64_t VariableFields_; //including 6 Byte-wide Reserved
    int64_t DataSize_;
    int64_t DataOffset_;
    int32_t DescriptorSize_;
    int32_t ComponentCount_;
    std::vector<int64_t> ComponentOffset_;

    char* container_;
};

#endif /*CONTAINERHEADER_H*/
