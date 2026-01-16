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