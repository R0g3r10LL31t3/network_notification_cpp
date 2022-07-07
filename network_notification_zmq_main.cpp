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
#include <thread>

#include <zmq.hpp>

#include "network_publisher.h"
#include "network_publisher_zmq.h"

using namespace std;

class publisher_handler1 : public _unnamed::system::network::publisher_handler,
    public _unnamed::system::network::zmq_publisher_handler_sender {
private:
    static const std::string _named_id;
public:
    publisher_handler1()
        : _unnamed::system::network::publisher_handler(publisher_handler1::_named_id) {
        this->set_publisher_handler(this);
    }

    ~publisher_handler1() = default;

    virtual bool do_notify() override {
        cout << _named_id <<
             "::invoke? publisher_controller:[" << get_publisher_controller()->get_named_id() <<
             "]:publisher[" << get_publisher()->get_named_id() <<
             "]:address[" <<	get_publisher()->get_address() <<
             "]:port[" << get_publisher()->get_port() <<
             "]:publisher_handler[" << get_publisher_handler()->get_topic_id() << "]." << endl;

        using namespace _unnamed::system::network;

        std::string topic{ "topic:angry" };
        std::string data{ "Dammit, I am {" + get_publisher()->get_named_id() + ":" + get_publisher()->get_port() + "}" };

        get_publisher()->send(t::const_buffer(topic.c_str(), topic.size()), t::const_buffer(data.c_str(), data.size()));

        return true;
    }

    _unnamed::system::network::zmq_publisher* get_publisher() {
        return static_cast<_unnamed::system::network::zmq_publisher*>(this->_publisher);
    }

};
const std::string publisher_handler1::_named_id{ "zmq_publish_handler1" };

class publisher_handler2
    : public _unnamed::system::network::publisher_handler,
      public _unnamed::system::network::zmq_publisher_handler_sender {
private:
    static const std::string _named_id;
public:
    publisher_handler2()
        : _unnamed::system::network::publisher_handler(publisher_handler2::_named_id) {
        this->set_publisher_handler(this);
    }

    ~publisher_handler2() = default;

    virtual bool do_notify() override {
        cout << _named_id <<
             "::invoke? publisher_controller:[" << get_publisher_controller()->get_named_id() <<
             "]:publisher[" << get_publisher()->get_named_id() <<
             "]:address[" << get_publisher()->get_address() <<
             "]:port[" << get_publisher()->get_port() <<
             "]:publisher_handler[" << get_publisher_handler()->get_topic_id() << "]." << endl;

        using namespace _unnamed::system::network;

        std::string topic{ "topic:happy" };
        std::string data{ "Hello, I am {" + get_publisher()->get_named_id() + ":" + get_publisher()->get_port() + "}"};

        get_publisher()->send(t::const_buffer(topic.c_str(), topic.size()), t::const_buffer(data.c_str(), data.size()));

        return true;
    }

    _unnamed::system::network::zmq_publisher * get_publisher() {
        return static_cast<_unnamed::system::network::zmq_publisher*>(this->_publisher);
    }
};
const std::string publisher_handler2::_named_id{ "zmq_publish_handler2" };

