//
// Created by micha on 15.01.2026.
//

#include "nodes.hpp"

void ReceiverPreferences::add_receiver(IPackageReceiver* ptr) {
    preferences_t_[ptr] = 0.0;

    double new_prob = 1.0 / static_cast<double>(preferences_t_.size());

    for (auto& [receiver, probability] : preferences_t_) {
        probability = new_prob;
    }
}

void ReceiverPreferences::remove_receiver(IPackageReceiver* ptr) {
    if (preferences_t_.find(ptr) != preferences_t_.end()) {
        preferences_t_.erase(ptr);

        double new_prob = 1.0 / static_cast<double>(preferences_t_.size());
        for (auto& [receiver, probability] : preferences_t_) {
            probability = new_prob;
        }
    }
}

IPackageReceiver* ReceiverPreferences::choose_receiver() {
    double prob = pg_();

    double cumulative_sum = 0.0;
    for (auto const& [receiver, preference] : preferences_t_) {
        cumulative_sum += preference;
        if (prob <= cumulative_sum) {
            return receiver;
        }
    }

    if (!preferences_t_.empty()) {
        return preferences_t_.rbegin()->first;
    }

    return nullptr;
}

void PackageSender::send_package() {
    if (!sending_buffer.has_value()) { return; }

    IPackageReceiver* receiver = receiver_preferences_.choose_receiver();
    receiver->receive_package(std::move(*sending_buffer));
    sending_buffer.reset();
}

void Storehouse::receive_package(Package&& p) {
    d_->push(std::move(p));
}

void Worker::receive_package(Package &&p) {
    queue->push(std::move(p));
}

void Worker::do_work(Time t) {
    if (!processing_buffer.has_value() && !queue->empty()) {
        package_processing_start_time=t;
        processing_buffer.emplace(queue->pop());
    }
    if (t - package_processing_start_time + 1 == processing_duration) {
        push_package(Package(processing_buffer.value().get_id()));
        processing_buffer.reset();
    }
}

void Ramp::deliver_goods(Time t) {
    if (!buffer_) {
        push_package(Package());
        buffer_.emplace(id_);
        t_ = t;
    }
    else if (t - time_offset_ == t_) {
        push_package(Package());
    }
}
