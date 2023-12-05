import json
import copy
import sys
import os

container_header_template = {
    "Signature" : {"size": 4, "offset": 0, "value" : "GNDC"},
    "Version" : {"size": 3, "offset": 4, "value" : 0},
    "Reserved" : {"size": 1, "offset": 7, "value" : 0},
    # ---------------------------------------- 8
    "HeaderType" : {"size": 2, "offset": 8, "value" : 4096},
    "Flags" : {"size": 2, "offset": 10, "value" : 0},
    "HeaderSize" : {"size": 4, "offset": 12, "value" : 0},
    # ---------------------------------------- 16
    "Id" : {"size": 8, "offset": 16, "value" : 0},
    # ---------------------------------------- 24
    "VariableFields" : {"size": 2, "offset": 24, "value" : 0},
    "Reserved2" : {"size": 6, "offset": 26, "value" : 0},
    # ---------------------------------------- 32
    "DataSize" : {"size": 8, "offset": 32, "value" : 0},
    # ---------------------------------------- 40
    "DataOffset" : {"size": 8, "offset": 40, "value" : 0},
    # ---------------------------------------- 48
    "DescriptorSize" : {"size": 4, "offset": 48, "value" : 0},
    "ComponentCount" : {"size": 4, "offset": 52, "value" : 0},
    # ---------------------------------------- 56
    "ComponentOffset" : {"size": 8, "offset": [56], "value" : []},
}

component_header_template = {
    "HeaderType" : {"size": 2, "offset": 0, "value" : 8192},
    "Flags" : {"size": 2, "offset": 2, "value" : 0},
    "HeaderSize" : {"size": 4, "offset": 4, "value" : 0},
    # ---------------------------------------- 8
    "Reserved" : {"size": 2, "offset": 8, "value" : 0},
    "GroupId" : {"size": 2, "offset": 10, "value" : 0},
    "SourceId" : {"size": 2, "offset": 12, "value" : 0},
    "RegionId" : {"size": 2, "offset": 14, "value" : 0},
    # ---------------------------------------- 16
    "RegionOffsetX" : {"size": 4, "offset": 16, "value" : 0},
    "RegionOffsetY" : {"size": 4, "offset": 20, "value" : 0},
    # ---------------------------------------- 24
    "Timestamp" : {"size": 8, "offset": 24, "value" : 0},
    # ---------------------------------------- 32
    "TypeId" : {"size": 8, "offset": 32, "value" : 0},
    # ---------------------------------------- 40
    "Format" : {"size": 4, "offset": 40, "value" : 0},
    "Reserved2" : {"size": 2, "offset": 44, "value" : 0},
    "PartCount" : {"size": 2, "offset": 46, "value" : 0},
    # ---------------------------------------- 48
    "PartOffset" : {"size": 8, "offset": [48], "value" : []},
}

part_header_template = {
    "HeaderType" : {"size": 2, "offset": 0, "value" : 0},
    "Flags" : {"size": 2, "offset": 2, "value" : 0},
    "HeaderSize" : {"size": 4, "offset": 4, "value" : 0},
    # ---------------------------------------- 8
    "Format" : {"size": 4, "offset": 8, "value" : 0},
    "Reserved" : {"size": 2, "offset": 12, "value" : 0},
    "FlowId" : {"size": 2, "offset": 14, "value" : 0},
    # ---------------------------------------- 16
    "FlowOffset" : {"size": 8, "offset": 16, "value" : 0},
    # ---------------------------------------- 24
    "DataSize" : {"size": 8, "offset": 24, "value" : 0},
    # ---------------------------------------- 32
    "DataOffset" : {"size": 8, "offset": 32, "value" : 0},
    # ---------------------------------------- 40
    "TypeSpecific" : {"size": 8, "offset": [40], "value" : []},
    # Note:
    # # ---------------------------------------- 40
    # "Dimension" : {"size": 8, "offset": 40, "value" : 0},
    # # ---------------------------------------- 48
    # "Padding" : {"size": 4, "offset": 48, "value" : 0},
    # "InfoReserved" : {"size": 4, "offset": 52, "value" : 0},
    # # ---------------------------------------- 56
    # ...
}

def is_valid_key(header_info, key):
    return key in header_info

# read and write
def get_offset(header_info, key):
    if not is_valid_key(header_info, key):
        raise Exception("Invalid key used")
    return header_info[key]["offset"]

