// Copyright 2021 GHA Test Team
#pragma once
#include <iostream>
#include <deque>
#include <thread>
#include <mutex>
#include <chrono>
#include <random>
#include <fstream>


class Buyer {

public:

    int products_number;

private:

    std::chrono::system_clock::time_point deque_time_start = std::chrono::system_clock::now();

public:

    explicit Buyer(int products_number);
    long long time_counting();
};


class Statistics {

private:

    unsigned served_byers_number = 0;
    unsigned unserved_byers_number = 0;
    long unsigned summary_queue_length = 0;
    long long unsigned summary_queue_time = 0;
    long long unsigned summary_cashbox_worktime = 0;
    long long unsigned summary_cashbox_downtime = 0;

    std::string filename;

public:

    long long unsigned int sampling_time;

public:

    Statistics(long long unsigned sampling_time, std::string filename);

    void queue_length_update(long long unsigned deque_length);
    void served_byers_update();
    void unserved_byers_update();
    void summary_queue_time_update(long long queue_time);
    void summary_cashbox_worktime_update(long long cashbox_worktime);
    void summary_cashbox_downtime_update(long long cashbox_downtime);

    void total();
};


class Cashbox {

public:

    bool is_free = true;
    int time;

private:

    std::chrono::system_clock::time_point downtime_start = std::chrono::system_clock::now();
    std::chrono::system_clock::time_point worktime_start;

    Statistics &statistics;

public:

    explicit Cashbox(unsigned int service_intensity, Statistics &statistics);

    void start_service(Buyer buyer);

private:

    void service(Buyer buyer);
};


class Shop {

private:

    unsigned intake_intensity;
    unsigned average_products_number;
    unsigned max_queue_length;

    std::chrono::system_clock::time_point start_time = std::chrono::system_clock::now();

    std::vector<Cashbox> cashboxes = {};
    std::deque<Buyer> buyers = {};

    std::mt19937 gen;
    std::random_device rnd;

    Statistics &statistics;

public:

    Shop(unsigned cashbox_number,
         unsigned intake_intensity,
         unsigned service_intensity,
         unsigned average_products_number,
         unsigned max_queue_length,
         Statistics &statistics);
    void start_work();

private:

    void buyer_service(Buyer buyer);
    void deque_update();
};


