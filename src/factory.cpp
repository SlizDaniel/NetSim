//
// Created by micha on 19.01.2026.
//

#include "factory.hpp"
#include <stdexcept>
#include "nodes.hpp"
#include <sstream>


bool has_reachable_storehouse(const PackageSender* sender,std::map<const PackageSender*,NodeColor>& node_colors){
    if (node_colors[sender]==NodeColor::VERIFIED) {
        return true;
    }
    node_colors[sender]=NodeColor::VISITED;
    if (sender->receiver_preferences_.get_preferences().empty()) {
        throw std::logic_error("Brak zdefiniowanych odbiorcow!");
    }
    bool has_neighbors = false;
    for (auto &pair : sender->receiver_preferences_.get_preferences()) {
        auto* receiver=pair.first;
        if (receiver->get_receiver_type()==ReceiverTypes::STOREHOUSE) {
            has_neighbors=true;
        }
        else if (receiver->get_receiver_type()==ReceiverTypes::WORKER) {
            auto worker_ptr = dynamic_cast<Worker*>(receiver);
            auto reworker_ptr = dynamic_cast<PackageSender*>(worker_ptr);

            if (static_cast<const void*>(reworker_ptr) == static_cast<const void*>(sender)) {
                continue;
            }
            has_neighbors=true;
            if (node_colors[worker_ptr]==NodeColor::UNVISITED) {
                has_reachable_storehouse(worker_ptr, node_colors);
            }
        }
    }
    if (has_neighbors) {
        node_colors[sender]=NodeColor::VERIFIED;
        return true;
    }
    else {
        throw std::logic_error("Brak prawidlowych odbiorcow");
    }
}

bool Factory::is_consistent(){
    std::map<const PackageSender*, NodeColor> node_colors;

    for (auto &worker : workers) {
        node_colors[&worker] = NodeColor::UNVISITED;
    }
    for (auto &ramp : ramps) {
        node_colors[&ramp] = NodeColor::UNVISITED;
    }
    try {
        for (auto &ramp : ramps) {
            has_reachable_storehouse (&ramp , node_colors);
            }
        }
    catch (const std::logic_error&) {
        return false;
    }
    return true;
};

void Factory::do_deliveries(Time t) {
    for (Ramp &r : ramps) {
        r.deliver_goods(t);
    }
}

void Factory::do_package_passing() {
    for (Worker &w : workers) {
        w.send_package();
    }
    for (Ramp &r : ramps) {
        r.send_package();
    }
}

void Factory::do_work(Time t) {
    for (Worker &w : workers) {
        w.do_work(t);
    }
}

ParsedLineData parse_line (std::string& line) {
    std::vector<std::string> tokens;
    std::string token;

    std::istringstream stream_line(line);
    char limiter = ' ';

    while (std::getline(stream_line, token , limiter)) {
        tokens.push_back(token);
    }

    ParsedLineData parsed_data;

    std::map<std::string, ElementType> element_types {
            {"LOADING_RAMP" , ElementType::RAMP},
            {"WORKER" , ElementType::WORKER},
            {"STOREHOUSE" , ElementType::STOREHOUSE},
            {"LINK" , ElementType::LINK}};

    try {
        parsed_data.element_type = element_types.at(tokens[0]);

        std::for_each(std::next(tokens.begin()), tokens.end(),[&parsed_data](const std::string& current_str) {
            std::istringstream stream_str (current_str);
            std::string str_;
            std::vector<std::string> tokens_;
            char limiter_ = '=';
            while (std::getline(stream_str , str_ , limiter_)) {
                tokens_.push_back(str_);
            }
            if (tokens_.size()==2){
                parsed_data.parameters [tokens_[0]] = tokens_[1];
            }
            else {
                throw std::invalid_argument("Invalid string structure!");
            }
        });
    }
    catch(std::out_of_range& v) {
        throw std::invalid_argument("Invalid data type:(");
    }
    return parsed_data;
}

std::vector<std::string> parsed_with_minus (std::string line_) {
    std::istringstream stream_line_ (line_);
    std::string current_line;
    std::vector<std::string> tokens_;
    char delimiter = '-';
    while (std::getline(stream_line_ , current_line,delimiter )) {
        tokens_.push_back(current_line);
    }
    return tokens_;
}

PackageQueueType get_package_queue_type (std::string& type_string) {
    std::map<std::string,PackageQueueType> str_to_type_map{
    {"FIFO" , PackageQueueType::FIFO},
    {"LIFO" , PackageQueueType::LIFO}};

    return str_to_type_map.at(type_string);
}

