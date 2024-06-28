# Test script for developers

`test.py` and `test.cpp` are for API check.

## STATUS

* `test.py`: WIP. Found the bug in https://github.com/Sensing-Dev/GenDC/issues/23
* `test.cpp`: Not implemented yet.

We are planning to use these script in CI but of couse you can use script to validate your local GenDC data!

## HOW TO USE (Local)

### 1. Prepare GenDC data binary file.

### 2. Edit/Create data_propertis.py. This file needs to have dictinary called `test_cases` which has the attribute with the key of binary file name and following dicrionary items.

* `num_components`: Integer, the total number of components
* `num_valid_components`: Integer,the total numbe of valid componetns
* `container_datasize`: Datasize written in the Container header
* `descriptor_size`: Descriptor size written in the Container header
* `valid_component`: List of true/false, whose length matches with `num_valid_components`
* `SourceId` : List of Integer, whose length matches with `num_valid_components`
* `TypeId` : List of Integer, whose length matches with `num_valid_components`
* `DataSize` : List of Integer, whose length matches with `num_valid_components`
* `Format` : List of Integer, whose length matches with `num_valid_components`
* `Dimension` : List of Integer, whose length matches with `num_valid_components`

(Planning to add more items in the future...)


e.g. 

```python
test_cases = {
    'output.bin' : {
        'num_components' : 9,
        'descriptor_size': 1280,
        'container_datasize': 2077344,
        'num_valid_components' : 9,
        'valid_component': [1, 1, 1, 1, 1, 1, 0, 0, 0],
        'SourceId' : [0x1001, 0x2001, 0x3001, 0x3002, 0x3003, 0x4001, 0x0001, 0x5001, 0x6001],
        'TypeId' : [1, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001],
        'DataSize' : [2073600, 3200, 32, 32, 32, 128, 0, 0, 0],
        'Format' : [Mono8, Data32, Data16, Data16, Data16, Data64, Data8, Data8, Data8 ],
        'Dimension' : ['1920x1080', '800', '16', '16', '16', '16', '0', '0', '0']
    },
    ...
}
```

### 3. Run the srcipt

```
python3 test.py -d <directory where you save binary files>
```
