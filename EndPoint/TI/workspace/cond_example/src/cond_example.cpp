#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <queue>

std::condition_variable common_cond_var;

int value1 = 0;
int value2 = 0;

std::queue <int> que1;
std::queue <int> que2;

std::mutex mutex1;
std::mutex mutex2;

void notifier_1() {
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
#if (1)
        std::cout << "1!";
        value1++;
        {
            std::lock_guard<std::mutex> lock(mutex1); /* Lock on queue */
            que1.emplace(value1);
        }
        common_cond_var.notify_one();
#endif
    }
}

void notifier_2() {
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
#if (1)
        std::cout << "2!";
        value2++;
        {
            std::lock_guard<std::mutex> lock(mutex2); /* Lock on queue */
            que2.emplace(value2);
        }
        common_cond_var.notify_one();
#endif
    }
}

void sleeper()
{
    std::mutex mtx;
    std::unique_lock<std::mutex> uni_lock(mtx); /* Local mutex to wait on */
    int val1 = 0;
    int val2 = 0;
    while(true)
    {
        if(common_cond_var.wait_for(
                            uni_lock,
                            std::chrono::milliseconds(4),
                            [&]{
                                  std::cout << '.';
                                  val1 = 0; val2 = 0;
                                  bool empty1 = true;
                                  bool empty2 = true;
                                  if (mutex1.try_lock())
                                  {
                                      empty1 = que1.empty();
                                      mutex1.unlock();
                                  }else { std::cout << "1l";}
                                  if (mutex2.try_lock())
                                  {
                                      empty2 = que2.empty();
                                      mutex2.unlock();
                                  } else { std::cout << "2l";}
                                  return ( ( !empty1 ) || ( !empty2 ) );
                               })!=true)
        {
          continue;
        }

        if( ( que1.empty() ) && ( que2.empty() ) )
        {
          std::cout << "!!!" << std::endl;
          continue;
        }

        {
            std::lock_guard<std::mutex> lock(mutex1); /* Lock on queue */
            if(!que1.empty()) { val1 = que1.front(); que1.pop(); }
        }
        if (val1) { std::cout << " 1:" << value1; }
        {
            std::lock_guard<std::mutex> lock(mutex2); /* Lock on queue */
            if (!que2.empty()) { val2 = que2.front(); que2.pop(); }
        }
        if (val2) { std::cout << " 2:" << value2; }
        std::cout << std::endl;
    } //end while
}


int main ()
{
  std::cout << "Start\n";
  std::thread n1th (notifier_1);
  std::thread n2th (notifier_2);
  std::thread sth  (sleeper);
  sth.join();
  return 0;
}