def set_offset(header_info, key, offset):
    if not is_valid_key(header_info, key):
        raise Exception("Invalid key used")
    header_info[key]["offset"] = offset

# read only
def get_size(header_info, key):
    if not is_valid_key(header_info, key):
        raise Exception("Invalid key used")
    return header_info[key]["size"]

# read and write
def get_value(header_info, key):
    if not is_valid_key(header_info, key):
        raise Exception("Invalid key used")
    return header_info[key]["value"]

def set_value(header_info, key, value):
    if not is_valid_key(header_info, key):
        raise Exception("Invalid key used")
    header_info[key]["value"] = value 

def load_from_binary(header_info, binary_info, key, cursor=0):
    offset = get_offset(header_info, key)
    size = get_size(header_info, key)
    if type(offset) is list:
        return [int.from_bytes(binary_info[cursor+off:cursor+off+size], "little") for off in offset]
    else:
        return int.from_bytes(binary_info[cursor+offset:cursor+offset+size], "little")

class Container:
    def __init__(self, binary_info):
        self.component_headers = []
        self.header = copy.deepcopy(container_header_template)

        if not self.is_gendc_descriptor(binary_info):
            raise Exception("This is not valid GenDC")

        for key in self.header:
            if key == "ComponentOffset":
                set_offset(self.header, "ComponentOffset", [56 + 8 * i for i in range(self.header["ComponentCount"]["value"])])
            set_value(self.header, key, load_from_binary(self.header, binary_info, key))

        for cursor in self.header["ComponentOffset"]["value"]:
            self.component_headers.append(Component(binary_info, cursor))
    
    def is_gendc_descriptor(self, binary_info):
        if load_from_binary(self.header, binary_info, "Signature") == 0x43444E47:
            return True
        return False

    def get_container_size(self):
        return get_value(self.header, "DataSize") + get_value(self.header, "DescriptorSize")

    # search component #########################################################
    def get_first_get_datatype_of(self, target_type):
        # 1 : intensity
        # 0x0000000000008001 : GDC_METADATA

        for ith, ch in enumerate(self.component_headers):
            if ch.is_valid():
                if ch.get_datatype() == target_type:
                    return ith
                
        return -1

    def get_first_get_sourceid_of(self, target_sourceid):
        for ith, ch in enumerate(self.component_headers):
            if ch.is_valid():
                if ch.get("SourceId", -1) == target_sourceid:
                    return ith
                
        return -1
    ############################################################################

    def get(self, key, ith_component=-1, jth_part=-1):
        if ith_component != -1:
            return (self.component_headers[ith_component]).get(key, jth_part)
        else:
            return get_value(self.header, key)

class Component:
    def __init__(self, binary_info, component_cursor):

        self.part_headers = []
        self.header = copy.deepcopy(component_header_template)

        for key in self.header:
            if key == "PartOffset":
                set_offset(self.header, key, [component_cursor + 48 + 8 * i for i in range(get_value(self.header, "PartCount"))])
            else:
                set_offset(self.header, key, component_cursor + get_offset(self.header, key))
            set_value(self.header, key, load_from_binary(self.header, binary_info, key))

        for cursor in self.header["PartOffset"]["value"]:
            self.part_headers.append(Part(binary_info, cursor))


    def is_valid(self):
        return get_value(self.header, "Flags") == 0

    def get_datatype(self):
        return get_value(self.header, "TypeId")

    def get(self, key, jth_part=-1):
        if jth_part != -1:
            return self.part_headers[jth_part].get(key)
        else:
            return get_value(self.header, key)

class Part:
    def __init__(self, binary_info, part_cursor):

        self.header = copy.deepcopy(part_header_template)

        for key in self.header:
            if not key == "TypeSpecific":
                set_offset(self.header, key, part_cursor + get_offset(self.header, key))
            set_value(self.header, key, load_from_binary(self.header, binary_info, key))

        num_typespecific = int((get_value(self.header, "HeaderSize") - 40) / 8)
        set_offset(self.header, "TypeSpecific", [part_cursor + 40 + 8 * i for i in range(num_typespecific)])
        set_value(self.header, "TypeSpecific", load_from_binary(self.header, binary_info, "TypeSpecific"))

    def get(self, key):
        return get_value(self.header, key)
