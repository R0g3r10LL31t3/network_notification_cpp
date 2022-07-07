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

#pragma once

#include <string>
#include <list>
#include <map>
#include <memory>

namespace _unnamed {

namespace system {

namespace network {

class subscriber_handler_receiver;
class subscriber_handler;
class subscriber;
class subscriber_controller;

namespace t {
/**
 * Mutable and imutable smart shared pointers to objects
 * Mutable objects can be modifiyed
 * Imutable objects can not be modifiyed, but const methods can be invoked
 */
using subscriber_handler_receiver_i = std::shared_ptr<const subscriber_handler_receiver>;
using subscriber_handler_receiver_m = std::shared_ptr<subscriber_handler_receiver>;

using subscriber_handler_i = std::shared_ptr<const subscriber_handler>;
using subscriber_handler_m = std::shared_ptr<subscriber_handler>;

using subscriber_i = std::shared_ptr<const subscriber>;
using subscriber_m = std::shared_ptr<subscriber>;

using subscriber_controller_i = std::shared_ptr<const subscriber_controller>;
using subscriber_controller_m = std::shared_ptr<subscriber_controller>;

/**
 * Constant mutable and imutable pointers to objects
 * Mutable objects can be modifiyed
 * Imutable objects can not be modifiyed, but const methods can be invoked
 */
using subscriber_handler_iptr = subscriber_handler* const;
using subscriber_handler_mptr = subscriber_handler*;

using subscriber_iptr = subscriber* const;
using subscriber_mptr = subscriber*;

using subscriber_controller_iptr = subscriber_controller* const;
using subscriber_controller_mptr = subscriber_controller*;

/**
 * Data Structure
 */
using subscriber_handlers_table_i = std::map<std::string, t::subscriber_handler_i>;
using subscriber_handlers_table_m = std::map<std::string, const t::subscriber_handler_m>;

using subscriber_table_i = std::map<std::string, t::subscriber_i>;
using subscriber_table_m = std::map<std::string, const t::subscriber_m>;

using subscriber_list_i = std::list<t::subscriber_i>;
using subscriber_list_m = std::list<const t::subscriber_m>;
} //t

class subscriber_handler_receiver {
protected:
    t::subscriber_handler_mptr _subscriber_handler { nullptr };
    t::subscriber_mptr _subscriber { nullptr };
    t::subscriber_controller_mptr _subscriber_controller { nullptr };

    subscriber_handler_receiver() = default;
public:
    ~subscriber_handler_receiver() = default;

    const std::string& get_subscriber_handler_topic_id() const;

    void set_subscriber_handler(t::subscriber_handler_mptr subscriber_handler);

    t::subscriber_handler_iptr get_subscriber_handler() const;

    void set_subscriber(t::subscriber_mptr subscriber);

    t::subscriber_iptr get_subscriber() const;

    void set_subscriber_controller(t::subscriber_controller_mptr subscriber_controller);

    t::subscriber_controller_iptr get_subscriber_controller() const;

    virtual bool do_observes() = 0;
};

class subscriber_handler {
private:
    std::string _topic_id;
public:
    explicit subscriber_handler(const std::string& topic_id);

    ~subscriber_handler() = default;

    const std::string& get_topic_id() const;

    template <typename lambda_function>
    bool observes(const lambda_function& function) {
        return function(this);
    }

    bool observes(const t::subscriber_handler_receiver_m& subscriber_handler_receiver);
};

class subscriber {
private:
    std::string _named_id;
    t::subscriber_handlers_table_m _subscriber_handlers_table;
public:
    explicit subscriber(const std::string& named_id);

    ~subscriber() = default;

    const std::string& get_named_id() const;

    const t::subscriber_handlers_table_m& get_subscriber_handlers_table() const;

    void add_subscriber_handler(const t::subscriber_handler_m& subscriber_handler);

    void rem_subscriber_handler(const t::subscriber_handler_m& subscriber_handler);

    template <typename lambda_function>
    bool observes(const std::string& subscriber_handler_topic_id, const lambda_function& function) {
        bool notified_ = false;

        auto subscriber_handler_it_ = this->_subscriber_handlers_table.find(subscriber_handler_topic_id);
        if (subscriber_handler_it_ != this->_subscriber_handlers_table.end()) {
            const t::subscriber_handler_m& subscriber_handler_ = (*subscriber_handler_it_).second;
            auto newFunction_ = [this, &function](t::subscriber_handler_mptr subscriber_handler) {
                return function(this, subscriber_handler);
            };
            notified_ = subscriber_handler_->observes(newFunction_);
        }

        return notified_;
    }

    bool observes(const t::subscriber_handler_receiver_m& subscriber_handler_receiver);
};

class subscriber_controller {
private:
    std::string _named_id;
    t::subscriber_table_m _subscribers;
    t::subscriber_list_i _subscribers_faileds;
protected:
    void clear_subscribers_faileds();
    void add_subscriber_failed(const t::subscriber_m& subscriber_failed);
public:
    explicit subscriber_controller(const std::string& named_id);

    ~subscriber_controller() = default;

    const std::string& get_named_id() const;

    const t::subscriber_list_i& get_subscribers_faileds() const;

    virtual void add_subscriber(const t::subscriber_m& subscriber);

    virtual void rem_subscriber(const t::subscriber_m& subscriber);

    const t::subscriber_table_m& get_subscribers() const;

    template <typename lambda_function>
    void observes_all(const std::string& subscriber_handler_topic_id, const lambda_function& function) {

        this->clear_subscribers_faileds();

        for (const auto& subscriber_pair_ : this->_subscribers) {
            const t::subscriber_m& subscriber_ = subscriber_pair_.second;
            auto newFunction_ = [this, &function](
            t::subscriber_mptr subscriber, t::subscriber_handler_mptr subscriber_handler) {
                return function(this, subscriber, subscriber_handler);
            };
            if (!subscriber_->observes(subscriber_handler_topic_id, newFunction_)) {
                add_subscriber_failed(subscriber_);
            }
        }
    }

    void observes_all(const t::subscriber_handler_receiver_m& subscriber_handler_receiver);
};

} //network
} //system
} //_unnamed
