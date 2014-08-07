#include <stdio.h>
#include "RefBase.h"


class RefTest : public base::Ref {
  public:
    RefTest(int32_t id) : mID(id) {
      printf("RefTest ctor: %d\n", mID);
    }

    virtual ~RefTest() {
      printf("RefTest dtor: %d\n", mID);
    }

    int32_t id() const {
      return mID;
    }

  private:
    int32_t mID;
};

int main() {
  sp<RefTest> ref1 = new RefTest(1);

  {
    sp<RefTest> ref2 = new RefTest(2);
  }

  wp<RefTest> ref3 = ref1;
  sp<RefTest> ref4 = ref3.toStrongRef();
  if (ref4 == NULL) {
    printf("RefTest object is destroyed\n");
  } else {
    printf("RefTest object %d is still around\n",
        ref4->id());
  }
  ref4 = NULL;

  ref1 = NULL;
  ref4 = ref3.toStrongRef();
  if (ref4 == NULL) {
    printf("RefTest object is destroyed\n");
  } else {
    printf("RefTest object %d is still around\n",
        ref4->id());
  }

  return 0;
}
