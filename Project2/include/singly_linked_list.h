/*
 * this file is for singly linked list
 * to satisfy project2's detail
 *
 * this is generic type so
 * function implementation must be in header file
 *
 * @author  Junsu Lim
 * @since   2016-11-01
 */


// about details of linked list
// use for 1) active thread list 2) version controller

// both have version value;

// where must be atomic opration in general
// insert : case 1 - atomic, case 2 - no need to do that
// delete : case 1 - atomic, case 2 - no need to do that


/*
 * SinglyLinkedList
 *
 * class name and function's name is clear,
 * so don't have to write about it's detail.
 */

template <typename T>
class SinglyLinkedList {

    private:
        struct node {
            T data;
            int version = -1;
            node *next;
        } *head, *tail;

    public:
        SinglyLinkedList() {
            head = new node;
            head->next = NULL;
            tail = head;
        }
        void Insert(T d, int v) {
            node *current = new node;
            current->data = d;
            current->next = NULL;
            current->version = v;

            node* t = tail;
            t->next = current; // when make atomic function, this line must sync
            tail = current; // this too.
        }
        bool DeleteByVersion (int v) {
            node *p = head->next;
            node *prev = head;
            while ( p != NULL ) {
                if ( p->version == v ) {    // or use equal function
                    prev->next = p->next; // when make atomic function, this line must sync
                    if ( p == tail ) { // this too.
                      tail = prev; // this too.
                    }
                    free(p);
                    return true;
                }
                prev = p;
                p = p->next;
            }
            return false;
        }
        void GetPrevVersions(int *list) {
            node *p = head->next;
            int i = 0;
            while (p != NULL) {
                list[i++] = p->version;
                p = p->next;
            }
        }
        T GetOldestData() {
            if (head->next == NULL) {
                return (T)head->next;
            }
            return head->next->data;
        }
        T GetOldestVersion() {
            if (head->next == NULL) {
                return (T)-1;
            }
            else (T)head->next->version;
        }
        T MakeReadView(int *v, int* update_list, int list_size) {
            node *p = head;
            while ( p ->next != NULL ) {
                if(IsInList(p->next->version, update_list, list_size) == false) {
                    if ( p->next->version >= *v) { 
                        *v = p->version;
                        return p->data;
                    }
                }
                else {
                    break;
                }
                p = p->next;
            }
            *v = p->version;
            return p->data;
        }
        bool IsInList(int item, int *update_list, int list_size) {
            for (int i = 0 ; i < list_size ; i++) {
                if (update_list[i] == -1) {
                    return false;
                }
                if (update_list[i] == item) {
                    return true;
                }
            }
            return false;
        }
	    bool IsFirst(int v) {
            node *p = head->next;
            if (p->version == v) {
                return true;
            }
            return false;
        }
        void GarbageCollect(int v) {
           node *p = head->next;
           if (p != NULL) {
               if ( p->version < v ){
                   head->next = p->next;
                   free(p);
                   return ;
               }
           }
        }
        T FindByVersion(int *v) {
            node *p = head;
            while ( p->next != NULL ) {
                if ( p->next->version >= *v ) {
                    *v = p->version;
                    return p->data;
                }
                p = p->next;
            }
            return head->next->data;
        }
        T FindCurrentVersion() {
            return tail->data;   
        }
        int FindCurrentV() {
            return tail->version;
        }
        void PrintAll() {
            node *p = head->next;
            while (p != NULL) {
                printf("v:%d ", p->version);
                p = p->next;
            }
            printf("\n");
        }
};


