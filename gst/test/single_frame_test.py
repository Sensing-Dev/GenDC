from gendc_python.gendc_separator import descriptor as gendc
import argparse
import os
import numpy as np
import sys
from pathlib import Path

DISPLAY_WARNING = 0

IS_WINDOWS = os.name == 'nt'
GST_LAUNCH = 'gst-launch-1.0'

import subprocess

def concat_plugins(*args):
    ret = args[0]

    if len(args) > 1:
        for arg in args[1:]:
            ret += ['!'] + arg

    return ret

def generate_command(input_bin, output_dir, num_output=None):

    is_windows = sys.platform.startswith('win')
    input_bin_path = Path(input_bin)
    output_dir_path = Path(output_dir)

    if is_windows:
        input_bin_str = input_bin_path.as_posix().replace('/', '//')
        descriptor_bin_str = Path(os.path.join(output_dir_path, f'descriptor.bin')).as_posix().replace('/', '//')
    else:
        input_bin_str = input_bin_path.as_posix()
        descriptor_bin_str = Path(os.path.join(output_dir_path, f'descriptor.bin')).as_posix()

    cmd = [GST_LAUNCH, 'filesrc', f'location={input_bin_str}']
    gendcseparator = ['gendcseparator', 'name=sep']
    
    if not num_output:
        filesink = ['filesink', f'location={descriptor_bin_str}']
    else:
        filesink = concat_plugins(['queue'], ['filesink', f'location={descriptor_bin_str}'])
        for i in range(0, num_output):
            if is_windows:
                output_bin_str = Path(os.path.join(output_dir_path, f'component{i}.bin')).as_posix().replace('/', '//')
            else:
                output_bin_str = Path(os.path.join(output_dir_path, f'component{i}.bin')).as_posix()
            pipeline_part = concat_plugins([f'sep.component_src{i}'], ['queue', 'max-size-buffers=1000'], ['filesink', f'location={output_bin_str}'])
            filesink += pipeline_part

    return concat_plugins(cmd, gendcseparator, filesink)

def disp_msg(msg, flag = 'info'):
    print('[{0:5}]'.format(flag), msg)

def test_ret(item, ret, err=''):
    if ret:
        msg = '{:15}{:<13}'.format(item, 'PASSED')
    else:
        msg = '{:15}{:>13}::{}'.format(item, 'FAILED', err)
    disp_msg(msg, 'TEST')
    return ret

def generate_dummpy_image(wh):
    w = wh[0]
    h = wh[1]
    row = row = np.arange(w)
    array = np.tile(row, (h, 1))
    for i in range(h):
        array[i] += i
    return array.astype('uint8')

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Test for Parse GenDC gst-plugin")
    parser.add_argument('-i', '--input', default='.', type=str, \
                        help='Input gendc file')
    parser.add_argument('-o', '--output', default='.', type=str, \
                        help='Output directory')
    parser.add_argument('-n', '--num_component', default=None, type=int, \
                        help='The number of components')
    
    input_bin = parser.parse_args().input
    output_dir = parser.parse_args().output
    num_component = parser.parse_args().num_component
    os.makedirs(output_dir, exist_ok=True)

    test_all_passed = True

    if not (os.path.isfile(input_bin)):
        raise Exception("Bin file " + input_bin + " does not exist")
    if not (os.path.isdir(output_dir)):
        raise Exception("Directory " + output_dir + " does not exist")

    disp_msg('Input: {0}'.format(input_bin), 'info')
    disp_msg('Output: {0}'.format(output_dir), 'info')

    command  = generate_command(input_bin, output_dir, num_component)


    disp_msg('Command: {0}'.format(' '.join(command)), 'info')

    ret  = subprocess.run(command, shell=IS_WINDOWS, capture_output=True)

    if ret.returncode:
        print('Failed')
        exit(1)
    else:
        if IS_WINDOWS:
            print()
            stdout = ret.stdout.decode('UTF-8').split('\r\n')
            print('=== stdout ===')
            for l in stdout:
                print(l)

            if DISPLAY_WARNING:
                print('=== stderr ===')
                stderr = ret.stderr.decode('UTF-8').split('\r\n')
                for l in stderr:
                    print(l)

    # check split files
    original_binary = open(input_bin, mode='rb').read()
    try:
        original_gendc_container = gendc.Container(original_binary)

        # check descriptor
        with open('{0}/descriptor.bin'.format(output_dir), mode='rb') as ifs:
            original_desctiptor_size = original_gendc_container.get_descriptor_size()
            filecontent = ifs.read()
            if len(filecontent) == original_desctiptor_size:
                if filecontent == original_binary[:original_desctiptor_size]:
                    test_all_passed &= test_ret('Descriptor', True)
                else:
                    test_all_passed &= test_ret('Descriptor', False)
            else:
                test_all_passed &= test_ret('Descriptor', False)

        if num_component:
            for n in range(num_component):

                filename = '{0}/component{1}.bin'.format(output_dir, n)

                with open(filename, mode='rb') as ifs:
                    filecontent = ifs.read()
                    cursor = 0

                    content = []

                    comp1 = original_gendc_container.get_component_by_index(n)
                    for part_index in range(comp1.get_part_count()):
                        part = comp1.get_part_by_index(part_index)
                        part_data = part.get_data() 
                        part_data_size =part.get_data_size()
                        content.append(filecontent[cursor:cursor + part_data_size] == part_data)

                        cursor += part_data_size

                    if cursor != len(filecontent):
                        test_all_passed &= test_ret('Comp{}'.format(n, part_index), False, 'Wrong file size (expected, actual)=({}, {})'.format(cursor, len(filecontent)))
                    
                    for part_index in range(comp1.get_part_count()):
                        test_all_passed &= test_ret('Comp{} Part{}'.format(n, part_index), content[part_index], 'Wrong file content')
    except:
        with open('{0}/descriptor.bin'.format(output_dir), mode='rb') as ifs:
            filecontent = ifs.read()
            if len(original_binary)==len(filecontent):
                if original_binary == filecontent:
                    test_all_passed &= test_ret('Image', True)
                else:
                    test_all_passed &= test_ret('Image', False, 'Wrong content')
            else:
                if original_binary == filecontent:
                    test_all_passed &= test_ret('Image', False, 'Wrong file size')
                else:
                    test_all_passed &= test_ret('Image', False, 'Wrong content and file size')  

    if test_all_passed:
        sys.exit(0)
    else:
        sys.exit(1)

                



                