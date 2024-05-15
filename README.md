# gendc-python

The gendc descriptor comprises three main elements: the container header, the component header, and the part header.
- **Container Header**
  - **Component 1 Header (RGB Planar Intensity)**
    - Part 1 Header
      - Part 1 Data (Red plane)
    - Part 2 Header
      - Part 2 Data (Green plane)
    - Part 3 Header
      - Part 3 Data (Blue plane)
  - **Component 2 Header (Range)**
    - Part 1 Header
      - Part 1 Data (Range)
  - **Component 3 Header (Metadata)**
    - Part 1 Header
      - Part 1 Data (Metadata Chunk)

## Quick start
```python
from gendc_python.gendc_separator import descriptor

gendc_container = descriptor.Container("binary_gendc_information")
```

## API

### Container
```
gendc_container = descriptor.Container("binary_gendc_information")
```

* `is_gendc_descriptor(binary_info)`: Checks if the descriptor in the binary data matches the specific signature for a GENDC descriptor.
  * Parameters:
    * binary_info (bytes): The binary data from which to load the signature.
  * Returns:
    * bool: True if the binary data is GenDC format
* `get_container_size()`: Calculates the total size of the container.
  * Returns:
    * int: total size in bytes of the container

* `get_container_size()`: Calculates the total size in bytes of the container by summing the sizes of the data and descriptor.
  * Returns:
    * int: total size in bytes of the container

* `get_descriptor_size()`: Retrieves the size in bytes of the descriptor part of the container.
  * Returns:
    * int: total size in bytes of the descriptor

* `get_component_count()`: Retrieves the count of components in the container.
  * Returns:
    * int: the count of components in the container.

*  `get_1st_component_idx_by_typeid(typeid)`: Finds the index of the first available component with the specified data type.
  * Parameters:
    * typeid (int): The target type ID to search for in components.
  * Returns:
    * int: The index of the first available component with the matching data type, or -1 if not found.

* `get_1st_component_idx_by_sourceid(sourceid)`: Finds the index of the first available component with the specified source ID.
  * Parameters:
    * sourceid (int): The target source ID to search for in components.
  * Returns:
    * int: The index of the first available component with the matching data type, or -1 if not found.

* `get_component_by_idx(ith_component)`: Retrieves the component header at the specified index.
  * Parameters:
    * ith_component (int): The index of the component to retrieve.
  * Returns:
    * Component object: The component object at the specified index.

* `get_component_header_value(key, ith_component)`: Retrieves a specific value from a component's header based on a given key.
  * Parameters:
    * ith_component (int): The index of the component to retrieve.
    * key (str): The value from component header to retrieve.
  * Returns:
    * Any: The value associated with the key from the specified component's header..
* `get_part_header_value(key, ith_component, jth_part)`: Retrieves a specific value from a part's header within a component based on a given key.
  * Parameters:
    * ith_component (int): The index of the component to retrieve.
    * jth_part (int): The index of the part within the component.
    * key (str): The key of the value from part header to retrieve.
  * Returns:
    * Any: The value associated with the key from the specified part's header within the component.
* `get(key)`: General method to retrieve a specific value from a container's header based on a given key.
  * Parameters:
    * key (str): The key of the value from part header to retrieve.
  * Returns:
    * Any: The value associated with the key from container header.

### Component
* `get_part_count()`: Retrieves the count of parts in the component.
  * Parameters:
    * key (str): The key of the value from part header to retrieve.
  * Returns:
    * int: The number of parts.

* `get_part_count()`: Retrieves a part based on its index.
  * Parameters: 
    * jth_part (int): The index of the part to retrieve.
  * Returns:
    * int: The number of parts.

* `get_part_by_idx(jth_part)`: Retrieves a part based on its index.
  * Parameters:
    * jth_part (int): The index of the part to retrieve.
  * Returns:
    * Part Object: The part header at the specified index.



* `get(key)`: Retrieves a specific value from container header based on a given key.
  * Parameters:
    * key (str): The key of the value from part header to retrieve.
  * Returns:
    * Any: The value associated with the key from container header.

* `get_part_header_value(key, jth_part)`: Retrieves a specific value from a part's header based on a given key.
  * Parameters:
    * jth_part (int): The index of the part to retrieve.
  * Returns:
    * Any: The value associated with the key from part header.
* `get(key)`: General method to retrieve a specific value from current component header based on a given key.
  * Parameters:
    * key (str): The key of the value from current component header to retrieve.
  * Returns:
    * Any: The value associated with the key from current component header.

## Part

* `is_data_1D_image()`: Determines if the part data is a 1D image based on the header type.
  * Returns:
    * bool: True if the header type indicates a 1D image, otherwise False.
* `is_data_2D_image()`: Determines if the part data is a 2D image based on the header type.
  * Returns:
    * bool: True if the header type indicates a 2D image, otherwise False.
* `get_dimension()`: Retrieves the dimensions of the data.
  * Returns:
    * list: A list containing the dimensions of the part. Returns an empty list if neither.

* `get_data_size()`: Retrieves the size of the data.
  * Returns:
    * int: The size of the data part in bytes

* `get_data_offset()`: Retrieves the offset at which the data part starts within the binary file.
  * Returns:
    * int: The offset of the data part in bytes.

 * `get_data()`: Extracts and returns the data part based on its size and offset.
   * Returns:
     * bytes: The binary data of the part.

* `get(key)`: General method to retrieve a specific value from current part header based on a given key.
  * Parameters:
    * key (str): The key of the value from current part header to retrieve.
  * Returns:
    * Any: The value associated with the key from current part header.


## Reference

GenDC v1.1 by emva https://www.emva.org/wp-content/uploads/GenICam_GenDC_v1_1.pdf
