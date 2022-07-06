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

#include "network_notification.h"

using namespace std;

class listener_handler1:
    public _unnamed::system::network::listener_handler,
    public _unnamed::system::network::listener_handler_notifier {
private:
    static const std::string _listener_handler_name;
public:
    listener_handler1() = default;

    ~listener_handler1() = default;

    virtual const std::string& get_listener_handler_name() const override {
        return _listener_handler_name;
    }

    virtual bool do_notify() override {
        cout << "listener_handler1::invoke?" <<
             (get_subject()) << ":" <<
             get_listener()->get_named_id() << ":" <<
             get_listener_handler()->get_listener_handler_name() << endl;
        return true;
    }
};
const std::string listener_handler1::_listener_handler_name{ "method1" };

class listener_handler2:
    public _unnamed::system::network::listener_handler,
    public _unnamed::system::network::listener_handler_notifier {
private:
    static const std::string _listener_handler_name;
public:
    listener_handler2() = default;

    ~listener_handler2() = default;

    virtual const std::string& get_listener_handler_name() const override {
        return _listener_handler_name;
    }

    virtual bool do_notify() override {
        cout << "listener_handler2::invoke?" <<
             (get_subject()) << ":" <<
             get_listener()->get_named_id() << ":" <<
             get_listener_handler()->get_listener_handler_name() << endl;
        return true;
    }
};
const std::string listener_handler2::_listener_handler_name{ "method2" };

int main(int argc, const char** argv) {
    auto subject_ = std::make_unique<_unnamed::system::network::subject>("localhost", "8000");
    auto listener1_ = std::make_shared<_unnamed::system::network::listener>("localhost:8001");
    auto listener2_ = std::make_shared<_unnamed::system::network::listener>("localhost:8002");
    auto listener3_ = std::make_shared<_unnamed::system::network::listener>("localhost:8003");
    auto listener4_ = std::make_shared<_unnamed::system::network::listener>("localhost:8004");

    subject_->subscriber(listener1_);
    subject_->unsubscriber(listener1_);
    subject_->subscriber(listener1_);
    subject_->subscriber(listener2_);
    subject_->subscriber(listener3_);
    subject_->subscriber(listener4_);

    const _unnamed::system::network::t::listener_table_m& listener_adresses_ = subject_->get_listeners();

    std::shared_ptr<listener_handler1> listener_handler1_ = std::make_shared<listener_handler1>();
    std::shared_ptr<listener_handler2> listener_handler2_ = std::make_shared<listener_handler2>();

    for (const auto& listener_pair_ : listener_adresses_) {
        const std::shared_ptr<_unnamed::system::network::listener>& listener = listener_pair_.second;

        listener->add_listener_handler(listener_handler1_);
        listener->add_listener_handler(listener_handler2_);
    }

    subject_->notify_all(listener_handler1_);
    subject_->notify_all(listener_handler2_);

    subject_->notify_all(listener_handler1_->get_listener_handler_name(),
                         [](_unnamed::system::network::subject const* subject,
                            _unnamed::system::network::listener const* listener,
    _unnamed::system::network::listener_handler const* listener_handler) {
        cout << "listener_handler1::invoke?" <<
             (subject) << ":" <<
             listener->get_named_id() << ":" <<
             listener_handler->get_listener_handler_name() << endl;
        return true;
    });
    subject_->notify_all(listener_handler2_->get_listener_handler_name(),
                         [](const _unnamed::system::network::subject* subject,
                            const _unnamed::system::network::listener* listener,
    const _unnamed::system::network::listener_handler* listener_handler) {
        cout << "listener_handler2::invoke?" <<
             (subject) << ":" <<
             listener->get_named_id() << ":" <<
             listener_handler->get_listener_handler_name() << endl;
        return false;
    });

    _unnamed::system::network::t::listener_list_i listeners_faileds_ = subject_->get_listeners_faileds();

    for (const auto& listener_failed_ : listeners_faileds_)	{
        cout << "listener_failed_: " << listener_failed_->get_named_id() << endl;
    }

    cout << "listener1: " << listener1_.use_count() << endl;
    cout << "listener2: " << listener2_.use_count() << endl;
    cout << "listener3: " << listener3_.use_count() << endl;
    cout << "listener4: " << listener4_.use_count() << endl;

    cout << "listener_handler1_: " << listener_handler1_.use_count() << endl;
    cout << "listener_handler2_: " << listener_handler2_.use_count() << endl;

    return 0;
}
