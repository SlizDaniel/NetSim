//
// Created by slizd on 7.01.2026.
//

#ifndef NETSIM_NODES_HPP
#define NETSIM_NODES_HPP

#include "storage_types.hpp"

class IPackageReceiver{

public:
    virtual void receive_package(Package&& p) = 0;

    virtual void get_id(ElementID) const = 0;

    virtual const_iterator begin() const = 0;

    virtual const_iterator end() const = 0;

    virtual const_iterator cbegin() const = 0;

    virtual const_iterator cend() const = 0;

    virtual ~IPackageReceiver() = default;
};

class ReceiverPreferences : IPackageReceiver {
public:
};
#endif //NETSIM_NODES_HPP