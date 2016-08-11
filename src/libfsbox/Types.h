#ifndef __FSBOX_TYPES_H__
#define __FSBOX_TYPES_H__

#include <boost/iostreams/device/mapped_file.hpp>

namespace FsBox
{

typedef boost::iostreams::stream_offset stream_offset;
typedef stream_offset BlockHandle;

}// namespace FsBox
#endif