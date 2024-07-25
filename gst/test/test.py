from gendc_python.gendc_separator import descriptor as gendc
import argparse
import os

import numpy as np
import cv2

import os

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
    cmd = [GST_LAUNCH, 'filesrc', 'location={0}'.format(input_bin)]
    gendcseparator = ['gendcseparator']
    
    if not num_output:
        filesink = ['filesink', 'location={0}/descriptor.bin'.format(output_dir)]
    else:
        filesink = concat_plugins(['queue'], ['filesink', 'location={0}/descriptor.bin'.format(output_dir)])
        for i in range(0, num_output):
            pipeline_part = concat_plugins(['gendcseparator0.'], ['queue'], ['filesink', 'location={0}/output{1}.bin'.format(output_dir, i)])
            filesink += pipeline_part

    return concat_plugins(cmd, gendcseparator, filesink)

def disp_msg(msg, flag = 'info'):
    print('[{0:5}]'.format(flag), msg)

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

            print('=== stderr ===')
            stderr = ret.stderr.decode('UTF-8').split('\r\n')
            for l in stderr:
                print(l)
    
    with open('{0}/descriptor.bin'.format(output_dir), mode='rb') as ifs:
        filecontent = ifs.read()
        try:
            gendc_container = gendc.Container(filecontent)
            descriptor_size = gendc_container.get_descriptor_size()
            container_data_size = gendc_container.get_data_size()
            disp_msg('Valid gendc descriptor')
        except :
            disp_msg('Wrong descriptor'.format(input_bin), 'error')
            exit(1)

    # check component 0 (image)
    if num_component > 0:
        filename = '{0}/output{1}.bin'.format(output_dir, 0)
        expected_image = generate_dummpy_image([1920, 1080])
        with open(filename, mode='rb') as ifs:
            filecontent = ifs.read()
            try:
                reshaped_data = np.frombuffer(filecontent, np.uint8).reshape([1080, 1920])
                if np.array_equal(reshaped_data, expected_image):
                    print("OK", filename, "is a valid image file")
                else:
                    disp_msg('{0} is a invalid image file'.format(filename), 'error')
                    exit(1)
            except Exception as err:
                disp_msg('{0} is a invalid image file'.format(filename), 'error')
                print(err)
                exit(1)
            