import os
from pathlib import Path
import sys
import numpy as np

import argparse
import importlib.util

def info_log(msg):
    print('[INFO]', msg)

def debug_log(msg):
    print('[DEBUG]', msg)

def error_log(msg):
    print('[ERR]', msg)
    raise Exception( msg)

def passed_or_failed(ret):
    if ret:
        return '{:<13}'.format('PASSED')
    else:
        return '{:>13}'.format('FAILED')

def test_log(test_item, expected_value, actual_value):
    ret = expected_value == actual_value
    msg = '\t{:20} '.format(test_item)

    if ret:
        msg += '[{:<12}]'.format('PASSED')
    else:
        msg += '[{:>12}]'.format('FAILED') 
    msg += ' '
    msg += ': Expected {:>10}, while it returned {:>10}'.format(expected_value, actual_value)

    print(msg)
    return ret

# This part is to prevent this test.py from using pip-installed gendc_separator
test_dir = os.path.dirname(__file__)
root_dir = Path(test_dir).parent.absolute()
dev_mod_dir = os.path.join(root_dir, 'gendc_python')
info_log(dev_mod_dir + " is added to sys.path to use non-packaged gendc_separator.")


sys.path.append(str(dev_mod_dir))
from gendc_separator import descriptor as gendc

sys.path.append(str(test_dir))
from data_properties import test_cases

if __name__ == "__main__":

    default_data_dir = os.path.join(root_dir, 'gendc_samples')

    parser = argparse.ArgumentParser(description="gendc separator test script")
    parser.add_argument('-d', '--directory', default=default_data_dir, type=str, help='Directory to load binfile')
    
    argvs = parser.parse_args()
    data_dir = argvs.directory
    
    if not (os.path.exists(data_dir) and os.path.isdir(data_dir)):
        error_log("Directory " + data_dir + " does not exist. Please use -h to set option.")


    for i, test_file in enumerate(test_cases):
        bin_file = os.path.join(data_dir, test_file)
        if not (os.path.exists(bin_file) and os.path.isfile(bin_file)):
            error_log('File ' + bin_file + ' does not exist')

        info_log('Test '+ str(i) + ': ' + os.path.basename(bin_file))

        with open(bin_file, mode='rb') as ifs:
            filecontent = ifs.read()
            cursor = 0

            # while cursor < len(filecontent):
            gendc_container = gendc.Container(filecontent[cursor:])
            descriptor_size = gendc_container.get_descriptor_size()
            container_data_size = gendc_container.get_data_size()

            num_component_count = gendc_container.get_component_count()
            test_log('Component count', test_cases[test_file]['num_components'], num_component_count)
            print()

            for ith_component_idx in range(num_component_count):
                ith_component = gendc_container.get_component_by_index(ith_component_idx)
                test_log('Component' + str(ith_component_idx) + ' Validity', test_cases[test_file]['valid_component'][ith_component_idx], ith_component.is_valid())
                test_log('Component' + str(ith_component_idx) + ' TypeId', test_cases[test_file]['TypeId'][ith_component_idx], ith_component.get_type_id())
                test_log('Component' + str(ith_component_idx) + ' SourceId', test_cases[test_file]['SourceId'][ith_component_idx], ith_component.get('SourceId'))
                test_log('Component' + str(ith_component_idx) + ' Format', test_cases[test_file]['Format'][ith_component_idx], ith_component.get('Format'))

                num_part_count = ith_component.get_part_count()
                ith_component_datasize = 0
                for jth_part_idx in range(num_part_count):
                    jth_part = ith_component.get_part_by_index(jth_part_idx)
                    ith_component_datasize += jth_part.get_data_size()

                test_log('Component' + str(ith_component_idx) + ' DataSize', test_cases[test_file]['DataSize'][ith_component_idx], ith_component_datasize)

                print()

                    

                # cursor = cursor + descriptor_size + container_data_size





