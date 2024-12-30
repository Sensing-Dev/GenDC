from .pfnc_mapping import *
from .sfnc_values import *

def pfnc_convert_pixelformat(key):
    if isinstance(key, str):
        return pfnc_data_str_key[key]
    elif isinstance(key, int):
        return pfnc_data_int_key[key]
    else:
        raise Exception("Invalid key type")

