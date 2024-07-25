from gendc_python.gendc_separator import descriptor as gendc
import argparse
import os

import numpy as np
import cv2

import os

IS_WINDOWS = os.name == 'nt'
GST_LAUNCH = 'gst-launch-1.0'

import subprocess

def disp_msg(msg, flag = 'info'):
    print('[{0:5}]'.format(flag), msg)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Test for Parse GenDC gst-plugin")
    parser.add_argument('-i', '--input', default='.', type=str, \
                        help='Input gendc file')
    parser.add_argument('-o', '--output', default='.', type=str, \
                        help='Output directory')
    
    input_bin = parser.parse_args().input
    output_dir = parser.parse_args().output
    os.makedirs(output_dir, exist_ok=True)

    if not (os.path.isfile(input_bin)):
        raise Exception("Bin file " + input_bin + " does not exist")
    if not (os.path.isdir(output_dir)):
        raise Exception("Directory " + output_dir + " does not exist")

    disp_msg('Input: {0}'.format(input_bin), 'info')
    disp_msg('Output: {0}'.format(output_dir), 'info')

    command = [GST_LAUNCH, 
        'filesrc', 'location={0}'.format(input_bin), '!', 'gendcseparator', '!', 'filesink', 'location={0}/descriptor.bin'.format(output_dir)]

    disp_msg('Command: {0}'.format(' '.join(command)), 'info')

    ret  = subprocess.run(command, shell=IS_WINDOWS, capture_output=True)

    if ret.returncode:
        print('Failed')
        exit(1)
    else:
        if IS_WINDOWS:
            print()
            stdout = ret.stdout.decode('UTF-8').split('\r\n')
            for l in stdout:
                print(l)
    
    with open('{0}/descriptor.bin'.format(output_dir), mode='rb') as ifs:
        filecontent = ifs.read()
        cursor = 0

        try:
            gendc_container = gendc.Container(filecontent)
            descriptor_size = gendc_container.get_descriptor_size()
            container_data_size = gendc_container.get_data_size()
            disp_msg('Valid gendc descriptor')
        except :
            disp_msg('Wrong descriptor'.format(input_bin), 'error')
            exit(1)