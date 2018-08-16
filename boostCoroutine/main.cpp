/*
* 协程与协程之间存在调用与被调用的关系，主要还是pull_type和push_type的关系
* 这种不平等的关系保证了被调用者回馈调用者，完成返回上一级操作
* 定义中参数属于：
* 被调用者：void functionname(boost::coroutines2::coroutine<int>::pull_type &) 
* 调用者  ：boost::coroutines2::coroutine<int>::push_type source(函数名)
*/
/*
#include <iostream>
#include <boost/coroutine2/all.hpp>

constexpr int N = 10;

// 方法一：中规中矩
void foo(boost::coroutines2::coroutine<int>::pull_type & sink) {
    using coIter = boost::coroutines2::coroutine<int>::pull_type::iterator;
    for (coIter start = begin(sink); start != end(sink); ++start) {
        std::cout << "retrieve " << *start << "\n";
    }
}
// 方法二：auto自动推导
void foo2(boost::coroutines2::coroutine<int>::pull_type & sink) {
    for (auto val : sink) {
        std::cout << "retrieve " << val << "\n";
    }
}
// 方法三：守旧
void foo3(boost::coroutines2::coroutine<int>::pull_type & sink) {
    for (int i = 0; i < N; i++) {
        std::cout << "retrieve " << sink.get() << "\n";
        sink();
    }
}

int main() {
    boost::coroutines2::coroutine<int>::push_type source(foo);
    for (int i = 0; i < N; i++) {
        source(i);
    }
    return 0;
}
*/
/*
#include <boost/coroutine2/all.hpp>
#include <tuple>
#include <string>
#include <iostream>


void cooperative(boost::coroutines2::coroutine<std::tuple<int, std::string>>::pull_type &source)
{
    auto args = source.get();
    std::cout << std::get<0>(args) << " " << std::get<1>(args) << '\n';
    source();
    args = source.get();
    std::cout << std::get<0>(args) << " " << std::get<1>(args) << '\n';
}

int main()
{
    boost::coroutines2::coroutine<std::tuple<int, std::string>>::push_type sink{ cooperative };
    sink(std::make_tuple(0, "aaa"));
    sink(std::make_tuple(1, "bbb"));
    std::cout << "end\n";
}
*/
#include<boost/thread/thread.hpp>
#include<boost/coroutine2/all.hpp>
#include<iostream>
#include<thread>
typedef boost::coroutines2::coroutine<int> boost_coroutine;

void function() {
    std::cout << "in function" << std::endl;
}
int main() 
{
    /*thread *t = new thread(function);
    thread::id id= t->get_id();
    
    std::cout << id << std::endl;*/
}