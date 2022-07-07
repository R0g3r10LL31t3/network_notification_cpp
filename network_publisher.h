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

class publisher_handler_sender;
class publisher_handler;
class publisher;
class publisher_controller;

namespace t {
/**
 * Mutable and imutable smart shared pointers to objects
 * Mutable objects can be modifiyed
 * Imutable objects can not be modifiyed, but const methods can be invoked
 */
using publisher_handler_sender_i = std::shared_ptr<const publisher_handler_sender>;
using publisher_handler_sender_m = std::shared_ptr<publisher_handler_sender>;

using publisher_handler_i = std::shared_ptr<const publisher_handler>;
using publisher_handler_m = std::shared_ptr<publisher_handler>;

using publisher_i = std::shared_ptr<const publisher>;
using publisher_m = std::shared_ptr<publisher>;

using publisher_controller_i = std::shared_ptr<const publisher_controller>;
using publisher_controller_m = std::shared_ptr<publisher_controller>;

/**
 * Constant mutable and imutable pointers to objects
 * Mutable objects can be modifiyed
 * Imutable objects can not be modifiyed, but const methods can be invoked
 */
using publisher_handler_iptr = publisher_handler* const;
using publisher_handler_mptr = publisher_handler*;

using publisher_iptr = publisher* const;
using publisher_mptr = publisher*;

using publisher_controller_iptr = publisher_controller* const;
using publisher_controller_mptr = publisher_controller*;

/**
 * Data Structure
 */
using publisher_handlers_table_i = std::map<std::string, t::publisher_handler_i>;
using publisher_handlers_table_m = std::map<std::string, const t::publisher_handler_m>;

using publisher_table_i = std::map<std::string, t::publisher_i>;
using publisher_table_m = std::map<std::string, const t::publisher_m>;

using publisher_list_i = std::list<t::publisher_i>;
using publisher_list_m = std::list<const t::publisher_m>;
} //t

class publisher_handler_sender {
protected:
    t::publisher_handler_mptr _publisher_handler { nullptr };
    t::publisher_mptr _publisher { nullptr };
    t::publisher_controller_mptr _publisher_controller { nullptr };

    publisher_handler_sender() = default;
public:
    ~publisher_handler_sender() = default;

    const std::string& get_publisher_handler_topic_id() const;

    void set_publisher_handler(t::publisher_handler_mptr publisher_handler);

    t::publisher_handler_iptr get_publisher_handler() const;

    void set_publisher(t::publisher_mptr publisher);

    t::publisher_iptr get_publisher() const;

    void set_publisher_controller(t::publisher_controller_mptr publisher_controller);

    t::publisher_controller_iptr get_publisher_controller() const;

    virtual bool do_notify() = 0;
};

class publisher_handler {
private:
    std::string _topic_id;
public:
    explicit publisher_handler(const std::string& topic_id);

    ~publisher_handler() = default;

    const std::string& get_topic_id() const;

    template <typename lambda_function>
    bool notify(const lambda_function& function) {
        return function(this);
    }

    bool notify(const t::publisher_handler_sender_m& publisher_handler_sender);
};

class publisher {
private:
    std::string _named_id;
    t::publisher_handlers_table_m _publisher_handlers_table;
public:
    explicit publisher(const std::string& named_id);

    ~publisher() = default;

    const std::string& get_named_id() const;

    const t::publisher_handlers_table_m& get_publisher_handlers_table() const;

    void add_publisher_handler(const t::publisher_handler_m& publisher_handler);

    void rem_publisher_handler(const t::publisher_handler_m& publisher_handler);

    template <typename lambda_function>
    bool notify(const std::string& publisher_handler_topic_id, const lambda_function& function) {
        bool notified_ = false;

        auto publisher_handler_it_ = this->_publisher_handlers_table.find(publisher_handler_topic_id);
        if (publisher_handler_it_ != this->_publisher_handlers_table.end()) {
            const t::publisher_handler_m& publisher_handler_ = (*publisher_handler_it_).second;
            auto newFunction_ = [this, &function](t::publisher_handler_mptr publisher_handler) {
                return function(this, publisher_handler);
            };
            notified_ = publisher_handler_->notify(newFunction_);
        }

        return notified_;
    }

    bool notify(const t::publisher_handler_sender_m& publisher_handler_sender);
};

class publisher_controller {
private:
    std::string _named_id;
    t::publisher_table_m _publishers;
    t::publisher_list_i _publishers_faileds;
protected:
    void clear_publishers_faileds();
    void add_publisher_failed(const t::publisher_m& publisher_failed);
public:
    explicit publisher_controller(const std::string& named_id);

    ~publisher_controller() = default;

    const std::string& get_named_id() const;

    const t::publisher_list_i& get_publishers_faileds() const;

    virtual void add_publisher(const t::publisher_m& publisher);

    virtual void rem_publisher(const t::publisher_m& publisher);

    const t::publisher_table_m& get_publishers() const;

    template <typename lambda_function>
    void notify_all(const std::string& publisher_handler_topic_id, const lambda_function& function) {

        this->clear_publishers_faileds();

        for (const auto& publisher_pair_ : this->_publishers) {
            const t::publisher_m& publisher_ = publisher_pair_.second;
            auto newFunction_ = [this, &function](
            t::publisher_mptr publisher, t::publisher_handler_mptr publisher_handler) {
                return function(this, publisher, publisher_handler);
            };
            if (!publisher_->notify(publisher_handler_topic_id, newFunction_)) {
                add_publisher_failed(publisher_);
            }
        }
    }

    void notify_all(const t::publisher_handler_sender_m& publisher_handler_sender);
};

} //network
} //system
} //_unnamed
