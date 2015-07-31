#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

#define PRODUCERS   3
#define CONSUMERS   2

/* template to store delivery package dimensions */
template <class T>
class package {
private:
    T length, width;
public:
    package(const int _h, const int _w) : length(_h), width(_w)
    {}
    /* check if dimensions are same or not */
    bool same_dimensions()
    {
        if (length == width)
            return true;
        return false;
    }
    /* print package details */
    void print_package()
    {
        cout << "Package : length ->\t" << length << ": width->\t" << width << endl;
    }
};

/* template for a thread safe queue */
template <class T>
class TSQueue {
private:
    queue<T> tsQueue;
    mutex mtx;
    condition_variable cv;
public:
    void push(T& entry)
    {
        unique_lock<mutex> uniqueLock(mtx);
        tsQueue.push(entry);
        uniqueLock.unlock();
        cv.notify_all();
    }
    T& pop()
    {
        unique_lock<mutex> uniqueLock(mtx);
        /* block thread if queue is empty */
        while (tsQueue.empty()) {
            cv.wait(uniqueLock);
        }
        auto& entry = tsQueue.front();
        tsQueue.pop();
        return entry;
    }
    int size() const
    {
        return tsQueue.size();
    }
};

inline void print_produced(int& item_num)
{
    cout << "Produced package number :\t" << item_num << endl;
}

inline void print_consumed(int& item_num)
{
    cout << "Consumed package number :\t" << item_num << endl;
}

template <typename T>
void producer(TSQueue<package<T>>& tsq, T length, T width, int num_items)
{
    while (num_items) {
        package<T> pkg(length, width);
        tsq.push(pkg);
        print_produced(num_items);
        --num_items;
    }
}

template <typename T>
void consumer(TSQueue<package<T>>& tsq,
              TSQueue<package<T>>& Box,
              TSQueue<package<T>>& Rectangle, int num_items)
{
    while (num_items) {
        auto& pkg = tsq.pop();
        print_consumed(num_items);
        pkg.print_package();
        /* separate out packages differentiated by
         * a box or rectangle
         */
        if (pkg.same_dimensions()) {
            Box.push(pkg);
        } else {
            Rectangle.push(pkg);
        }
        --num_items;
    }
}

int main() {
    TSQueue<package<int>> tsQ, Box, Rectangle;
    vector<thread> producers;
    vector<thread> consumers;
    int i, num_box, num_rectangle;

    for (i = 0; i < PRODUCERS; i++) {
        producers.push_back(thread(producer<int>, ref(tsQ), 5, (5+i), 10));
    }
    for (i = 0; i < CONSUMERS; i++) {
        consumers.push_back(thread(consumer<int>, ref(tsQ), ref(Box), ref(Rectangle), 15));
    }

    for (auto& x : producers) {
        x.join();
    }
    for (auto& x : consumers) {
        x.join();
    }

    num_box = Box.size();
    cout << endl << endl << "Box" << endl;
    cout << "Number of Boxes :\t" << num_box << endl;
    while (num_box) {
        auto& pkg = Box.pop();
        pkg.print_package();
        num_box--;
    }
    num_rectangle = Rectangle.size();
    cout << endl << endl << "Rectangle" << endl;
    cout << "Number of Rectangles :\t" << num_rectangle << endl;
    while (num_rectangle) {
        auto& pkg = Rectangle.pop();
        pkg.print_package();
        num_rectangle--;
    }

    return 0;
}