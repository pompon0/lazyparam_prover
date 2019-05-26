#define DEBUG if(1)
#include "gtest/gtest.h"
#include "mgu.h"
#include "util/log.h"

TEST(MGU,loop) {
  Valuation V;
  auto var0 = Term(V.alloc_var());
  auto var1 = Term(V.alloc_var());
  ASSERT_TRUE(V.mgu(var1,var0));
  ASSERT_TRUE(V.mgu(var0,var1));
  ASSERT_TRUE(!V.val[0]);
  ASSERT_EQ(V.val[1].get(),var0);
}

