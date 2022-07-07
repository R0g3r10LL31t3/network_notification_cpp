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


#include "network_announcer.h"

using namespace _unnamed::system::network;

const std::string& announcer_handler_notifier::get_announcer_handler_named_id() const {
    return get_announcer_handler()->get_named_id();
}

void announcer_handler_notifier::set_announcer_handler(t::announcer_handler_mptr announcer_handler) {
    this->_announcer_handler = announcer_handler;
}

t::announcer_handler_iptr announcer_handler_notifier::get_announcer_handler() const {
    return this->_announcer_handler;
}

void announcer_handler_notifier::set_announcer(t::announcer_mptr announcer) {
    this->_announcer = announcer;
}

t::announcer_iptr announcer_handler_notifier::get_announcer() const {
    return this->_announcer;
}

void announcer_handler_notifier::set_announcer_controller(t::announcer_controller_mptr announcer_controller) {
    this->_announcer_controller = announcer_controller;
}

t::announcer_controller_iptr announcer_handler_notifier::get_announcer_controller() const {
    return this->_announcer_controller;
}

announcer_handler::announcer_handler(const std::string& named_id) {
    this->_named_id = named_id;
}

const std::string& announcer_handler::get_named_id() const {
    return this->_named_id;
}

bool announcer_handler::notify(const t::announcer_handler_notifier_m& announcer_handler_notifier) {
    return announcer_handler_notifier->do_notify();
}

announcer::announcer(const std::string& named_id)
    : _named_id(named_id) {
}

const std::string& announcer::get_named_id() const {
    return this->_named_id;
}

const t::announcer_handlers_table_m& announcer::get_announcer_handlers_table() const {
    return this->_announcer_handlers_table;
}

void announcer::add_announcer_handler(const t::announcer_handler_m& announcer_handler) {
    this->_announcer_handlers_table.insert({ announcer_handler->get_named_id(), announcer_handler });
}

void announcer::rem_announcer_handler(const t::announcer_handler_m& announcer_handler) {
    this->_announcer_handlers_table.erase(announcer_handler->get_named_id());
}

bool announcer::notify(const t::announcer_handler_notifier_m& announcer_handler_notifier) {
    bool notified_ = false;

    auto announcer_handler_it_ = this->_announcer_handlers_table.find(announcer_handler_notifier->get_announcer_handler_named_id());
    if (announcer_handler_it_ != this->_announcer_handlers_table.end()) {
        const t::announcer_handler_m announcer_handler_ = (*announcer_handler_it_).second;
        announcer_handler_notifier->set_announcer(this);
        notified_ = announcer_handler_->notify(announcer_handler_notifier);
        announcer_handler_notifier->set_announcer(nullptr);
    }

    return notified_;
}

announcer_controller::announcer_controller(const std::string& named_id)
    : _named_id(named_id) {
}

const std::string& announcer_controller::get_named_id() const {
    return this->_named_id;
}

void announcer_controller::add_announcer(const t::announcer_m& announcer) {
    this->_announcers.insert({ announcer->get_named_id(), announcer });
}

void announcer_controller::rem_announcer(const t::announcer_m& announcer) {
    this->_announcers.erase(announcer->get_named_id());
}

const t::announcer_table_m& announcer_controller::get_announcers() const {
    return this->_announcers;
}

void announcer_controller::clear_announcers_faileds() {
    this->_announcers_faileds.clear();
}

void announcer_controller::add_announcer_failed(const t::announcer_m& announcer_failed) {
    this->_announcers_faileds.push_back(announcer_failed);
}

const t::announcer_list_i& announcer_controller::get_announcers_faileds() const {
    return this->_announcers_faileds;
}

void announcer_controller::notify_all(const t::announcer_handler_notifier_m& announcer_handler_notifier) {

    this->clear_announcers_faileds();

    for (const auto& announcer_address_pair_ : this->_announcers) {
        const t::announcer_m& announcer_ = announcer_address_pair_.second;
        announcer_handler_notifier->set_announcer_controller(this);
        if (!announcer_->notify(announcer_handler_notifier)) {
            this->add_announcer_failed(announcer_);
        }
        announcer_handler_notifier->set_announcer_controller(nullptr);
    }
}
