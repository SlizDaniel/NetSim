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

enum class ReceiverTypes {
    WORKER , STOREHOUSE
};

class IPackageReceiver{
public:
    virtual void receive_package(Package&& p) = 0;

    virtual ElementID get_id() const = 0;

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

    ReceiverPreferences(ProbabilityGenerator pg) : pg_(std::move(pg)) {};

    void add_receiver(IPackageReceiver* ptr);
    void remove_receiver(IPackageReceiver* ptr);

    IPackageReceiver* choose_receiver();
    const preferences_t& get_preferences() {
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
    PackageSender(ProbabilityGenerator pg = probability_generator)
        : preferences(pg) {}

    PackageSender(PackageSender&&) = default;
    PackageSender& operator=(PackageSender&&) = default;
    void push_package(Package&& p);
    void send_package();
    const std::optional<Package>& get_sending_buffer() const { return sending_buffer; };
    ReceiverPreferences preferences;
protected:
    std::optional<Package> sending_buffer = std::nullopt;
};
class Storehouse : public IPackageReceiver {
public:
    Storehouse(ElementID id, std::unique_ptr<IPackageStockpile> d) {
        this -> id_ = id;
        this -> d_ = std::move(d);
    }
    void receive_package(Package&& p) override;
    ElementID get_id() const override {
        return id_;
    }

    static ReceiverTypes get_receiver_type() { return ReceiverTypes::STOREHOUSE; }

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

class Worker : public PackageSender,  public IPackageQueue {
public:
    Worker(ElementID id , TimeOffset pd , Time t, std::unique_ptr<IPackageQueue> q) : id_(id), processing_duration(pd), package_processing_start_time(t),
                                                                              queue(std::move(q)) {}
    void do_work (Time t);

    TimeOffset get_processing_duration () const {return processing_duration;}

    Time get_package_processing_start_time () const {return package_processing_start_time;}

    ElementID get_id () const {return id_;}

    static ReceiverTypes get_receiver_type () {return ReceiverTypes::WORKER;}

    std::optional<Package> get_processing_buffer () const {return processing_buffer;}

    void receive_package (Package&& p);

    const_iterator end () const override {return queue->end();}

    const_iterator begin () const override {return queue->begin();}

    const_iterator cend () const override {return queue->cend();}

    const_iterator cbegin() const override {return queue->cbegin();}
private:
    ElementID id_;

    TimeOffset processing_duration;

    Time package_processing_start_time;

    std::unique_ptr<IPackageQueue> queue;

    std::optional<Package> processing_buffer = std::nullopt;
};

class Ramp : public PackageSender {
public:
    Ramp(ElementID id, TimeOffset time_offset) :  PackageSender(), id_(id), time_offset_(time_offset) {};
private:
    ElementID id_;
    TimeOffset time_offset_;
};


#endif //NETSIM_NODES_HPP