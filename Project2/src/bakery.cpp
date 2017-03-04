/*
 * this file is bakery lock implementation file
 *
 *
 * @author Junsu Lim
 * @since 2016-11-03
 */

#include "bakery.h"

using namespace std;

/*
 * this is used to initialize Bakery class
 * , to set all flag - false and ticket - 0
 *
 * @param[in]   n : joined consumer's num
 */

Bakery::Bakery(int n) {
    flag_ = new bool[n];
    label_ = new int[n];
    consumer_num_ = n;

    for (int i = 0; i < n; i++) {
        flag_[i] = false;
        label_[i] = 0;
    }
}

/*
 * this is used to implement lock by bakery
 *
 * @param[in]   i : consumer's id
 */

void Bakery::lock(int i) {
    flag_[i] = true;
    label_[i] = maxTicket() + 1;

    while (check(i));
}

/*
 * this is used to implement unlock by bakery
 *
 * @param[in]   i : consumer's id
 */
void Bakery::unlock(int i) {
    flag_[i] = false;
}

/*
 * this is function that
 * in every consumer, check any consumer that
 * have intention to join and order is higher than me.
 *
 * if any consumer is here, return true
 * else return false
 *
 * @param[in]   i : consumer's id
 */
bool Bakery::check(int i) {
    // for every i
    for (int j = 0 ; j < consumer_num_ ; j++) {
        // flag i == true
        if (flag_[j] == true) {
            // and lexicographic order is higher than other
            if (label_[i] > label_[j]) {

                __sync_synchronize();
                return true;
            }
            else if (label_[i] == label_[j]) {
                if (i > j) {

                    __sync_synchronize();
                    return true;
                }
            }
        }
    }
    return false;
}

/*
 * this is function that
 * pick max number ticket of consumer
 */
int Bakery::maxTicket() {
    int max = -1;
    for (int i = 0; i < consumer_num_; i++) {
        if (label_[i] > max) {
            max = label_[i];
        }
    }
    return max;
}
