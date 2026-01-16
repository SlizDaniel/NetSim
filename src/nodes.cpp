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
    auto
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