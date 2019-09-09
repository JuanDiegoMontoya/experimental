#include <iostream>
#include <thread>

using namespace std;

void pi()
{
  double n = 1000000000000000000000000000.0;
  double sum = 0.0;
  int sign = 1;
  for (int i = 0; i < n; ++i)
  {           
    sum += sign/(2.0*i+1.0);
    sign *= -1;
  }
  
  cout << 4.0 * sum << endl;
}

int main()
{
  thread t1(pi);
  thread t2(pi);
  thread t3(pi);
  thread t4(pi);
  thread t5(pi);
  thread t6(pi);
  thread t7(pi);
  thread t8(pi);
  
  t1.join();
  t2.join();
  t3.join();
  t4.join();
  t5.join();
  t6.join();
  t7.join();
  t8.join();
  
  return 0;
}