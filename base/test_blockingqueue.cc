#include <string.h>

// #include <boost/shared_ptr.hpp>
#include <memory>

#include "Mutex.h"
#include "BlockingQueue.h"

#include "thirdparty/gtest/gtest.h"

#define boost std
typedef struct {
  char video[88];
} Parcel;

class DataClass {
public:
  explicit DataClass() {
    printf("+ build DataClass()\n");
  }

  DataClass(const DataClass& data) {
    memcpy(video_, data.video(), sizeof(video_));
    printf("+ copy DataCLass()\n");
  }
  DataClass& operator=(const DataClass& data) {
    memcpy(video_, data.video(), sizeof(video_));
    printf("+ assgin DataCLass()\n");
    return *this;
  }
  ~DataClass() {
    printf("- destroy DataClass()\n");
  }
  void reset(char ch) {
    memset(video_, ch, sizeof(video_));
  }

  const char* video() const {
    return video_;
  }

private:
  char video_[88];
};

class BlockingQueueTest : public ::testing::Test {
protected:
  virtual void SetUp() {}
  virtual void TearDown() {}

  base::BlockingQueue<std::string> qString_;
  base::BlockingQueue<Parcel> qStruct_;
  base::BlockingQueue<DataClass> qClass_;
  base::BlockingQueue<boost::shared_ptr<DataClass>> qSharedPtr_;
};

TEST_F(BlockingQueueTest, takeString) {
  qString_.put("hello");
  qString_.put(" World\n");

  EXPECT_TRUE(2 == qString_.size());

  printf("take: %s\n", qString_.take().c_str());
  printf("take: %s\n", qString_.take().c_str());
}

TEST_F(BlockingQueueTest, takeStruct) {
  Parcel parcel;
  memset(static_cast<void*>(parcel.video), 'A', sizeof(parcel));
  qStruct_.put(parcel);
  memset(static_cast<void*>(parcel.video), 'B', sizeof(parcel));
  qStruct_.put(parcel);

  EXPECT_TRUE(2 == qStruct_.size());
  printf("take: %s\n", qStruct_.take().video);
  printf("take: %s\n", qStruct_.take().video);
  printf("size: %zu\n", qStruct_.size());
}


TEST_F(BlockingQueueTest, takeClass) {
  DataClass dataClass;
  dataClass.reset('x');
  qClass_.put(dataClass);
  dataClass.reset('y');
  qClass_.put(dataClass);

  EXPECT_TRUE(2 == qClass_.size());
  printf("take: %s\n", qClass_.take().video());
  printf("take: %s\n", qClass_.take().video());
  printf("size: %zu\n", qClass_.size());
}

TEST_F(BlockingQueueTest, takeClassPtr) {
  boost::shared_ptr<DataClass> dataPtr(new DataClass());
  dataPtr->reset('M');
  qSharedPtr_.put(dataPtr);

  boost::shared_ptr<DataClass> dataPtr2(new DataClass());
  dataPtr2->reset('N');
  qSharedPtr_.put(dataPtr2);
  EXPECT_TRUE(2 == qSharedPtr_.size());

  boost::shared_ptr<DataClass> dataPtr3(qSharedPtr_.take());
  printf("user_count: %lu\n", dataPtr3.use_count());
  printf("take: %s\n", dataPtr3->video());

  printf("take: %s\n", qSharedPtr_.take()->video());
  printf("size: %zu\n", qSharedPtr_.size());
}