Factory load_factory_structure (std::istream& is) {
    Factory factory;
    std::string line;
    while(std::getline(is,line)) {
        if (line.empty() || line[0]==';') {
            continue;
        }
        ParsedLineData parsed_line = parse_line(line);

        switch (parsed_line.element_type) {
            case ElementType::WORKER: {
                ElementID id_ = std::stoi(parsed_line.parameters.at("id"));
                TimeOffset processing_time_ = std::stoi(parsed_line.parameters.at("processing-time"));
                PackageQueueType queue_type_ = get_package_queue_type (parsed_line.parameters.at("queue-type"));
                Worker worker (id_,processing_time_, std::make_unique<PackageQueue>(queue_type_));
                factory.add_worker(std::move(worker));
                break;
                }
            case ElementType::STOREHOUSE: {
                ElementID id_ = std::stoi(parsed_line.parameters.at("id"));
                Storehouse storehouse (id_);
                factory.add_storehouse(std::move(storehouse));
                break;
            }
            case ElementType::RAMP: {
                ElementID id_ = std::stoi(parsed_line.parameters.at("id"));
                TimeOffset delivery_time = std::stoi(parsed_line.parameters.at("delivery-interval"));
                Ramp ramp (id_,delivery_time);
                factory.add_ramp(std::move(ramp));
                break;
            }
            case ElementType::LINK: {
                enum class NodeType {
                    RAMP , STOREHOUSE , WORKER
                };
                std::map<std::string , NodeType> nodes_str{
                {"ramp" , NodeType::RAMP},
                {"worker" , NodeType::WORKER},
                {"store" , NodeType::STOREHOUSE}};

                std::string src_str = parsed_line.parameters.at("src");
                std::string dest_str = parsed_line.parameters.at("dest");

                auto parsed_src_with_minus = parsed_with_minus(src_str);
                NodeType src_type = nodes_str.at(parsed_src_with_minus[0]);
                ElementID src_id = std::stoi(parsed_src_with_minus[1]);

                auto parsed_dest_with_minus = parsed_with_minus(dest_str);
                NodeType dest_type = nodes_str.at(parsed_dest_with_minus[0]);
                ElementID dest_id = std::stoi(parsed_dest_with_minus[1]);

                IPackageReceiver* package_receiver = nullptr;
                switch (dest_type){
                    case NodeType::RAMP: {
                        break;
                    }
                    case NodeType::STOREHOUSE: {
                        package_receiver = &*factory.find_storehouse_by_id(dest_id);
                        break;
                    }
                    case NodeType::WORKER: {
                        package_receiver = &*factory.find_worker_by_id(dest_id);
                        break;
                    }
                }
                if (package_receiver != nullptr) {
                    switch (src_type){
                        case NodeType::RAMP: {
                            factory.find_ramp_by_id(src_id)->receiver_preferences_.add_receiver(package_receiver);
                            break;
                        }
                        case NodeType::STOREHOUSE: {
                            break;
                        }
                        case NodeType::WORKER: {
                            factory.find_worker_by_id(src_id)->receiver_preferences_.add_receiver(package_receiver);
                            break;
                        }
                    }
                }
                break;
            }
        }
    }
    return factory;
}

void write_receivers(std::ostream& os , const PackageSender& sender) {
    auto preferences_ = sender.receiver_preferences_.get_preferences();
    if (!preferences_.empty()) {
        os<<"  Receivers:\n";
        for (const auto& pair : preferences_) {
            const IPackageReceiver* receiver_ = pair.first;
            if (receiver_->get_receiver_type() == ReceiverTypes::WORKER) {
                ElementID id_ = receiver_->get_id();
                os<<"    worker #"<<id_<<"\n";
            }
            else if (receiver_->get_receiver_type() == ReceiverTypes::STOREHOUSE){
                ElementID id_ = receiver_->get_id();
                os<<"    storehouse #"<<id_<<"\n";
            }
            else {
                throw std::invalid_argument("Incorrect receiver type");
            }
        }
    }
    os<<"\n";
}

std::string queue_to_str(PackageQueueType queue_) {
    switch (queue_) {
        case(PackageQueueType::FIFO): {
            return "FIFO";
        }
        case(PackageQueueType::LIFO): {
            return "LIFO";
        }
    }
    return {};
};

void save_factory_structure (Factory& factory , std::ostream& os) {
    //ramp worker storehouse link
    os<<"== LOADING RAMPS ==\n\n";
    std::for_each(factory.ramp_cbegin(), factory.ramp_cend(), [&](const Ramp& ramp) {
        ElementID id_ = ramp.get_id();
        TimeOffset delivery_interval_ = ramp.get_delivery_interval();
        os<<"LOADING RAMP #"<<id_<<"\n  Delivery interval: "<<delivery_interval_;
        write_receivers(os , ramp);
    });
    os<<"\n== WORKERS ==\n\n";
    std::for_each(factory.worker_cbegin(), factory.worker_cend(), [&](const Worker& worker) {
        ElementID id_ = worker.get_id();
        Time processing_time_ = worker.get_processing_duration();
        PackageQueueType queue_type_ = worker.get_queue()->get_queue_type();
        os<<"WORKER #"<<id_<<"\n  Processing time: "<<processing_time_
        <<"\n  Queue type: "<<queue_to_str(queue_type_);

        write_receivers(os , worker);
    });
    os<<"\n== STOREHOUSES ==\n\n";
    std::for_each(factory.storehouse_cbegin(), factory.storehouse_cend(), [&](const Storehouse& storehouse) {
        ElementID id_ = storehouse.get_id();
        os<<"STOREHOUSE #"<<id_<<"\n";
    });
    os.flush();
}

