#include "../include/lsm/engine.h"
#include <iostream>
#include <string>

int main() {
  LSM lsm("/tmp/lsm");
  // change the data manually
  lsm.put("key02", "value02");
  lsm.flush_all();
}