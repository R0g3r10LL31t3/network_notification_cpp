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

#include "network_announcer.h"

using namespace std;

class announcer_handler1:
    public _unnamed::system::network::announcer_handler,
    public _unnamed::system::network::announcer_handler_notifier {
private:
    static const std::string _named_id;
public:
    announcer_handler1() = default;

    ~announcer_handler1() = default;

    virtual const std::string& get_named_id() const override {
        return _named_id;
    }

    virtual bool do_notify() override {
        cout << "announcer_handler1::invoke?" <<
             get_announcer_controller()->get_named_id() << ":" <<
             get_announcer()->get_named_id() << ":" <<
             get_announcer_handler()->get_named_id() << endl;
        return true;
    }
};
const std::string announcer_handler1::_named_id{ "method1" };

class announcer_handler2:
    public _unnamed::system::network::announcer_handler,
    public _unnamed::system::network::announcer_handler_notifier {
private:
    static const std::string _named_id;
public:
    announcer_handler2() = default;

    ~announcer_handler2() = default;

    virtual const std::string& get_named_id() const override {
        return _named_id;
    }

    virtual bool do_notify() override {
        cout << "announcer_handler2::invoke?" <<
             get_announcer_controller()->get_named_id() << ":" <<
             get_announcer()->get_named_id() << ":" <<
             get_announcer_handler()->get_named_id() << endl;
        return true;
    }
};
const std::string announcer_handler2::_named_id{ "method2" };

int main(int argc, const char** argv) {
    auto announcer_controller_ = std::make_unique<_unnamed::system::network::announcer_controller>("announcer_controller[localhost]");
    auto announcer1_ = std::make_shared<_unnamed::system::network::announcer>("localhost:8001");
    auto announcer2_ = std::make_shared<_unnamed::system::network::announcer>("localhost:8002");
    auto announcer3_ = std::make_shared<_unnamed::system::network::announcer>("localhost:8003");
    auto announcer4_ = std::make_shared<_unnamed::system::network::announcer>("localhost:8004");

    announcer_controller_->add_announcer(announcer1_);
    announcer_controller_->rem_announcer(announcer1_);
    announcer_controller_->add_announcer(announcer1_);
    announcer_controller_->add_announcer(announcer2_);
    announcer_controller_->add_announcer(announcer3_);
    announcer_controller_->add_announcer(announcer4_);

    const _unnamed::system::network::t::announcer_table_m& announcer_adresses_ = announcer_controller_->get_announcers();

    std::shared_ptr<announcer_handler1> announcer_handler1_ = std::make_shared<announcer_handler1>();
    std::shared_ptr<announcer_handler2> announcer_handler2_ = std::make_shared<announcer_handler2>();

    for (const auto& announcer_pair_ : announcer_adresses_) {
        const std::shared_ptr<_unnamed::system::network::announcer>& announcer = announcer_pair_.second;

        announcer->add_announcer_handler(announcer_handler1_);
        announcer->add_announcer_handler(announcer_handler2_);
    }

    announcer_controller_->notify_all(announcer_handler1_);
    announcer_controller_->notify_all(announcer_handler2_);

    announcer_controller_->notify_all(announcer_handler1_->get_named_id(),
                                      [](_unnamed::system::network::announcer_controller const* announcer_controller,
                                         _unnamed::system::network::announcer const* announcer,
    _unnamed::system::network::announcer_handler const* announcer_handler) {
        cout << "announcer_handler1::invoke?" <<
             announcer_controller->get_named_id() << ":" <<
             announcer->get_named_id() << ":" <<
             announcer_handler->get_named_id() << endl;
        return true;
    });
    announcer_controller_->notify_all(announcer_handler2_->get_named_id(),
                                      [](const _unnamed::system::network::announcer_controller* announcer_controller,
                                         const _unnamed::system::network::announcer* announcer,
    const _unnamed::system::network::announcer_handler* announcer_handler) {
        cout << "announcer_handler2::invoke?" <<
             announcer_controller->get_named_id() << ":" <<
             announcer->get_named_id() << ":" <<
             announcer_handler->get_named_id() << endl;
        return false;
    });

    _unnamed::system::network::t::announcer_list_i announcers_faileds_ = announcer_controller_->get_announcers_faileds();

    for (const auto& announcer_failed_ : announcers_faileds_)	{
        cout << "announcer_failed_: " << announcer_failed_->get_named_id() << endl;
    }

    cout << "announcer1: " << announcer1_.use_count() << endl;
    cout << "announcer2: " << announcer2_.use_count() << endl;
    cout << "announcer3: " << announcer3_.use_count() << endl;
    cout << "announcer4: " << announcer4_.use_count() << endl;

    cout << "announcer_handler1_: " << announcer_handler1_.use_count() << endl;
    cout << "announcer_handler2_: " << announcer_handler2_.use_count() << endl;

    return 0;
}