void subscriber_task_ok(string phrass, list<string> connect_list, list<string> subscribe_topics) {
    zmq::context_t context(4);
    zmq::socket_t subscriber(context, zmq::socket_type::sub);

    for (string topic : subscribe_topics) {
        subscriber.set(zmq::sockopt::subscribe, topic);
        cout << phrass << "subscribe: topic[" << topic << "]" << endl;
    }

    subscriber.set(zmq::sockopt::linger, 0);
    subscriber.set(zmq::sockopt::rcvhwm, 1000);
    subscriber.set(zmq::sockopt::ipv6, 0);

    try {
        for (string connect : connect_list) {
            subscriber.connect(connect);
            cout << phrass << "connected " << connect << endl;
        }
    } catch (zmq::error_t& ex) {
        cout << phrass << ex.num() << ex.what() << endl;
    }

    int repeat_failed = 0;

    while (1) {

        zmq::message_t topic{};
        zmq::message_t data{};

        zmq::recv_result_t rt = subscriber.recv(topic, zmq::recv_flags::dontwait);
        zmq::recv_result_t rd = subscriber.recv(data, zmq::recv_flags::dontwait);

        if (rt.has_value() || rd.has_value()) {
        }

        if (data.size() == 0 && topic.size() == 0) {
            repeat_failed++;
        } else {
            cout << phrass << "topic: [" << topic.to_string_view() << "], ";
            cout << "data: [" << data.to_string_view() << "]." << endl;
        }


        std::this_thread::sleep_for(std::chrono::milliseconds(250));

        if (data.to_string() == "close" || repeat_failed > 100) {
            string ddata(static_cast<const char*>(data.data()), data.size());
            cout << phrass << "command [" << ddata << "]" << endl;
            break;
        }

        if (data.size() == 0 && topic.size() == 0) {
            repeat_failed++;
        }

        if (!subscriber.connected()) {
            cout << phrass << "disconnected." << endl;
            break;
        }
    }

    cout << phrass << "finished" << endl;

    try {
        subscriber.close();
        context.shutdown();
        context.close();
    } catch (zmq::error_t& ex) {
        cout << phrass << ex.num() << ex.what() << endl;
    } catch (std::exception& ex) {
        cout << phrass << ex.what() << endl;
    }
}

