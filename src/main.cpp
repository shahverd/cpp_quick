#include <iostream>
#include <vector>

void bubbleSort(std::vector<int> &v) {
  int len = v.size();
  for (int i = 0; i < len; i++) {
    for (int j = 0; j < len - 1; j++) {
      if (v.at(j) > v.at(j + 1)) {
        std::swap(v[j], v[j + 1]);
      }
    }
  }
}

using namespace std;

#ifndef UNIT_TEST
int main() {
  std::vector<int> myVector = {64, 34, 25, 12, 22, 11, 90};

  std::cout << "Original array: ";
  for (int x : myVector) {
    std::cout << x << " ";
  }
  std::cout << std::endl;

  bubbleSort(myVector);

  std::cout << "Sorted array: ";
  for (int x : myVector) {
    std::cout << x << " ";
  }
  std::cout << std::endl;

  return 0;
}
#endif
