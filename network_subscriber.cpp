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


#include "network_subscriber.h"

using namespace _unnamed::system::network;

const std::string& subscriber_handler_receiver::get_subscriber_handler_topic_id() const {
    return get_subscriber_handler()->get_topic_id();
}

void subscriber_handler_receiver::set_subscriber_handler(t::subscriber_handler_mptr subscriber_handler) {
    this->_subscriber_handler = subscriber_handler;
}

t::subscriber_handler_iptr subscriber_handler_receiver::get_subscriber_handler() const {
    return this->_subscriber_handler;
}

void subscriber_handler_receiver::set_subscriber(t::subscriber_mptr subscriber) {
    this->_subscriber = subscriber;
}

t::subscriber_iptr subscriber_handler_receiver::get_subscriber() const {
    return this->_subscriber;
}

void subscriber_handler_receiver::set_subscriber_controller(t::subscriber_controller_mptr subscriber_controller) {
    this->_subscriber_controller = subscriber_controller;
}

t::subscriber_controller_iptr subscriber_handler_receiver::get_subscriber_controller() const {
    return this->_subscriber_controller;
}

subscriber_handler::subscriber_handler(const std::string& topic_id) {
    this->_topic_id = topic_id;
}

const std::string& subscriber_handler::get_topic_id() const {
    return this->_topic_id;
}

bool subscriber_handler::observes(const t::subscriber_handler_receiver_m& subscriber_handler_receiver) {
    return subscriber_handler_receiver->do_observes();
}

subscriber::subscriber(const std::string& named_id)
    : _named_id(named_id) {
}

const std::string& subscriber::get_named_id() const {
    return this->_named_id;
}

const t::subscriber_handlers_table_m& subscriber::get_subscriber_handlers_table() const {
    return this->_subscriber_handlers_table;
}

void subscriber::add_subscriber_handler(const t::subscriber_handler_m& subscriber_handler) {
    this->_subscriber_handlers_table.insert({ subscriber_handler->get_topic_id(), subscriber_handler });
}

void subscriber::rem_subscriber_handler(const t::subscriber_handler_m& subscriber_handler) {
    this->_subscriber_handlers_table.erase(subscriber_handler->get_topic_id());
}

bool subscriber::observes(const t::subscriber_handler_receiver_m& subscriber_handler_receiver) {
    bool notified_ = false;

    auto subscriber_handler_it_ = this->_subscriber_handlers_table.find(subscriber_handler_receiver->get_subscriber_handler_topic_id());
    if (subscriber_handler_it_ != this->_subscriber_handlers_table.end()) {
        const t::subscriber_handler_m subscriber_handler_ = (*subscriber_handler_it_).second;
        subscriber_handler_receiver->set_subscriber(this);
        notified_ = subscriber_handler_->observes(subscriber_handler_receiver);
        subscriber_handler_receiver->set_subscriber(nullptr);
    }

    return notified_;
}

subscriber_controller::subscriber_controller(const std::string& named_id)
    : _named_id(named_id) {
}

const std::string& subscriber_controller::get_named_id() const {
    return this->_named_id;
}

void subscriber_controller::add_subscriber(const t::subscriber_m& subscriber) {
    this->_subscribers.insert({ subscriber->get_named_id(), subscriber });
}

void subscriber_controller::rem_subscriber(const t::subscriber_m& subscriber) {
    this->_subscribers.erase(subscriber->get_named_id());
}

const t::subscriber_table_m& subscriber_controller::get_subscribers() const {
    return this->_subscribers;
}

void subscriber_controller::clear_subscribers_faileds() {
    this->_subscribers_faileds.clear();
}

void subscriber_controller::add_subscriber_failed(const t::subscriber_m& subscriber_failed) {
    this->_subscribers_faileds.push_back(subscriber_failed);
}

const t::subscriber_list_i& subscriber_controller::get_subscribers_faileds() const {
    return this->_subscribers_faileds;
}

void subscriber_controller::observes_all(const t::subscriber_handler_receiver_m& subscriber_handler_receiver) {

    this->clear_subscribers_faileds();

    for (const auto& subscriber_address_pair_ : this->_subscribers) {
        const t::subscriber_m& subscriber_ = subscriber_address_pair_.second;
        subscriber_handler_receiver->set_subscriber_controller(this);
        if (!subscriber_->observes(subscriber_handler_receiver)) {
            this->add_subscriber_failed(subscriber_);
        }
        subscriber_handler_receiver->set_subscriber_controller(nullptr);
    }
}
