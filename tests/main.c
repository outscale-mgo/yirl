#include <stdlib.h>
#include <glib.h>
#include "entity.h"
#include "tests.h"

int main(int argc, char **argv)
{
  g_test_init(&argc, &argv, NULL);
  g_test_add_func("/entity/lifecycle/simple", testLifecycleSimple);
  g_test_add_func("/entity/lifecycle/complex", testsLifecycleFlow);
  g_test_run();
}
