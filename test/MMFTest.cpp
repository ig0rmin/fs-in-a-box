#include <libfsbox/MMF.h>

#include "gtest/gtest.h"


TEST(MMF, OpenEmpty)
{
	MemoryMappedFile mmf;

	EXPECT_FALSE(mmf.Open("No such file"));
}

