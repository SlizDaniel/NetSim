//
// Created by slizd on 7.01.2026.
//

#ifndef NETSIM_NODES_HPP
#define NETSIM_NODES_HPP

#include <map>
#include <memory>
#include <optional>

#include "helpers.hpp"
#include "storage_types.hpp"
#include "config.hpp"

enum class ReceiverTypes {
    WORKER , STOREHOUSE
};

class IPackageReceiver{
public:
    virtual void receive_package(Package&& p) = 0;

    virtual ElementID get_id() const = 0;

    virtual ReceiverTypes get_receiver_type() const = 0;

    virtual IPackageStockpile::const_iterator begin() const = 0;

    virtual IPackageStockpile::const_iterator end() const = 0;

    virtual IPackageStockpile::const_iterator cbegin() const = 0;

    virtual IPackageStockpile::const_iterator cend() const = 0;

#if (defined EXERCISE_ID && EXERCISE_ID != EXERCISE_ID_NODES)
    virtual ReceiverTypes get_receiver_type() const = 0;
#endif
    virtual ~IPackageReceiver() = default;

};

class ReceiverPreferences {
public:
    using preferences_t = std::map<IPackageReceiver*, double>;
    using const_iterator = preferences_t::const_iterator;

    ReceiverPreferences(ProbabilityGenerator pg = probability_generator) : pg_(std::move(pg)) {};

    void add_receiver(IPackageReceiver* ptr);
    void remove_receiver(IPackageReceiver* ptr);

    IPackageReceiver* choose_receiver();
    const preferences_t& get_preferences() const {
        return this->preferences_t_;
    };

    const_iterator begin() const{
        return preferences_t_.begin();
    };
    const_iterator end() const{
        return preferences_t_.end();
    };
    const_iterator cbegin() const{
        return preferences_t_.cbegin();
    };
    const_iterator cend() const {
        return preferences_t_.cend();
    };

private:
    std::map<IPackageReceiver*, double> preferences_t_;
    ProbabilityGenerator pg_;
};

class PackageSender{
public:
    PackageSender() = default;
    PackageSender(PackageSender&&) = default;
    PackageSender& operator=(PackageSender&&) = default;
    void send_package();
    void remove_receiver(IPackageReceiver* receiver) {
        receiver_preferences_.remove_receiver(receiver);
    }
    const std::optional<Package>& get_sending_buffer() const { return sending_buffer; };
    ReceiverPreferences receiver_preferences_;
protected:
    void push_package(Package&& moved_package) { sending_buffer.emplace(moved_package.get_id()); };
    std::optional<Package> sending_buffer = std::nullopt;
};

class Storehouse : public IPackageReceiver {
public:
    ReceiverTypes get_receiver_type() const override {
        return ReceiverTypes::STOREHOUSE;
    }
    Storehouse(ElementID id, std::unique_ptr<IPackageStockpile> d = std::make_unique<PackageQueue>(PackageQueueType::FIFO))
                : id_(id), d_(std::move(d)) {}
    void receive_package(Package&& p) override;
    ElementID get_id() const override {
        return id_;
    }

#if (defined EXERCISE_ID && EXERCISE_ID != EXERCISE_ID_NODES)
    ReceiverTypes get_receiver_type() const override { return ReceiverTypes::STOREHOUSE; }
#endif

    IPackageStockpile::const_iterator begin() const override {
        return d_->begin();
    };
    IPackageStockpile::const_iterator end() const override {
        return d_->end();
    }
    IPackageStockpile::const_iterator cbegin() const override {
        return d_->cbegin();
    }
    IPackageStockpile::const_iterator cend() const override {
        return d_->cend();
    }

private:
    ElementID id_;
    std::unique_ptr<IPackageStockpile> d_;
};

class Worker : public PackageSender,  public IPackageReceiver {
public:
    Worker(ElementID id, TimeOffset pd, std::unique_ptr<IPackageQueue> q)
        :id_(id), processing_duration(pd), package_processing_start_time(0), queue(std::move(q)) {
    }
    ReceiverTypes get_receiver_type() const override {
        return ReceiverTypes::WORKER;
    }

    void do_work (Time t);

    TimeOffset get_processing_duration () const {return processing_duration;}

    Time get_package_processing_start_time () const {return package_processing_start_time;}

    ElementID get_id () const override {return id_;}

    #if (defined EXERCISE_ID && EXERCISE_ID != EXERCISE_ID_NODES)
        ReceiverTypes get_receiver_type() const override { return ReceiverTypes::WORKER; }
    #endif

    const std::optional<Package>& get_processing_buffer() const { return processing_buffer; }

    void receive_package (Package&& p) override;

    IPackageStockpile::const_iterator end () const override {return queue->end();}

    IPackageStockpile::const_iterator begin () const override {return queue->begin();}

    IPackageStockpile::const_iterator cend () const override {return queue->cend();}

    IPackageStockpile::const_iterator cbegin() const override {return queue->cbegin();}

    IPackageQueue* get_queue () const {return queue.get();}

private:
    ElementID id_;

    TimeOffset processing_duration;

    Time package_processing_start_time;

    std::unique_ptr<IPackageQueue> queue;

    std::optional<Package> processing_buffer = std::nullopt;
};

class Ramp : public PackageSender {
public:
    Ramp(ElementID id, TimeOffset time_offset) : id_(id), time_offset_(time_offset), t_(0) {};

    ElementID get_id () const {return id_;}

    Time get_delivery_interval () const {return time_offset_;}

    void deliver_goods (Time t);
private:
    ElementID id_;
    TimeOffset time_offset_;
    std::optional<Package> buffer_;
    Time t_;
};


#endif //NETSIM_NODES_HPP