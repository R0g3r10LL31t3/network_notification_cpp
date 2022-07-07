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

class observer_handler_notifier;
class observer_handler;
class observer;
class observer_controller;

namespace t {
/**
 * Mutable and imutable smart shared pointers to objects
 * Mutable objects can be modifiyed
 * Imutable objects can not be modifiyed, but const methods can be invoked
 */
using observer_handler_notifier_i = std::shared_ptr<const observer_handler_notifier>;
using observer_handler_notifier_m = std::shared_ptr<observer_handler_notifier>;

using observer_handler_i = std::shared_ptr<const observer_handler>;
using observer_handler_m = std::shared_ptr<observer_handler>;

using observer_i = std::shared_ptr<const observer>;
using observer_m = std::shared_ptr<observer>;

using observer_controller_i = std::shared_ptr<const observer_controller>;
using observer_controller_m = std::shared_ptr<observer_controller>;

/**
 * Constant mutable and imutable pointers to objects
 * Mutable objects can be modifiyed
 * Imutable objects can not be modifiyed, but const methods can be invoked
 */
using observer_handler_iptr = observer_handler* const;
using observer_handler_mptr = observer_handler*;

using observer_iptr = observer* const;
using observer_mptr = observer*;

using observer_controller_iptr = observer_controller* const;
using observer_controller_mptr = observer_controller*;

/**
 * Data Structure
 */
using observer_handlers_table_i = std::map<std::string, t::observer_handler_i>;
using observer_handlers_table_m = std::map<std::string, const t::observer_handler_m>;

using observer_table_i = std::map<std::string, t::observer_i>;
using observer_table_m = std::map<std::string, const t::observer_m>;

using observer_list_i = std::list<t::observer_i>;
using observer_list_m = std::list<const t::observer_m>;
} //t

class observer_handler_notifier {
protected:
    t::observer_handler_mptr _observer_handler { nullptr };
    t::observer_mptr _observer { nullptr };
    t::observer_controller_mptr _observer_controller { nullptr };

    observer_handler_notifier() = default;
public:
    ~observer_handler_notifier() = default;

    const std::string& get_observer_handler_named_id() const;

    void set_observer_handler(t::observer_handler_mptr observer_handler);

    t::observer_handler_iptr get_observer_handler() const;

    void set_observer(t::observer_mptr observer);

    t::observer_iptr get_observer() const;

    void set_observer_controller(t::observer_controller_mptr observer_controller);

    t::observer_controller_iptr get_observer_controller() const;

    virtual bool do_notify() = 0;
};

class observer_handler {
private:
    std::string _named_id;
public:
    explicit observer_handler(const std::string& named_id);

    ~observer_handler() = default;

    const std::string& get_named_id() const;

    template <typename lambda_function>
    bool notify(const lambda_function& function) {
        return function(this);
    }

    bool notify(const t::observer_handler_notifier_m& observer_handler_notifier);
};

class observer {
private:
    std::string _named_id;
    t::observer_handlers_table_m _observer_handlers_table;
public:
    explicit observer(const std::string& named_id);

    ~observer() = default;

    const std::string& get_named_id() const;

    const t::observer_handlers_table_m& get_observer_handlers_table() const;

    void add_observer_handler(const t::observer_handler_m& observer_handler);

    void rem_observer_handler(const t::observer_handler_m& observer_handler);

    template <typename lambda_function>
    bool notify(const std::string& observer_handler_named_id, const lambda_function& function) {
        bool notified_ = false;

        auto observer_handler_it_ = this->_observer_handlers_table.find(observer_handler_named_id);
        if (observer_handler_it_ != this->_observer_handlers_table.end()) {
            const t::observer_handler_m& observer_handler_ = (*observer_handler_it_).second;
            auto newFunction_ = [this, &function](t::observer_handler_mptr observer_handler) {
                return function(this, observer_handler);
            };
            notified_ = observer_handler_->notify(newFunction_);
        }

        return notified_;
    }

    bool notify(const t::observer_handler_notifier_m& observer_handler_notifier);
};

class observer_controller {
private:
    std::string _named_id;
    t::observer_table_m _observers;
    t::observer_list_i _observers_faileds;
protected:
    void clear_observers_faileds();
    void add_observer_failed(const t::observer_m& observer_failed);
public:
    explicit observer_controller(const std::string& named_id);

    ~observer_controller() = default;

    const std::string& get_named_id() const;

    const t::observer_list_i& get_observers_faileds() const;

    virtual void add_observer(const t::observer_m& observer);

    virtual void rem_observer(const t::observer_m& observer);

    const t::observer_table_m& get_observers() const;

    template <typename lambda_function>
    void notify_all(const std::string& observer_handler_named_id, const lambda_function& function) {

        this->clear_observers_faileds();

        for (const auto& observer_pair_ : this->_observers) {
            const t::observer_m& observer_ = observer_pair_.second;
            auto newFunction_ = [this, &function](
            t::observer_mptr observer, t::observer_handler_mptr observer_handler) {
                return function(this, observer, observer_handler);
            };
            if (!observer_->notify(observer_handler_named_id, newFunction_)) {
                add_observer_failed(observer_);
            }
        }
    }

    void notify_all(const t::observer_handler_notifier_m& observer_handler_notifier);
};

} //network
} //system
} //_unnamed
