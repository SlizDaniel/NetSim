//
// Created by slizd on 13.01.2026.
//

#include "package.hpp"
#include "types.hpp"

Package::Package(){
    if (assigned_IDs.empty() && freed_IDs.empty()) {
        ID_ = 1;
    }
    else if (!freed_IDs.empty()) {
        ID_ = *freed_IDs.begin();
        freed_IDs.erase(*freed_IDs.begin());
    }
    else {
        ID_ = *assigned_IDs.end() + 1;
    }
    assigned_IDs.insert(ID_);
}

Package::Package(ElementID ID) {
    ID_ = ID;
    assigned_IDs.insert(ID_);
}

Package::Package(Package &&package) noexcept {
    ID_ = package.ID_;
    package.ID_ = 0;
}

Package &Package::operator=(Package &&package) noexcept {
    if (this == &package) {
        return *this;
    }
    assigned_IDs.erase(this->ID_);
    freed_IDs.insert(this->ID_);
    (this->ID_) = package.ID_;
    assigned_IDs.insert(this->ID_);
    return *this;
}

ElementID Package::get_id() const{
    return ID_;
}

Package::~Package() {
    assigned_IDs.erase(ID_);
    if (this->ID_ !=0) {
        freed_IDs.insert(ID_);
    }

}

