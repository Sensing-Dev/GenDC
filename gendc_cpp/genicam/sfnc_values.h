#ifndef GENICAM_SFNC_VALUES_H
#define GENICAM_SFNC_VALUES_H
// https://www.emva.org/wp-content/uploads/GenICam_SFNC_v2_7.pdf

namespace SFNC{
namespace sfnc{
// TypeId (i.e. ComponentIdValue)
#define SFNC_COMP_ID_VAL_UNDEFINED 0
#define SFNC_COMP_ID_VAL_INTENSITY 1
#define SFNC_COMP_ID_VAL_INFRARED 2
#define SFNC_COMP_ID_VAL_ULTRAVIOLET 3
#define SFNC_COMP_ID_VAL_RANGE 4
#define SFNC_COMP_ID_VAL_REFLECTANCE 5
#define SFNC_COMP_ID_VAL_CONFIDENCE 6
#define SFNC_COMP_ID_VAL_SCATTER 7
#define SFNC_COMP_ID_VAL_DISPARITY 8
#define SFNC_COMP_ID_VAL_MULTISPECTRAL 9
#define SFNC_COMP_ID_VAL_METADATA 0x8001
//TODO add Custom = 0xFF00-0xFFFE 
} // namespace sfnc
} // namespace SFNC

#endif /*GENICAM_SFNC_VALUES_H*/