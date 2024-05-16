
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
        return [int.from_bytes(binary_info[cursor + off:cursor + off + size], "little") for off in offset]
    else:
        return int.from_bytes(binary_info[cursor + offset:cursor + offset + size], "little")

