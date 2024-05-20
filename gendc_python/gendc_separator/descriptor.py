from .component import Component
from .utils import *


class Container:
    def __init__(self, binary_info):
        self.component_headers = []
        self.header = {
            "Signature": {"size": 4, "offset": 0, "value": "GNDC"},
            "Version": {"size": 3, "offset": 4, "value": 0},
            "Reserved": {"size": 1, "offset": 7, "value": 0},
            # ---------------------------------------- 8
            "HeaderType": {"size": 2, "offset": 8, "value": 4096},
            "Flags": {"size": 2, "offset": 10, "value": 0},
            "HeaderSize": {"size": 4, "offset": 12, "value": 0},
            # ---------------------------------------- 16
            "Id": {"size": 8, "offset": 16, "value": 0},
            # ---------------------------------------- 24
            "VariableFields": {"size": 2, "offset": 24, "value": 0},
            "Reserved2": {"size": 6, "offset": 26, "value": 0},
            # ---------------------------------------- 32
            "DataSize": {"size": 8, "offset": 32, "value": 0},
            # ---------------------------------------- 40
            "DataOffset": {"size": 8, "offset": 40, "value": 0},
            # ---------------------------------------- 48
            "DescriptorSize": {"size": 4, "offset": 48, "value": 0},
            "ComponentCount": {"size": 4, "offset": 52, "value": 0},
            # ---------------------------------------- 56
            "ComponentOffset": {"size": 8, "offset": [56], "value": []},
        }

        if not self.is_gendc_descriptor(binary_info):
            raise Exception("This is not valid GenDC")

        for key in self.header:
            if key == "ComponentOffset":
                set_offset(self.header, "ComponentOffset",
                           [56 + 8 * i for i in range(self.header["ComponentCount"]["value"])])
            set_value(self.header, key, load_from_binary(self.header, binary_info, key))

        for cursor in self.header["ComponentOffset"]["value"]:
            self.component_headers.append(Component(binary_info, cursor))

    def is_gendc_descriptor(self, binary_info):
        return load_from_binary(self.header, binary_info, "Signature") == 0x43444E47

    def get_container_size(self):
        return get_value(self.header, "DataSize") + get_value(self.header, "DescriptorSize")

    def get_data_size(self):
        return get_value(self.header, "DataSize")

    def get_descriptor_size(self):
        return get_value(self.header, "DescriptorSize")

    def get_component_count(self):
        return get_value(self.header, "ComponentCount")

    # search component #########################################################
    def get_1st_component_idx_by_typeid(self, target_type):
        # 1 : intensity
        # 0x0000000000008001 : GDC_METADATA

        for ith, ch in enumerate(self.component_headers):
            if ch.is_valid():
                if ch.get_type_id() == target_type:
                    return ith
        return -1

    def get_1st_component_idx_by_sourceid(self, target_sourceid):
        for ith, ch in enumerate(self.component_headers):
            if ch.is_valid():
                if ch.get("SourceId") == target_sourceid:
                    return ith
        return -1

    ############################################################################

    def get_component_by_index(self, ith_component):
        return self.component_headers[ith_component]


    def get_component_header_value(self, key, ith_component):
        return self.component_headers[ith_component].get(key)


    def get_part_header_value(self, key, ith_component, jth_part):
        return self.component_headers[ith_component].get_part_header_value(key, jth_part)

    def get(self, key):
        return get_value(self.header, key)

