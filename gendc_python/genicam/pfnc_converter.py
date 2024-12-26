from .data_int_key import data_int_key
from .data_str_key import data_str_key

def convert_pixelformat(key):
    if isinstance(key, str):
        return data_str_key[key]
    elif isinstance(key, int):
        return data_int_key[key]
    else:
        raise Exception("Invalid key type")