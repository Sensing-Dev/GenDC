import os
from pathlib import Path
import sys
import numpy as np

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

# GenICam Pixel Format Names and Values
# https://www.emva.org/wp-content/uploads/GenICamPixelFormatValues.pdf
Mono8 = 0x01080001 # component 0
Data8 = 0x01080116 # component 6, 7 and 8
Data16 = 0x01100118 # component 2, 3, 4
Data32 = 0x0120011A # component 1
Data64 = 0x0140011D # component 5 

# This part is to prevent this test.py from using pip-installed gendc_separator
test_dir = os.path.dirname(__file__)
root_dir = Path(test_dir).parent.absolute()
dev_mod_dir = os.path.join(root_dir, 'gendc_python')
info_log(dev_mod_dir + " is added to sys.path to use non-packaged gendc_separator.")

test_cases = {
    'all-enabled-0.bin' : {
        'num_components' : 9,
        'num_valid_components' : 9,
        'valid_component': [1, 1, 1, 1, 1, 1, 0, 0, 0],
        'SourceId' : [0x1001, 0x2001, 0x3001, 0x3002, 0x3003, 0x4001, 0x0001, 0x5001, 0x6001],
        'TypeId' : [1, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001],
        'DataSize' : [2073600, 0, 0, 0, 0, 0, 0, 0, 0],
        'Format' : [Mono8, Data32, Data16, Data16, Data16, Data64, Data8, Data8, Data8 ],
        'Dimension' : ['1920x1080', '800', '0', '0', '0', '0', '0', '0', '0']
    },
    # 'image-and-audio-0.bin' : {
    #     'num_components' : 9,
    #     'num_valid_components' : 9,
    #     'valid_component': [1, 1, 0, 0, 0, 0, 0, 0, 0],
    #     'SourceId' : [0x1001, 0x2001, 0x3001, 0x3002, 0x3003, 0x4001, 0x0001, 0x5001, 0x6001],
    #     'TypeId' : [1, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001],
    #     'DataSize' : [2073600, 0, 0, 0, 0, 0, 0, 0, 0],
    #     'Format' : [Mono8, Data32, Data16, Data16, Data16, Data64, Data8, Data8, Data8 ],
    #     'Dimension' : ['1920x1080', '800', '0', '0', '0', '0', '0', '0', '0']
    # }
}

sys.path.append(str(dev_mod_dir))
from gendc_separator import descriptor as gendc

if __name__ == "__main__":
    data_dir = os.path.join(test_dir, 'gendc_samples')
    if not (os.path.exists(data_dir) and os.path.isdir(data_dir)):
        error_log("Directory " + data_dir + " does not exist")

    # test_cases = [
    #    'all-enabled-',
    #     'image-and-audio-'
    # ]

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

                # # This section is for audio data check
                # if ith_component.get('SourceId') == 0x2001:
                #     first_part = ith_component.get_part_by_index(0)
                #     typespecific4 = first_part.get_typespecific_by_index(3)
                #     struct1 =  int.from_bytes(typespecific4.to_bytes(8, 'little')[1:2], "little")
                #     if struct1 == 0x00:
                #         print('\tAudio is monaural')
                        
                #     elif struct1 == 0x01:
                #         print('\tAudio is Stereo')

                #         audio_data_size = first_part.get_data_size()
                #         np_type = np.uint16
                #         num_channel = 2
                #         data_length = int(audio_data_size/np.dtype(np_type).itemsize/num_channel)
                #         audio_data = np.frombuffer(first_part.get_data(), dtype=np_type).reshape((data_length, num_channel))

                #         import matplotlib.pyplot as plt
                #         graph_color = ['orange', 'green']
                #         limit_samples = 48000
                        
                #         for ch in range(num_channel):
                #             datas = audio_data[:, ch]
                #             times = np.arange(0, data_length)
                #             plt.figure(figsize=(15, 5))
                #             plt.scatter(times[:limit_samples], datas[:limit_samples], color=graph_color[ch], s=0.1)
                #             plt.show()

                #         if np.array_equal(audio_data[0, :], audio_data[1, :]):
                #             print('Lch and Rch are identical')
                #         else:
                #             print('Lch and Rch are NOT identical')
                #     else:
                #         raise Exception('Invalid value')

                test_log('Component' + str(ith_component_idx) + ' Format', test_cases[test_file]['Format'][ith_component_idx], ith_component.get('Format'))

                num_part_count = ith_component.get_part_count()
                ith_component_datasize = 0
                for jth_part_idx in range(num_part_count):
                    jth_part = ith_component.get_part_by_index(jth_part_idx)
                    ith_component_datasize += jth_part.get_data_size()

                test_log('Component' + str(ith_component_idx) + ' DataSize', test_cases[test_file]['DataSize'][ith_component_idx], ith_component_datasize)

                print()

                    

                # cursor = cursor + descriptor_size + container_data_size





