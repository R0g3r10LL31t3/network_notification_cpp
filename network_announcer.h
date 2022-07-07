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

class announcer_handler_notifier;
class announcer_handler;
class announcer;
class announcer_controller;

namespace t {
/**
 * Mutable and imutable smart shared pointers to objects
 * Mutable objects can be modifiyed
 * Imutable objects can not be modifiyed, but const methods can be invoked
 */
using announcer_handler_notifier_i = std::shared_ptr<const announcer_handler_notifier>;
using announcer_handler_notifier_m = std::shared_ptr<announcer_handler_notifier>;

using announcer_handler_i = std::shared_ptr<const announcer_handler>;
using announcer_handler_m = std::shared_ptr<announcer_handler>;

using announcer_i = std::shared_ptr<const announcer>;
using announcer_m = std::shared_ptr<announcer>;

using announcer_controller_i = std::shared_ptr<const announcer_controller>;
using announcer_controller_m = std::shared_ptr<announcer_controller>;

/**
 * Constant mutable and imutable pointers to objects
 * Mutable objects can be modifiyed
 * Imutable objects can not be modifiyed, but const methods can be invoked
 */
using announcer_handler_iptr = announcer_handler* const;
using announcer_handler_mptr = announcer_handler*;

using announcer_iptr = announcer* const;
using announcer_mptr = announcer*;

using announcer_controller_iptr = announcer_controller* const;
using announcer_controller_mptr = announcer_controller*;

/**
 * Data Structure
 */
using announcer_handlers_table_i = std::map<std::string, t::announcer_handler_i>;
using announcer_handlers_table_m = std::map<std::string, const t::announcer_handler_m>;

using announcer_table_i = std::map<std::string, t::announcer_i>;
using announcer_table_m = std::map<std::string, const t::announcer_m>;

using announcer_list_i = std::list<t::announcer_i>;
using announcer_list_m = std::list<const t::announcer_m>;
} //t

class announcer_handler_notifier {
protected:
    t::announcer_handler_mptr _announcer_handler { nullptr };
    t::announcer_mptr _announcer { nullptr };
    t::announcer_controller_mptr _announcer_controller { nullptr };

    announcer_handler_notifier() = default;
public:
    ~announcer_handler_notifier() = default;

    const std::string& get_announcer_handler_named_id() const;

    void set_announcer_handler(t::announcer_handler_mptr announcer_handler);

    t::announcer_handler_iptr get_announcer_handler() const;

    void set_announcer(t::announcer_mptr announcer);

    t::announcer_iptr get_announcer() const;

    void set_announcer_controller(t::announcer_controller_mptr announcer_controller);

    t::announcer_controller_iptr get_announcer_controller() const;

    virtual bool do_notify() = 0;
};

class announcer_handler {
private:
    std::string _named_id;
public:
    explicit announcer_handler(const std::string& named_id);

    ~announcer_handler() = default;

    const std::string& get_named_id() const;

    template <typename lambda_function>
    bool notify(const lambda_function& function) {
        return function(this);
    }

    bool notify(const t::announcer_handler_notifier_m& announcer_handler_notifier);
};

class announcer {
private:
    std::string _named_id;
    t::announcer_handlers_table_m _announcer_handlers_table;
public:
    explicit announcer(const std::string& named_id);

    ~announcer() = default;

    const std::string& get_named_id() const;

    const t::announcer_handlers_table_m& get_announcer_handlers_table() const;

    void add_announcer_handler(const t::announcer_handler_m& announcer_handler);

    void rem_announcer_handler(const t::announcer_handler_m& announcer_handler);

    template <typename lambda_function>
    bool notify(const std::string& announcer_handler_named_id, const lambda_function& function) {
        bool notified_ = false;

        auto announcer_handler_it_ = this->_announcer_handlers_table.find(announcer_handler_named_id);
        if (announcer_handler_it_ != this->_announcer_handlers_table.end()) {
            const t::announcer_handler_m& announcer_handler_ = (*announcer_handler_it_).second;
            auto newFunction_ = [this, &function](t::announcer_handler_mptr announcer_handler) {
                return function(this, announcer_handler);
            };
            notified_ = announcer_handler_->notify(newFunction_);
        }

        return notified_;
    }

    bool notify(const t::announcer_handler_notifier_m& announcer_handler_notifier);
};

class announcer_controller {
private:
    std::string _named_id;
    t::announcer_table_m _announcers;
    t::announcer_list_i _announcers_faileds;
protected:
    void clear_announcers_faileds();
    void add_announcer_failed(const t::announcer_m& announcer_failed);
public:
    explicit announcer_controller(const std::string& named_id);

    ~announcer_controller() = default;

    const std::string& get_named_id() const;

    const t::announcer_list_i& get_announcers_faileds() const;

    virtual void add_announcer(const t::announcer_m& announcer);

    virtual void rem_announcer(const t::announcer_m& announcer);

    const t::announcer_table_m& get_announcers() const;

    template <typename lambda_function>
    void notify_all(const std::string& announcer_handler_named_id, const lambda_function& function) {

        this->clear_announcers_faileds();

        for (const auto& announcer_pair_ : this->_announcers) {
            const t::announcer_m& announcer_ = announcer_pair_.second;
            auto newFunction_ = [this, &function](
            t::announcer_mptr announcer, t::announcer_handler_mptr announcer_handler) {
                return function(this, announcer, announcer_handler);
            };
            if (!announcer_->notify(announcer_handler_named_id, newFunction_)) {
                add_announcer_failed(announcer_);
            }
        }
    }

    void notify_all(const t::announcer_handler_notifier_m& announcer_handler_notifier);
};

} //network
} //system
} //_unnamed
