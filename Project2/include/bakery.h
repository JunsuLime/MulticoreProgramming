/*
 * this file is for bakery lock
 *
 * to do not implement pthread_mutex()
 *
 * @author Junsu Lim
 * @since 2016-11-03 
 */

class Bakery {
private:
    bool *flag_; // for check consumer's intention
    int *label_; // for check ticket num of consumer's ticket
    int consumer_num_;   // joined consumer number
public:
    Bakery (int n);
    void lock(int i);
    void unlock(int i);

    bool check(int i);
    int maxTicket();
};
