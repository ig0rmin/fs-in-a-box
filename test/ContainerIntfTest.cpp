#include <libfsbox/ContainerIntf.h>

#include "gtest/gtest.h"
#include "TestUtils.h"

#include <sstream>

using namespace FsBox;

class ContainerIntfTestsuite : public testing::Test
{
protected:
	ContainerIntfTestsuite():container(new(std::nothrow) Container)
	{
	}
	virtual void SetUp()
	{
		std::ostringstream os;
		os << "CONTAINERINTF" << counter++ << ".fsbox";
		fileName = os.str();
		ASSERT_TRUE(container->Open(fileName));
		ASSERT_TRUE(container->IsOpened());
	}

	virtual void TearDown()
	{
		container->Close();
		TestUtils::DeleteFile(fileName);
	}

	std::shared_ptr<Container> container;
	std::string fileName;

	static size_t counter;
};

size_t ContainerIntfTestsuite::counter = 0;

TEST_F(ContainerIntfTestsuite, DelteOpenedFile)
{
	ContainerIntf containerIntf(container);

	BlockHandle root = containerIntf.GetRoot();
	EXPECT_NE(0, root);
	BlockHandle file = containerIntf.CreateFile(root, "TestFile");
	EXPECT_NE(0, file);
	// we have this file opened, so can't remove it
	EXPECT_FALSE(containerIntf.DeleteFile(root, "TestFile"));
	// close
	containerIntf.CloseFile(file);
	// now we can delete
	EXPECT_TRUE(containerIntf.DeleteFile(root, "TestFile"));
}