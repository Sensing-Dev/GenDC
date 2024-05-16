from .part import Part
from .utils import *


class Component:
    def __init__(self, binary_info, component_cursor):

        self.part_headers = []
        self.header = {
            "HeaderType": {"size": 2, "offset": 0, "value": 8192},
            "Flags": {"size": 2, "offset": 2, "value": 0},
            "HeaderSize": {"size": 4, "offset": 4, "value": 0},
            # ---------------------------------------- 8
            "Reserved": {"size": 2, "offset": 8, "value": 0},
            "GroupId": {"size": 2, "offset": 10, "value": 0},
            "SourceId": {"size": 2, "offset": 12, "value": 0},
            "RegionId": {"size": 2, "offset": 14, "value": 0},
            # ---------------------------------------- 16
            "RegionOffsetX": {"size": 4, "offset": 16, "value": 0},
            "RegionOffsetY": {"size": 4, "offset": 20, "value": 0},
            # ---------------------------------------- 24
            "Timestamp": {"size": 8, "offset": 24, "value": 0},
            # ---------------------------------------- 32
            "TypeId": {"size": 8, "offset": 32, "value": 0},
            # ---------------------------------------- 40
            "Format": {"size": 4, "offset": 40, "value": 0},
            "Reserved2": {"size": 2, "offset": 44, "value": 0},
            "PartCount": {"size": 2, "offset": 46, "value": 0},
            # ---------------------------------------- 48
            "PartOffset": {"size": 8, "offset": [48], "value": []},
        }

        for key in self.header:
            if key == "PartOffset":
                set_offset(self.header, key,
                           [component_cursor + 48 + 8 * i for i in range(get_value(self.header, "PartCount"))])
            else:
                set_offset(self.header, key, component_cursor + get_offset(self.header, key))
            set_value(self.header, key, load_from_binary(self.header, binary_info, key))

        for cursor in self.header["PartOffset"]["value"]:
            self.part_headers.append(Part(binary_info, cursor))

    def is_valid(self):
        return get_value(self.header, "Flags") == 0

    def get_type_id(self):
        return get_value(self.header, "TypeId")

    def get_part_count(self):
        return get_value(self.header, "PartCount")

    def get_part_by_index(self, jth_part):
        return self.part_headers[jth_part]

    def get_part_header_value(self, key, jth_part):
        return self.part_headers[jth_part].get(key)

    def get(self, key):
        return get_value(self.header, key)
