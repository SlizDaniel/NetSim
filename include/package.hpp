//
// Created by slizd on 7.01.2026.
//

#ifndef NETSIM_PACKAGE_HPP
#define NETSIM_PACKAGE_HPP
#include "types.hpp"
#include <set>

class Package {
public:
    Package();

    explicit Package(ElementID ID);

    Package(Package &&package) noexcept;

    Package &operator = (Package &&package) noexcept;

    ElementID get_id() const;

    ~Package();

private:
    ElementID ID_;
    static std::set<ElementID> assigned_IDs;
    static std::set<ElementID> freed_IDs;
};

#endif //NETSIM_PACKAGE_HPP