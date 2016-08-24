#include <libfsbox/DirIntf.h>
#include <libfsbox/FileIntf.h>
#include <libfsbox/Container.h>

#include "gtest/gtest.h"
#include "TestUtils.h"

#include <vector>
#include <sstream>

using namespace FsBox;

class DirIntfTestsuite : public testing::Test
{
protected:
	virtual void SetUp()
	{
		std::ostringstream os;
		os << "DIRTEST" << counter++ << ".fsbox";
		fileName = os.str();
		ASSERT_TRUE(container.Open(fileName));
		ASSERT_TRUE(container.IsOpened());
		ASSERT_GT(container.GetFileMapping().GetFileSize(), 0);
	}

	virtual void TearDown()
	{
		container.Close();
		TestUtils::DeleteFile(fileName);
	}

	Container container;
	std::string fileName;

	static size_t counter;
};

size_t DirIntfTestsuite::counter = 0;

TEST_F(DirIntfTestsuite, GetRoot)
{
	DirIntf dirIntf(container);

	EXPECT_NE(0, dirIntf.GetRoot());
}

TEST_F(DirIntfTestsuite, CreateDir)
{
	DirIntf dirIntf(container);

	BlockHandle root = dirIntf.GetRoot();

	EXPECT_NE(0, root);
	BlockHandle dirA = dirIntf.CreateDir(root, "A");
	EXPECT_NE(0, dirA);
	BlockHandle dirB = dirIntf.CreateDir(root, "B");
	EXPECT_NE(0, dirB);
}

TEST_F(DirIntfTestsuite, OpenDir)
{
	{
		DirIntf dirIntf(container);

		BlockHandle root = dirIntf.GetRoot();

		EXPECT_NE(0, root);
		BlockHandle dirA = dirIntf.CreateDir(root, "A");
		EXPECT_NE(0, dirA);
		BlockHandle dirB = dirIntf.CreateDir(root, "B");
		EXPECT_NE(0, dirB);
	}

	{
		DirIntf dirIntf(container);

		BlockHandle root = dirIntf.GetRoot();

		EXPECT_NE(0, root);
		BlockHandle dirA = dirIntf.OpenDir(root, "A");
		EXPECT_NE(0, dirA);
		BlockHandle dirB = dirIntf.OpenDir(root, "B");
		EXPECT_NE(0, dirB);
	}
}

TEST_F(DirIntfTestsuite, DirEnum)
{
	DirIntf dirIntf(container);

	BlockHandle root = dirIntf.GetRoot();

	EXPECT_NE(0, root);
	BlockHandle dirA = dirIntf.CreateDir(root, "A");
	EXPECT_NE(0, dirA);
	BlockHandle dirB = dirIntf.CreateDir(root, "B");
	EXPECT_NE(0, dirB);

	std::vector<std::string> dirList;
	auto enumCallback = [&dirList](BlockHandle, BlockTypes::DirEntry*, const std::string& name)
	{
		dirList.push_back(name);
		return true;
	};

	dirIntf.EnumerateDir(root, enumCallback);

	EXPECT_EQ(2u, dirList.size());
	EXPECT_NE(dirList.end(), std::find(dirList.begin(), dirList.end(), "A"));
	EXPECT_NE(dirList.end(), std::find(dirList.begin(), dirList.end(), "B"));
}

TEST_F(DirIntfTestsuite, CreateNestedDirs)
{
	DirIntf dirIntf(container);

	BlockHandle root = dirIntf.GetRoot();

	EXPECT_NE(0, root);
	BlockHandle dirA = dirIntf.CreateDir(root, "A");
	EXPECT_NE(0, dirA);
	BlockHandle dirB = dirIntf.CreateDir(root, "B");
	EXPECT_NE(0, dirB);
	BlockHandle dirC = dirIntf.CreateDir(dirB, "C");

	EXPECT_FALSE(dirIntf.IsEmpty(root));
	EXPECT_TRUE(dirIntf.IsEmpty(dirA));
	EXPECT_FALSE(dirIntf.IsEmpty(dirB));
	EXPECT_TRUE(dirIntf.IsEmpty(dirC));
}

TEST_F(DirIntfTestsuite, DirDelete)
{
	DirIntf dirIntf(container);

	BlockHandle root = dirIntf.GetRoot();

	EXPECT_NE(0, root);
	BlockHandle dirA = dirIntf.CreateDir(root, "A");
	EXPECT_NE(0, dirA);
	BlockHandle dirB = dirIntf.CreateDir(root, "B");
	EXPECT_NE(0, dirB);
	BlockHandle dirC = dirIntf.CreateDir(dirB, "C");

	EXPECT_TRUE(dirIntf.DeleteDir(root, "A"));
	EXPECT_FALSE(dirIntf.DeleteDir(root, "B"));
	EXPECT_TRUE(dirIntf.DeleteDir(dirB, "C"));
	EXPECT_TRUE(dirIntf.DeleteDir(root, "B"));
	EXPECT_TRUE(dirIntf.IsEmpty(root));
}

TEST_F(DirIntfTestsuite, CreateFile)
{
	DirIntf dirIntf(container);

	BlockHandle root = dirIntf.GetRoot();

	BlockHandle dirA = dirIntf.CreateDir(root, "LevelFirst");
	EXPECT_NE(0, dirA);
	EXPECT_TRUE(dirIntf.IsEmpty(dirA));
	BlockHandle fileA = dirIntf.CreateFile(dirA, "LevelFirstFile");
	EXPECT_NE(0, fileA);
	EXPECT_FALSE(dirIntf.IsEmpty(dirA));
}

TEST_F(DirIntfTestsuite, FileAccess)
{
	DirIntf dirIntf(container);

	BlockHandle root = dirIntf.GetRoot();

	BlockHandle dirA = dirIntf.CreateDir(root, "LevelFirst");
	EXPECT_NE(0, dirA);
	EXPECT_TRUE(dirIntf.IsEmpty(dirA));
	BlockHandle fileA = dirIntf.CreateFile(dirA, "LevelFirstFile");
	EXPECT_NE(0, fileA);
	EXPECT_FALSE(dirIntf.IsEmpty(dirA));

	FileIntf fileIntf(container);

	const std::string buff = "Little brown fox";
	EXPECT_TRUE(fileIntf.Append(fileA, &buff[0], buff.size()));
	EXPECT_EQ(buff.size(), fileIntf.GetSize(fileA));

	std::string outBuff(buff.size(), ' ');

	EXPECT_TRUE(fileIntf.Read(fileA, &outBuff[0], outBuff.size(), 0));
	EXPECT_EQ(buff, outBuff);

	EXPECT_TRUE(dirIntf.DeleteFile(dirA, "LevelFirstFile"));
	EXPECT_TRUE(dirIntf.IsEmpty(dirA));

	EXPECT_FALSE(fileIntf.Read(fileA, &outBuff[0], outBuff.size(), 0));
}