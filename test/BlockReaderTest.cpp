#include <libfsbox/BlockReader.hpp>

#include "gtest/gtest.h"

using namespace std;
using namespace BlockTypes;

TEST(BlockReader, ContainerHeader)
{
	const string fileName = "BLOCKREADER00.fsbox";

	Container container;
	ASSERT_TRUE(container.Open(fileName));

	BlockReader blockReader(container);

	auto containerHeader = blockReader.Get<ContainerHeader>(0);

	EXPECT_TRUE(containerHeader);
}