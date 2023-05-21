// Copyright 2021 GHA Test Team
#include "task.h"

/**/
int main() {
    const unsigned cashbox_number = 5;
    const unsigned intake_intensity = 3;
    const unsigned service_intensity = 1;
    const unsigned average_products_number = 8;
    const unsigned max_queue_length = 10;
    const long long unsigned sampling_time = 500;

    auto statistics = Statistics(sampling_time, "statistics");
    auto shop = Shop(cashbox_number, intake_intensity, service_intensity, average_products_number, max_queue_length, statistics);
    shop.start_work();
}
