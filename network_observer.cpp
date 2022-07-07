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


#include "network_observer.h"

using namespace _unnamed::system::network;

const std::string& observer_handler_notifier::get_observer_handler_named_id() const {
    return get_observer_handler()->get_named_id();
}

void observer_handler_notifier::set_observer_handler(t::observer_handler_mptr observer_handler) {
    this->_observer_handler = observer_handler;
}

t::observer_handler_iptr observer_handler_notifier::get_observer_handler() const {
    return this->_observer_handler;
}

void observer_handler_notifier::set_observer(t::observer_mptr observer) {
    this->_observer = observer;
}

t::observer_iptr observer_handler_notifier::get_observer() const {
    return this->_observer;
}

void observer_handler_notifier::set_observer_controller(t::observer_controller_mptr observer_controller) {
    this->_observer_controller = observer_controller;
}

t::observer_controller_iptr observer_handler_notifier::get_observer_controller() const {
    return this->_observer_controller;
}

observer_handler::observer_handler(const std::string& named_id) {
    this->_named_id = named_id;
}

const std::string& observer_handler::get_named_id() const {
    return this->_named_id;
}

bool observer_handler::notify(const t::observer_handler_notifier_m& observer_handler_notifier) {
    return observer_handler_notifier->do_notify();
}

observer::observer(const std::string& named_id)
    : _named_id(named_id) {
}

const std::string& observer::get_named_id() const {
    return this->_named_id;
}

const t::observer_handlers_table_m& observer::get_observer_handlers_table() const {
    return this->_observer_handlers_table;
}

void observer::add_observer_handler(const t::observer_handler_m& observer_handler) {
    this->_observer_handlers_table.insert({ observer_handler->get_named_id(), observer_handler });
}

void observer::rem_observer_handler(const t::observer_handler_m& observer_handler) {
    this->_observer_handlers_table.erase(observer_handler->get_named_id());
}

bool observer::notify(const t::observer_handler_notifier_m& observer_handler_notifier) {
    bool notified_ = false;

    auto observer_handler_it_ = this->_observer_handlers_table.find(observer_handler_notifier->get_observer_handler_named_id());
    if (observer_handler_it_ != this->_observer_handlers_table.end()) {
        const t::observer_handler_m observer_handler_ = (*observer_handler_it_).second;
        observer_handler_notifier->set_observer(this);
        notified_ = observer_handler_->notify(observer_handler_notifier);
        observer_handler_notifier->set_observer(nullptr);
    }

    return notified_;
}

observer_controller::observer_controller(const std::string& named_id)
    : _named_id(named_id) {
}

const std::string& observer_controller::get_named_id() const {
    return this->_named_id;
}

void observer_controller::add_observer(const t::observer_m& observer) {
    this->_observers.insert({ observer->get_named_id(), observer });
}

void observer_controller::rem_observer(const t::observer_m& observer) {
    this->_observers.erase(observer->get_named_id());
}

const t::observer_table_m& observer_controller::get_observers() const {
    return this->_observers;
}

void observer_controller::clear_observers_faileds() {
    this->_observers_faileds.clear();
}

void observer_controller::add_observer_failed(const t::observer_m& observer_failed) {
    this->_observers_faileds.push_back(observer_failed);
}

const t::observer_list_i& observer_controller::get_observers_faileds() const {
    return this->_observers_faileds;
}

void observer_controller::notify_all(const t::observer_handler_notifier_m& observer_handler_notifier) {

    this->clear_observers_faileds();

    for (const auto& observer_address_pair_ : this->_observers) {
        const t::observer_m& observer_ = observer_address_pair_.second;
        observer_handler_notifier->set_observer_controller(this);
        if (!observer_->notify(observer_handler_notifier)) {
            this->add_observer_failed(observer_);
        }
        observer_handler_notifier->set_observer_controller(nullptr);
    }
}
