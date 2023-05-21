// Copyright 2021 GHA Test Team
#include "task.h"


Buyer::Buyer(int products_number) : products_number(products_number) {}

long long Buyer::time_counting() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - deque_time_start).count();
}


Cashbox::Cashbox(unsigned service_intensity, Statistics &statistics) : statistics(statistics) {
    time = 1000 / (int)service_intensity;
}

void Cashbox::start_service(Buyer buyer) {
    std::mutex mutex;
    std::lock_guard<std::mutex> lock(mutex);
    is_free = false;
    std::thread thread(&Cashbox::service, this, buyer);
    thread.detach();
}

void Cashbox::service(Buyer buyer) {
    statistics.summary_cashbox_downtime_update(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - downtime_start).count());
    worktime_start = std::chrono::system_clock::now();
    std::this_thread::sleep_for(std::chrono::milliseconds(buyer.products_number * time));
    statistics.summary_cashbox_worktime_update(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - worktime_start).count());
    is_free = true;
}


Statistics::Statistics(long long unsigned sampling_time, std::string filename) : sampling_time(1000 * sampling_time), filename(std::move(filename)) {}

void Statistics::queue_length_update(long long unsigned deque_length) { summary_queue_length += deque_length; }
void Statistics::served_byers_update() { served_byers_number++; }
void Statistics::unserved_byers_update() { unserved_byers_number++; }
void Statistics::summary_queue_time_update(long long queue_time) { summary_queue_time += queue_time; }
void Statistics::summary_cashbox_worktime_update(long long cashbox_worktime) { summary_cashbox_worktime += cashbox_worktime; }
void Statistics::summary_cashbox_downtime_update(long long cashbox_downtime) { summary_cashbox_downtime += cashbox_downtime; }

void Statistics::total() {

    unsigned int byers_number = served_byers_number + unserved_byers_number;
    double average_queue_length = (double)summary_queue_length / byers_number;
    double average_queue_time = (double)summary_queue_time / 1000 / served_byers_number;
    double average_cashbox_time = (double)summary_cashbox_worktime / 1000 / served_byers_number;
    double average_cashbox_downtime = (double)summary_cashbox_downtime / 1000 / served_byers_number;

    double failure_probability = (double)unserved_byers_number / byers_number;
    double relative_throughput = 1 - failure_probability;
    double absolute_throughput = ((double)(1000 * byers_number) / (double)sampling_time) * relative_throughput;

    std::ofstream f(filename + ".txt", std::ios_base::app);
    f << "STATISTICAL DATA\n"
      << "\nserved byers number: " << served_byers_number
      << "\nunserved byers number: " << unserved_byers_number
      << "\naverage cashbox downtime: " << average_cashbox_downtime
      << "\n"
      << "\naverage queue length: " << average_queue_length
      << "\naverage queue time: " << average_queue_time
      << "\naverage cashbox time: " << average_cashbox_time
      << "\nfailure probability: " << failure_probability
      << "\nrelative_throughput: " << relative_throughput
      << "\nabsolute_throughput: " << absolute_throughput;
    f.close();
}


Shop::Shop(unsigned cashbox_number,
           unsigned intake_intensity,
           unsigned service_intensity,
           unsigned average_products_number,
           unsigned max_queue_length,
           Statistics &statistics) : statistics(statistics) {

    gen.seed(rnd());

    this->intake_intensity = intake_intensity;
    this->average_products_number = average_products_number;
    this->max_queue_length = max_queue_length;

    for (int i = 0; i < cashbox_number; i++) {
        cashboxes.emplace_back(service_intensity, statistics);
    }
}

void Shop::start_work() {
    std::uniform_int_distribution<> products_number_range(1, 2 * average_products_number - 1);
    unsigned time = 1000 / intake_intensity;
    std::uniform_int_distribution<> time_range(1, 2 * time - 1);
    while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start_time).count() < statistics.sampling_time) {
        auto buyer = Buyer(products_number_range(gen));
        buyer_service(buyer);
        std::this_thread::sleep_for(std::chrono::milliseconds(time_range(gen)));
    }
    statistics.total();
}

void Shop::buyer_service(Buyer buyer) {
    if (buyers.size() < max_queue_length) {
        buyers.push_back(buyer);
    }
    else {
        statistics.unserved_byers_update();
    }
    statistics.queue_length_update(buyers.size());
    deque_update();
}

void Shop::deque_update() {
    for (auto &cashbox: cashboxes) {
        if (cashbox.is_free) {
            cashbox.start_service(buyers.front());
            statistics.served_byers_update();
            statistics.summary_queue_time_update(buyers.front().time_counting());
            buyers.pop_front();
            return;
        }
    }
}
