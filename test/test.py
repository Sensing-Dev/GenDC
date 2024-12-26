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
    sys.exit(1)

def passed_or_failed(ret):
    if ret:
        return '{:<13}'.format('PASSED')
    else:
        return '{:>13}'.format('FAILED')

def is_error(test_item, expected_value, actual_value, show_log):
    failed = expected_value != actual_value
    msg = '{:32} '.format(test_item)

    if not failed:
        msg += '[{:<12}]'.format('PASSED')
    else:
        msg += '[{:>12}]'.format('FAILED') 
    msg += ' '
    msg += ': Expected {:>10} ({:>10})'.format(expected_value, actual_value)

    if show_log or failed:
        print(msg)
    return failed

# This part is to prevent this test.py from using pip-installed gendc_separator
test_dir = os.path.dirname(__file__)
root_dir = Path(test_dir).parent.absolute()
dev_mod_dir = os.path.join(root_dir, 'gendc_python')
info_log(dev_mod_dir + " is added to sys.path to use non-packaged gendc_separator.")

sys.path.append(str(dev_mod_dir))
from gendc_separator import descriptor as gendc
from genicam import pfnc_converter as genicam

test_cases = {
    'output.bin' : {
        'num_components' : 9,
        'descriptor_size': 1520,
        'container_datasize': 2076992,
        'num_valid_components' : 9,
        'valid_component': [1, 1, 1, 1, 1, 1, 0, 0, 0],
        'num_pard': [1, 2, 1, 1, 1, 3, 0, 0, 0],
        'SourceId' : [0x1001, 0x2001, 0x3001, 0x3002, 0x3003, 0x4001, 0x0001, 0x5001, 0x6001],
        'TypeId' : [1, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001],
        'DataSize' : [2073600, 3200, 32, 32, 32, 96, 0, 0, 0],
        'Format' : [
            genicam.convert_pixelformat("Mono8"), 
            genicam.convert_pixelformat("Data16"), 
            genicam.convert_pixelformat("Data16"),
            genicam.convert_pixelformat("Data16"),
            genicam.convert_pixelformat("Data16"), 
            genicam.convert_pixelformat("Data16"),
            genicam.convert_pixelformat("Data8"),
            genicam.convert_pixelformat("Data8"),
            genicam.convert_pixelformat("Data8") ],
        'Dimension' : ['1920x1080', '800', '16', '16', '16', '16', '0', '0', '0']
    },
}

def format_dim(dimension):
    if len(dimension) == 0:
        return '0'
    return 'x'.join([str(d) for d in dimension])

if __name__ == "__main__":

    default_data_dir = os.path.join(root_dir, 'test/generated_stub')

    parser = argparse.ArgumentParser(description="gendc separator test script")
    parser.add_argument('-d', '--directory', default=default_data_dir, type=str, help='Directory to load binfile')
    parser.add_argument('-v', '--verbose', \
                        action='store_true', help='Show all the logs regardless of the test results')
    argvs = parser.parse_args()
    data_dir = argvs.directory
    show_log = argvs.verbose
    
    if not (os.path.exists(data_dir) and os.path.isdir(data_dir)):
        error_log("Directory " + data_dir + " does not exist. Please use -h to set option.")

    num_failed = 0


    for i, test_file in enumerate(test_cases):
        bin_file = os.path.join(data_dir, test_file)
        if not (os.path.exists(bin_file) and os.path.isfile(bin_file)):
            error_log('File ' + bin_file + ' does not exist')

        info_log('Test '+ str(i) + ': ' + os.path.basename(bin_file))

        with open(bin_file, mode='rb') as ifs:
            filecontent = ifs.read()
            cursor = 0
            cnt_gendc = 0

            while cursor < len(filecontent):
                total_actual_datasize = 0
                gendc_container = gendc.Container(filecontent[cursor:])
                descriptor_size = gendc_container.get_descriptor_size()
                container_data_size = gendc_container.get_data_size()

                num_component_count = gendc_container.get_component_count()
                num_failed += is_error('Descriptor size', test_cases[test_file]['descriptor_size'], descriptor_size, show_log)
                num_failed += is_error('Container datasize', test_cases[test_file]['container_datasize'], container_data_size, show_log)
                num_failed += is_error('Component count', test_cases[test_file]['num_components'], num_component_count, show_log)

                if show_log:
                    print()

                for ith_component_idx in range(num_component_count):

                    ith_component = gendc_container.get_component_by_index(ith_component_idx)                   
                    num_failed += is_error('Component' + str(ith_component_idx) + ' Validity', test_cases[test_file]['valid_component'][ith_component_idx], ith_component.is_valid(), show_log)
                    num_failed += is_error('Component' + str(ith_component_idx) + ' TypeId', test_cases[test_file]['TypeId'][ith_component_idx], ith_component.get_type_id(), show_log)
                    num_failed += is_error('Component' + str(ith_component_idx) + ' SourceId', test_cases[test_file]['SourceId'][ith_component_idx], ith_component.get('SourceId'), show_log)
                    num_failed += is_error('Component' + str(ith_component_idx) + ' Format', test_cases[test_file]['Format'][ith_component_idx], ith_component.get('Format'), show_log)

                    num_part_count = ith_component.get_part_count()
                    ith_component_datasize = 0
                    for jth_part_idx in range(num_part_count):
                        jth_part = ith_component.get_part_by_index(jth_part_idx)
                        ith_component_datasize += jth_part.get_data_size()
                        dimension = jth_part.get_dimension()
                        num_failed += is_error('Component' + str(ith_component_idx) + ' Part' + str(jth_part_idx) + ' Dimension', test_cases[test_file]['Dimension'][ith_component_idx], format_dim(dimension), show_log)

                    num_failed += is_error('Component' + str(ith_component_idx) + ' DataSize', test_cases[test_file]['DataSize'][ith_component_idx], ith_component_datasize, show_log)

                    total_actual_datasize += ith_component_datasize

                    if show_log:
                        print()
                cursor += descriptor_size + container_data_size
                cnt_gendc += 1

    info_log(str(num_failed) + ' ERROR in this test in ' + str(cnt_gendc) + ' GenDC container(s)')

    if num_failed != 0:
        sys.exit(1)





