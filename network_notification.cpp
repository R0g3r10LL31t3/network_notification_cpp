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

using namespace _unnamed::system::network;

void listener_handler_notifier::set_listener_handler(t::listener_handler_mptr listener_handler) {
    this->_listener_handler = listener_handler;
}

t::listener_handler_iptr listener_handler_notifier::get_listener_handler() const {
    return this->_listener_handler;
}

void listener_handler_notifier::set_listener(t::listener_mptr listener) {
    this->_listener = listener;
}

t::listener_iptr listener_handler_notifier::get_listener() const {
    return this->_listener;
}

void listener_handler_notifier::set_subject(t::subject_mptr subject) {
    this->_subject = subject;
}

t::subject_iptr listener_handler_notifier::get_subject() const {
    return this->_subject;
}

bool listener_handler::notify(const t::listener_handler_notifier_m& listener_method_notifier) {
    bool notified_ = false;

    listener_method_notifier->set_listener_handler(this);
    notified_ = listener_method_notifier->do_notify();
    listener_method_notifier->set_listener(nullptr);

    return notified_;
}

listener::listener(const std::string& named_id)
    : _named_id(named_id) {
}

const std::string& listener::get_named_id() const {
    return this->_named_id;
}

const t::listener_handlers_table_m& listener::get_listener_handlers_table() const {
    return this->_listener_handlers_table;
}

void listener::add_listener_handler(const t::listener_handler_m& listener_handler) {
    this->_listener_handlers_table.insert({ listener_handler->get_listener_handler_name(), listener_handler });
}

void listener::rem_listener_handler(const t::listener_handler_m& listener_handler) {
    this->_listener_handlers_table.erase(listener_handler->get_listener_handler_name());
}

bool listener::notify(const t::listener_handler_notifier_m& listener_handler_notifier) {
    bool notified_ = false;

    auto listener_handler_it_ = this->_listener_handlers_table.find(listener_handler_notifier->get_listener_handler_name());
    if (listener_handler_it_ != this->_listener_handlers_table.end()) {
        const t::listener_handler_m listener_handler_ = (*listener_handler_it_).second;
        listener_handler_notifier->set_listener(this);
        notified_ = listener_handler_->notify(listener_handler_notifier);
        listener_handler_notifier->set_listener(nullptr);
    }

    return notified_;
}

subject::subject(const std::string& address, const std::string& port)
    : _address(address), _port(port) {
}

const std::string& subject::get_address() const {
    return this->_address;
}

const std::string& subject::get_port() const {
    return this->_port;
}

void subject::subscriber(const t::listener_m& listener) {
    this->_listeners.insert({ listener->get_named_id(), listener });
}

void subject::unsubscriber(const t::listener_m& listener) {
    this->_listeners.erase(listener->get_named_id());
}

const t::listener_table_m& subject::get_listeners() const {
    return this->_listeners;
}

void subject::clear_listeners_faileds() {
    this->_listeners_faileds.clear();
}

void subject::add_listener_failed(const t::listener_m& listener_failed) {
    this->_listeners_faileds.push_back(listener_failed);
}

const t::listener_list_i& subject::get_listeners_faileds() const {
    return this->_listeners_faileds;
}

void subject::notify_all(const t::listener_handler_notifier_m& listener_handler_notifier) {

    this->clear_listeners_faileds();

    for (const auto& listener_address_pair_ : this->_listeners) {
        const t::listener_m& listener_ = listener_address_pair_.second;
        listener_handler_notifier->set_subject(this);
        if (!listener_->notify(listener_handler_notifier)) {
            this->add_listener_failed(listener_);
        }
        listener_handler_notifier->set_subject(nullptr);
    }
}