void subscriber_task(string phrass, list<string> connect_list, list<string> subscribe_topics) {
    zmq::context_t context(1);
    zmq::socket_t subscriber(context, zmq::socket_type::sub);

    //std::string topic{ topicListen };
    //subscriber.set(zmq::sockopt::subscribe, topic);
    //subscriber.set(zmq::sockopt::subscribe, "");

    for (string topic : subscribe_topics) {
        subscriber.set(zmq::sockopt::subscribe, topic);
        cout << phrass << "subscribe: topic[" << topic << "]" << endl;
    }

    //subscriber.set(zmq::sockopt::invert_matching, 1);
    subscriber.set(zmq::sockopt::linger, 0);
    subscriber.set(zmq::sockopt::rcvhwm, 1000);
    //subscriber.set(zmq::sockopt::tcp_keepalive, 1);
    subscriber.set(zmq::sockopt::ipv6, 0);

    try {
        //cout << phrass << "try connect " << connect << endl;
        //subscriber.connect(connect);
        for (string connect : connect_list)	{
            subscriber.connect(connect);
            cout << phrass << "connected " << connect << endl;
        }
    } catch (zmq::error_t& ex) {
        cout << phrass << ex.num() << ex.what() << endl;
    }

    int repeat_failed = 0;

    while (1) {

        //zmq::message_t topic{};
        //zmq::message_t data{};
        char _tpc[2048];
        char _dat[2048];

        zmq::mutable_buffer topic = zmq::buffer(_tpc, 1024); //{ _tpc, 1024 };
        zmq::mutable_buffer data = zmq::buffer(_dat, 1024);//{ _dat, 1024 };

        //cout << phrass << "trying... " << endl;

        zmq::recv_buffer_result_t rt = subscriber.recv(topic, zmq::recv_flags::dontwait);
        zmq::recv_buffer_result_t rd = subscriber.recv(data, zmq::recv_flags::dontwait);
        //zmq::recv_result_t rt = subscriber.recv(topic, zmq::recv_flags::dontwait);
        //zmq::recv_result_t rd = subscriber.recv(data, zmq::recv_flags::dontwait);

        if ((data.size() == 0 && topic.size() == 0)
                || (!rt.has_value() || !rd.has_value())) {
            //cout << phrass << "without topics." << endl;
            repeat_failed++;
        } else {
            string tdata(static_cast<const char*>(topic.data()), rt.value().size);
            string ddata(static_cast<const char*>(data.data()), rd.value().size);
            cout << phrass << "topic: [" << tdata << "], ";
            cout << "data: [" << ddata << "]." << endl;
            //cout << phrass << "topic: [" << topic.to_string_view() << "], "; //topic.to_string_view();
            //cout << "data: [" << data.to_string_view() << "]." << endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(250));

        //if (data.to_string() == "close" || repeat_failed > 100) {
        if (rd.has_value() && string(static_cast<const char*>(data.data()), rd.value().size) == "close" || repeat_failed > 100) {
            //cout << phrass << "command [" << data.to_string_view() << "]" << endl;
            string ddata(static_cast<const char*>(data.data()), rd.value().size);
            cout << phrass << "command [" << ddata << "]" << endl;
            break;
        }

        if (data.size() == 0 && topic.size() == 0) {
            //cout << phrass << "without topics." << endl;
            repeat_failed++;
        }

        if (!subscriber.connected()) {
            cout << phrass << "disconnected." << endl;
            break;
        }
    }

    cout << phrass << "finished" << endl;

    try {
        subscriber.close();
        context.shutdown();
        context.close();
    } catch (zmq::error_t& ex) {
        cout << phrass << ex.num() << ex.what() << endl;
    } catch (std::exception& ex) {
        cout << phrass << ex.what() << endl;
    }
}

int main(int argc, const char** argv) {
    auto publisher_controller_ = std::make_unique<_unnamed::system::network::publisher_controller>("publisher_controller[localhost]");
    auto publisher1_ = std::make_shared<_unnamed::system::network::zmq_publisher>("publisher[1]", "*", "8001");
    auto publisher2_ = std::make_shared<_unnamed::system::network::zmq_publisher>("publisher[2]", "*", "8002");
    auto publisher3_ = std::make_shared<_unnamed::system::network::zmq_publisher>("publisher[3]", "*", "8003");
    auto publisher4_ = std::make_shared<_unnamed::system::network::zmq_publisher>("publisher[4]", "*", "8004");

    auto context = std::make_shared<_unnamed::system::network::t::context>(1);

    try {
        publisher1_->close();
        publisher2_->close();
        publisher3_->close();
        publisher4_->close();
    } catch (std::exception& ex) {
        cout << "Reason >>\n" << ex.what() << endl;
    }

    publisher1_->bind(context);
    publisher2_->bind(context);
    publisher3_->bind(context);
    publisher4_->bind(context);

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

    //auto subscriber_thread_all = std::make_unique<thread>(subscriber_task,
    //	"tcp://localhost:8001;localhost:8002;localhost:8003;localhost:8004",
    //	"subscript[all]: ",
    //	list<string>{ "" });
    //std::this_thread::sleep_for(std::chrono::milliseconds(250));

    auto subscriber_thread_all = std::make_unique<thread>(subscriber_task,
                                 "subscript[all]: ",
                                 list<string> { "tcp://localhost:8001", "tcp://localhost:8002", "tcp://localhost:8003", "tcp://localhost:8004" },
                                 //list<string>{ "tcp://localhost:8002;localhost:8001;localhost:8003;localhost:8004" },
                                 list<string> { "" });
    std::this_thread::sleep_for(std::chrono::milliseconds(250));

    auto subscriber_thread1 = std::make_unique<thread>(subscriber_task,
                              "subscript[1]: ",
                              list<string> { "tcp://localhost:8001" },
                              list<string> { "topic:happy", "topic:command" });
    std::this_thread::sleep_for(std::chrono::milliseconds(250));

    auto subscriber_thread2 = std::make_unique<thread>(subscriber_task,
                              "subscript[2]: ",
                              list<string> { "tcp://localhost:8002" },
                              list<string> { "topic:angry", "topic:command" });
    std::this_thread::sleep_for(std::chrono::milliseconds(250));

    auto subscriber_thread3 = std::make_unique<thread>(subscriber_task,
                              "subscript[3]: ",
                              list<string> { "tcp://localhost:8003" },
                              list<string> { "topic:happy", "topic:angry", "topic:command" });
    std::this_thread::sleep_for(std::chrono::milliseconds(250));

    auto subscriber_thread4 = std::make_unique<thread>(subscriber_task,
                              "subscript[4]: ",
                              list<string> { "tcp://localhost:8004" },
                              list<string> { "topic:angry", "topic:happy", "topic:command" });
    std::this_thread::sleep_for(std::chrono::milliseconds(250));

    std::this_thread::sleep_for(std::chrono::seconds(5));

    publisher_controller_->notify_all(publisher_handler1_);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    publisher_controller_->notify_all(publisher_handler2_);

    publisher_controller_->notify_all(publisher_handler1_->get_topic_id(),
                                      [](_unnamed::system::network::publisher_controller* publisher_controller,
                                         _unnamed::system::network::publisher* publisher,
    _unnamed::system::network::publisher_handler* publisher_handler) {

        _unnamed::system::network::t::zmq_publisher_mptr publisher_ =
            reinterpret_cast<_unnamed::system::network::t::zmq_publisher_mptr>(publisher);

        cout << "lambda1" << "::do_notify()? " <<
             "publisher_controller: [" << publisher_controller->get_named_id() <<
             "]:publisher[" << publisher_->get_named_id() <<
             "]:address[" << publisher_->get_address() <<
             "]:port[" << publisher_->get_port() <<
             "]:publisher_handler[" << publisher_handler->get_topic_id() << "]." << endl;

        return true;
    });

    publisher_controller_->notify_all(publisher_handler2_->get_topic_id(),
                                      [](_unnamed::system::network::publisher_controller* publisher_controller,
                                         _unnamed::system::network::publisher* publisher,
    _unnamed::system::network::publisher_handler* publisher_handler) {

        _unnamed::system::network::t::zmq_publisher_mptr publisher_ =
            reinterpret_cast<_unnamed::system::network::t::zmq_publisher_mptr>(publisher);

        cout << "lambda2" << "::do_notify()? " <<
             "publisher_controller: [" << publisher_controller->get_named_id() <<
             "]:publisher[" << publisher_->get_named_id() <<
             "]:address[" << publisher_->get_address() <<
             "]:port[" << publisher_->get_port() <<
             "]:publisher_handler[" << publisher_handler->get_topic_id() << "]." << endl;

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

    //publisher1_->unbind("tcp://127.0.0.1:" + publisher1_->get_port());
    //publisher2_->unbind("tcp://127.0.0.1:" + publisher2_->get_port());
    //publisher3_->unbind("tcp://127.0.0.1:" + publisher3_->get_port());
    //publisher4_->unbind("tcp://127.0.0.1:" + publisher4_->get_port());

    std::string topic{ "topic:command" };
    std::string data{ "close" };

    using namespace _unnamed::system::network;
    cout << "sending close..." << endl;
    publisher1_->send(t::const_buffer(topic.c_str(), topic.size()), t::const_buffer(data.c_str(), data.size()));
    publisher2_->send(t::const_buffer(topic.c_str(), topic.size()), t::const_buffer(data.c_str(), data.size()));
    publisher3_->send(t::const_buffer(topic.c_str(), topic.size()), t::const_buffer(data.c_str(), data.size()));
    publisher4_->send(t::const_buffer(topic.c_str(), topic.size()), t::const_buffer(data.c_str(), data.size()));

    std::this_thread::sleep_for(std::chrono::seconds(5));

    publisher1_->close();
    publisher2_->close();
    publisher3_->close();
    publisher4_->close();

    cout << "closed publishers" << endl;

    subscriber_thread1->join();
    subscriber_thread2->join();
    subscriber_thread3->join();
    subscriber_thread4->join();
    subscriber_thread_all->join();
    //thread::native_handle_type t = subscriber_thread_all->native_handle();

    cout << "joined subscribers" << endl;

    try {
        context->shutdown();
        context->close();
    } catch (zmq::error_t& ex) {
        cout << ex.num() << ex.what() << endl;
    } catch (std::exception& ex) {
        cout << ex.what() << endl;
    }

    return 0;
}
