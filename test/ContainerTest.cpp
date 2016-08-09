#include <libfsbox/Container.h>

#include "gtest/gtest.h"
#include "TestUtils.h"

using namespace std;

TEST(Container, TryOpenCorrupted)
{
	const string fileName = "CORRUPTED.fsbox";

	ASSERT_TRUE(TestUtils::CreateSmallFile(fileName, "JUST SOME GIBBERISH"));

	Container container;
	EXPECT_FALSE(container.Open(fileName));
	EXPECT_FALSE(container.IsOpened());

	TestUtils::DeleteFile(fileName);
}

TEST(Container, BootstrapContainer)
{
	const string fileName = "EMPTY.fsbox";
	ASSERT_FALSE(TestUtils::FileExists(fileName));
	// Write
	{
		Container container;
		EXPECT_TRUE(container.Open(fileName));
		EXPECT_TRUE(container.IsOpened());
		EXPECT_GT(container.GetFileMapping().GetFileSize(), 0);
	}
	EXPECT_TRUE(TestUtils::FileExists(fileName));
	// Read
	{
		Container container;
		EXPECT_TRUE(container.Open(fileName));
		EXPECT_TRUE(container.IsOpened());
		EXPECT_GT(container.GetFileMapping().GetFileSize(), 0);
	}
	TestUtils::DeleteFile(fileName);
}