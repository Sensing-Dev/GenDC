# GenICam Pixel Format Names and Values
# https://www.emva.org/wp-content/uploads/GenICamPixelFormatValues.pdf
Mono8 = 0x01080001
Data8 = 0x01080116
Data16 = 0x01100118
Data32 = 0x0120011A
Data64 = 0x0140011D

### PLEASE EDIT FILE NAME AND ITS PROPERTIES ################################################
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
        'Format' : [Mono8, Data16, Data16, Data16, Data16, Data16, Data8, Data8, Data8 ],
        'Dimension' : ['1920x1080', '800', '16', '16', '16', '16', '0', '0', '0']
    },
}