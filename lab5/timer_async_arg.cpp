#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

void Print(const boost::system::error_code& e,
            boost::asio::deadline_timer* t,
            int* count) {
    if (*count < 6) {
        std::cout << *count << std::endl;
        ++(*count);

        t->expires_at(t->expires_at() + boost::posix_time::seconds(1));
        t->async_wait(boost::bind(Print, boost::asio::placeholders::error, t, count));
    }
}

int main() {
    boost::asio::io_service io;

    int count = 0;
    boost::asio::deadline_timer t (io, boost::posix_time::seconds(1));
    t.async_wait(boost::bind(Print,
                boost::asio::placeholders::error, &t, &count));
    io.run();

    std::cout << "Final coutn is " << count << std::endl;

    return 0;
}
