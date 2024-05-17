from .utils import *


class Part:
    def __init__(self, binary_info, part_cursor):
        self.header = {
            "HeaderType": {"size": 2, "offset": 0, "value": 0},
            "Flags": {"size": 2, "offset": 2, "value": 0},
            "HeaderSize": {"size": 4, "offset": 4, "value": 0},
            # ---------------------------------------- 8
            "Format": {"size": 4, "offset": 8, "value": 0},
            "Reserved": {"size": 2, "offset": 12, "value": 0},
            "FlowId": {"size": 2, "offset": 14, "value": 0},
            # ---------------------------------------- 16
            "FlowOffset": {"size": 8, "offset": 16, "value": 0},
            # ---------------------------------------- 24
            "DataSize": {"size": 8, "offset": 24, "value": 0},
            # ---------------------------------------- 32
            "DataOffset": {"size": 8, "offset": 32, "value": 0},
            # ---------------------------------------- 40
            "TypeSpecific": {"size": 8, "offset": [40], "value": []},
            # Note:
            # # ---------------------------------------- 40
            # "Dimension": {"size": 8, "offset": 40, "value": 0},
            # # ---------------------------------------- 48
            # "Padding": {"size": 4, "offset": 48, "value": 0},
            # # ---------------------------------------- 52
            # "InfoReserved": {"size": 4, "offset": 52, "value": 0},
            # # ---------------------------------------- 56
            # ...
        }

        for key in self.header:
            if not key == "TypeSpecific":
                set_offset(self.header, key, part_cursor + get_offset(self.header, key))
            set_value(self.header, key, load_from_binary(self.header, binary_info, key))

        # this num_typespecific includes Dimension, Padding and InfoReserved  
        num_typespecific = int((get_value(self.header, "HeaderSize") - 40) / 8)

        # setup typespecific offset for <Dimension> and <Padding with InfoReserved> (optinoal) 
        typespecific_offsets = [part_cursor + 40 + 8 * i for i in range(num_typespecific)]

        set_offset(self.header, "TypeSpecific", typespecific_offsets)
        set_value(self.header, "TypeSpecific", load_from_binary(self.header, binary_info, "TypeSpecific"))
        self.binary_info = binary_info

    def is_data_2D_image(self):
        return get_value(self.header, "HeaderType") & 0xFF00 == 0x4200

    def is_data_1D_image(self):
        return get_value(self.header, "HeaderType") & 0xFF00 == 0x4100

    def is_data_metadata(self):
        return get_value(self.header, "HeaderType") & 0xFF00 == 0x4000

    def get_dimension(self):
        dimension = get_value(self.header, "TypeSpecific")[0]
        if self.is_data_1D_image():
            return [dimension]
        elif self.is_data_2D_image():
            dimension_byte = dimension.to_bytes(8, 'little')
            width = int.from_bytes(dimension_byte[0:4], 'little')
            height = int.from_bytes(dimension_byte[4:], 'little')
            return [width, height]
        return []

    def get_data_size(self):
        return get_value(self.header, "DataSize")

    def get_data_offset(self):
        return get_value(self.header, "DataOffset")

    def get_data(self):
        size = self.get_data_size()
        start = self.get_data_offset()
        return self.binary_info[start:start + size]

    def get_typespecific_by_index(self, kth_typespecific):
        return  get_value(self.header, "TypeSpecific")[kth_typespecific]

    def get(self, key):
        return get_value(self.header, key)
