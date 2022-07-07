/**
* MIT License
*
* Copyright(c) 2022 Rogério Lecarião Leite, Brazil.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this softwareand associated documentation files(the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions :
*
* The above copyright noticeand this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

#if defined(WIN32) || defined(WIN64)
#include <windows.h>
#endif /* #if defined(WIN32) || defined(WIN64) */

#include <string>
#include <iostream>
#include <chrono>

#include "network_publisher.h"

using namespace std;

class publisher_handler1:
    public _unnamed::system::network::publisher_handler,
    public _unnamed::system::network::publisher_handler_sender {
private:
    static const std::string _named_id;
public:
    publisher_handler1() = default;

    ~publisher_handler1() = default;

    virtual const std::string& get_named_id() const override {
        return _named_id;
    }

    virtual bool do_publish() override {
        cout << "publisher_handler1::invoke?" <<
             get_publisher_controller()->get_named_id() << ":" <<
             get_publisher()->get_named_id() << ":" <<
             get_publisher_handler()->get_named_id() << endl;
        return true;
    }
};
const std::string publisher_handler1::_named_id{ "method1" };

class publisher_handler2:
    public _unnamed::system::network::publisher_handler,
    public _unnamed::system::network::publisher_handler_sender {
private:
    static const std::string _named_id;
public:
    publisher_handler2() = default;

    ~publisher_handler2() = default;

    virtual const std::string& get_named_id() const override {
        return _named_id;
    }

    virtual bool do_publish() override {
        cout << "publisher_handler2::invoke?" <<
             get_publisher_controller()->get_named_id() << ":" <<
             get_publisher()->get_named_id() << ":" <<
             get_publisher_handler()->get_named_id() << endl;
        return true;
    }
};
const std::string publisher_handler2::_named_id{ "method2" };

int main(int argc, const char** argv) {
    auto publisher_controller_ = std::make_unique<_unnamed::system::network::publisher_controller>("publisher_controller[localhost]");
    auto publisher1_ = std::make_shared<_unnamed::system::network::publisher>("localhost:8001");
    auto publisher2_ = std::make_shared<_unnamed::system::network::publisher>("localhost:8002");
    auto publisher3_ = std::make_shared<_unnamed::system::network::publisher>("localhost:8003");
    auto publisher4_ = std::make_shared<_unnamed::system::network::publisher>("localhost:8004");

    publisher_controller_->add_publisher(publisher1_);
    publisher_controller_->rem_publisher(publisher1_);
    publisher_controller_->add_publisher(publisher1_);
    publisher_controller_->add_publisher(publisher2_);
    publisher_controller_->add_publisher(publisher3_);
    publisher_controller_->add_publisher(publisher4_);

    const _unnamed::system::network::t::publisher_table_m& publisher_adresses_ = publisher_controller_->get_publishers();

    std::shared_ptr<publisher_handler1> publisher_handler1_ = std::make_shared<publisher_handler1>();
    std::shared_ptr<publisher_handler2> publisher_handler2_ = std::make_shared<publisher_handler2>();

    for (const auto& publisher_pair_ : publisher_adresses_) {
        const std::shared_ptr<_unnamed::system::network::publisher>& publisher = publisher_pair_.second;

        publisher->add_publisher_handler(publisher_handler1_);
        publisher->add_publisher_handler(publisher_handler2_);
    }

    publisher_controller_->publish_all(publisher_handler1_);
    publisher_controller_->publish_all(publisher_handler2_);

    publisher_controller_->publish_all(publisher_handler1_->get_named_id(),
                                       [](_unnamed::system::network::publisher_controller const* publisher_controller,
                                          _unnamed::system::network::publisher const* publisher,
    _unnamed::system::network::publisher_handler const* publisher_handler) {
        cout << "publisher_handler1::invoke?" <<
             publisher_controller->get_named_id() << ":" <<
             publisher->get_named_id() << ":" <<
             publisher_handler->get_named_id() << endl;
        return true;
    });
    publisher_controller_->publish_all(publisher_handler2_->get_named_id(),
                                       [](const _unnamed::system::network::publisher_controller* publisher_controller,
                                          const _unnamed::system::network::publisher* publisher,
    const _unnamed::system::network::publisher_handler* publisher_handler) {
        cout << "publisher_handler2::invoke?" <<
             publisher_controller->get_named_id() << ":" <<
             publisher->get_named_id() << ":" <<
             publisher_handler->get_named_id() << endl;
        return false;
    });

    _unnamed::system::network::t::publisher_list_i publishers_faileds_ = publisher_controller_->get_publishers_faileds();

    for (const auto& publisher_failed_ : publishers_faileds_)	{
        cout << "publisher_failed_: " << publisher_failed_->get_named_id() << endl;
    }

    cout << "publisher1: " << publisher1_.use_count() << endl;
    cout << "publisher2: " << publisher2_.use_count() << endl;
    cout << "publisher3: " << publisher3_.use_count() << endl;
    cout << "publisher4: " << publisher4_.use_count() << endl;

    cout << "publisher_handler1_: " << publisher_handler1_.use_count() << endl;
    cout << "publisher_handler2_: " << publisher_handler2_.use_count() << endl;

    return 0;
}
