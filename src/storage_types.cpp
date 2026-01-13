//
// Created by slizd on 13.01.2026.
//

#include "storage_types.hpp"

Package PackageQueue::pop() {
    Package package;

    if (this -> queue_type_ == PackageQueueType::LIFO) {
        package = std::move(this -> package_list_.back());
        this -> package_list_.pop_back();
    }
    else {
        package = std::move(this -> package_list_.front());
        this -> package_list_.pop_front();
    }

    return package;
}
