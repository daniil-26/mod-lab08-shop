// Copyright 2021 GHA Test Team
#include <gtest/gtest.h>
#define private public
#include "task.h"


TEST(test1, Buyer_products_number) {
    auto buyer = Buyer(7);

    EXPECT_TRUE(buyer.products_number == 7);
}

TEST(test2, Buyer_time_counting) {
    auto buyer = Buyer(1);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    EXPECT_TRUE(buyer.time_counting() - 1000 <= 10);
}

TEST(test3, Cashbox_downtime) {
    auto statistics = Statistics(1, "");
    auto cashbox = Cashbox(1000, statistics);
    auto buyer = Buyer(1);

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    cashbox.start_service(buyer);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    EXPECT_TRUE(statistics.summary_cashbox_downtime - 500 <= 10);
}

TEST(test4, Cashbox_worktime) {
    auto statistics = Statistics(1, "");
    auto cashbox = Cashbox(2, statistics);
    auto buyer = Buyer(5);

    cashbox.start_service(buyer);
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));

    EXPECT_TRUE(statistics.summary_cashbox_worktime - 2500 <= 10);
}

TEST(test5, Shop_queue_advancement) {
    auto statistics = Statistics(1, "");
    auto shop = Shop(1, 1, 1, 1, 5, statistics);
    auto buyer = Buyer(100);

    for (int i = 0; i < 3; i++) {
        shop.buyer_service(buyer);
    }

    EXPECT_TRUE(
        !shop.cashboxes[0].is_free &&
        shop.buyers.size() == 2);
}

TEST(test6, Shop_queue_overflow) {
    auto statistics = Statistics(1, "");
    auto shop = Shop(2, 1, 1, 1, 3, statistics);
    auto buyer = Buyer(100);

    for (int i = 0; i < 8; i++) {
        shop.buyer_service(buyer);
    }

    EXPECT_TRUE(
        !shop.cashboxes[0].is_free &&
        !shop.cashboxes[1].is_free &&
        shop.buyers.size() == 3);
}

TEST(test7, Statistics_update) {
    auto statistics = Statistics(1, "");

    statistics.queue_length_update(5);
    statistics.queue_length_update(4);
    statistics.served_byers_update();
    statistics.served_byers_update();
    statistics.served_byers_update();
    statistics.unserved_byers_update();
    statistics.unserved_byers_update();
    statistics.summary_queue_time_update(100);
    statistics.summary_queue_time_update(50);
    statistics.summary_cashbox_worktime_update(70);
    statistics.summary_cashbox_worktime_update(20);
    statistics.summary_cashbox_downtime_update(90);
    statistics.summary_cashbox_downtime_update(20);

    EXPECT_TRUE(
        statistics.summary_queue_length == 9 &&
        statistics.served_byers_number == 3 &&
        statistics.unserved_byers_number == 2 &&
        statistics.summary_queue_time == 150 &&
        statistics.summary_cashbox_worktime == 90 &&
        statistics.summary_cashbox_downtime == 110);
}
