/*

g++ test.cpp -o test \
-I ../gendc_cpp

*/

#include <iostream>
#include <filesystem>

#ifdef _WIN32
#include <Windows.h>
#include <stdio.h>
#include <filesystem>
#define PATH_MAX 4096
#else
#include <unistd.h>
#endif

#include <iostream>
#include <string>
#include <limits.h>

#include <fstream>

#include "gendc_separator/ContainerHeader.h"
#include "gendc_separator/tools.h"
#include "genicam/pfnc_converter.h"

const int32_t NUM_COMPONENT = 9;
const int32_t DESCRIPTOR_SIZE = 1520;
const int64_t CONTAINER_DATASIZE = 2076992;

const bool VALIDITY[9] = {1, 1, 1, 1, 1, 1, 0, 0, 0};
const int32_t NUM_PART[9] = {1, 2, 1, 1, 1, 3, 0, 0, 0};
const int16_t SOURCEID[9] = {0x1001, 0x2001, 0x3001, 0x3002, 0x3003, 0x4001, 0x0001, 0x5001, 0x6001};
const int64_t TYPEID[9] = {1, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001};
const int64_t DATASIZE[9] = {2073600, 3200, 32, 32, 32, 96, 0, 0, 0};
const int32_t FORMAT[9] = {
    convertPixelFormat("Mono8"), 
    convertPixelFormat("Data16"), 
    convertPixelFormat("Data16"), 
    convertPixelFormat("Data16"), 
    convertPixelFormat("Data16"), 
    convertPixelFormat("Data16"), 
    convertPixelFormat("Data8"), 
    convertPixelFormat("Data8"), 
    convertPixelFormat("Data8")};
const std::string DIMENSION[9] = {"1920x1080", "800", "16", "16", "16", "16", "0", "0", "0"};

#include <iomanip>

std::string GetExecutableDir() {
#ifdef _WIN32
    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, PATH_MAX);

    if (exePath){
        std::string str(exePath);
        std::filesystem::path path(str);
        // Note: assume it is built under build directory i.e. test\\build\\Release\\test.exe
        std::filesystem::path release_dir = path.parent_path();
        std::filesystem::path test_dir = release_dir.parent_path().parent_path();
        return test_dir.string();
    }
    return "";
#else
    char path[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);
    if (len != -1) {
        path[len] = '\0';
        std::string executable_path = std::string(path);
        size_t pos = executable_path.find_last_of("/");
        return (std::string::npos == pos) ? "" : executable_path.substr(0, pos);
    }
    return "";
#endif
}

template <typename T>
int isError(std::string item_name, T expected_value, T actual_value, bool display_log){
    bool failed = expected_value != actual_value;

    if (failed){
        std::cout
            << std::left << std::setw(32) << item_name
            << "["
            << std::right << std::setw(12) << "FAILED"
            << "]" 
            << " : Expected "
            << std::right << std::setw(10) << expected_value
            << " ("
            << std::right << std::setw(10) << actual_value
            << ")"
            << std::endl;
    }else{
        if (display_log){
            std::cout
                << std::left << std::setw(32) << item_name
                << "["
                << std::left << std::setw(12) << "PASSED"
                << "]" 
                << std::endl;
        }
    }

    if (failed){
        return 1;
    }

    return 0;

}

int main(int argc, char* argv[]){
    std::filesystem::path test_dir = GetExecutableDir();
    std::filesystem::path data_dir = test_dir / "generated_stub";
    bool show_log = false;

    if (argc > 1){
        for (int i = 1; i < argc; i++){
            if (strcmp(argv[i],"-d")==0|| strcmp(argv[i],"--directory")==0){
                data_dir = argv[++i];
            }else if (strcmp(argv[i],"-v")==0|| strcmp(argv[i],"--verbose")==0){
                show_log = true;
            }
        }
    }

    if (!std::filesystem::exists(data_dir)) {
        std::cerr << "Error: Directory '" << data_dir << "' does not exist.\n";
        return 1;
    }

    int num_failed = 0;

    std::filesystem::path bin_file = data_dir / "output.bin";

    std::ifstream ifs(bin_file, std::ios::binary);
    if (!ifs.is_open()){
        std::cerr << "Failed to open " << bin_file << std::endl;
        return 1;
    }
    ifs.seekg(0, std::ios::end);
    std::streampos filesize = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    char* filecontent = new char[filesize];

    if (!ifs.read(filecontent, filesize)) {
        std::cerr << "Failed to open bin file.\n";
        return 1;
    }

    if (!isGenDC(filecontent)){
        std::cerr << "Bin file is not GenDC format.\n";
        return 1;
    }

    int cnt_gendc = 0;

    ContainerHeader gendc_descriptor = ContainerHeader(filecontent);

    int32_t descriptor_size = gendc_descriptor.getDescriptorSize();
    int64_t container_data_size = gendc_descriptor.getDataSize();
    int32_t num_component_count = gendc_descriptor.getComponentCount();

    num_failed += isError("Descriptor size", DESCRIPTOR_SIZE, descriptor_size, show_log );
    num_failed += isError("Container datasize", CONTAINER_DATASIZE, container_data_size, show_log );
    num_failed += isError("Component count", NUM_COMPONENT, num_component_count, show_log );

    if (show_log){
        std::cout << std::endl;
    }

    for (int ith_comp_idx = 0; ith_comp_idx < num_component_count; ith_comp_idx++){
        ComponentHeader ith_component = gendc_descriptor.getComponentByIndex(ith_comp_idx);
        std::string prefix = "\tComponent count" + std::to_string(ith_comp_idx) + " ";

        num_failed += isError( prefix + "Validity", VALIDITY[ith_comp_idx], ith_component.isValidComponent(), show_log );
        num_failed += isError( prefix + "TypeId", TYPEID[ith_comp_idx], ith_component.getTypeId(), show_log );
        num_failed += isError( prefix + "SourceId", SOURCEID[ith_comp_idx], ith_component.getSourceId(), show_log );
        num_failed += isError( prefix + "Format", FORMAT[ith_comp_idx], ith_component.getFormat(), show_log );

        int num_part_count = ith_component.getPartCount();
        int64_t ith_comp_datasize = 0;
        for (int jth_part_idx = 0; jth_part_idx < num_part_count; jth_part_idx++){
            PartHeader jth_part = ith_component.getPartByIndex(jth_part_idx);
            prefix += "Part ";
            ith_comp_datasize += jth_part.getDataSize();

            std::vector<int32_t> dimension = jth_part.getDimension();
            std::string dim_str = std::to_string(dimension[0]);
            for (int k=1; k < dimension.size(); k++){
                dim_str += "x" + std::to_string(dimension[k]);
            }
            num_failed += isError(prefix + "Dimension", DIMENSION[ith_comp_idx], dim_str, show_log );
        }

        prefix = "\tComponent count" + std::to_string(ith_comp_idx) + " ";
        num_failed += isError(prefix + "DataSize", DATASIZE[ith_comp_idx], ith_comp_datasize, show_log );
        if (show_log){
            std::cout << std::endl;
        } 
    }

    cnt_gendc += 1;

    std::cout << num_failed << " ERROR in this test in " << cnt_gendc << " GenDC container(s)" << std::endl;

    delete[] filecontent;

    if (num_failed){
        return 1;
    }

    return 0;
}