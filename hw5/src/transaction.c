#include "transaction.h"
#include "csapp.h"
#include "debug.h"

int doesNotExist(TRANSACTION *dependee, DEPENDENCY *depends)
{
    DEPENDENCY *next = depends;

    while(next != NULL)
    {
        if(next -> trans == dependee)
            return 1;
        next = next -> next;
    }

    return 0;
}

void notify_waiters(TRANSACTION *tp)
{
    while(tp -> waitcnt != 0)
    {
        pthread_mutex_lock(&tp -> mutex);
        V(&tp -> sem);
        --tp -> waitcnt;
        pthread_mutex_unlock(&tp -> mutex);
    }
}

/*
 * Initialize the transaction manager.
 */
void trans_init(void)
{
    debug("Initialize transaction manager");
    trans_list.next = &trans_list;
    trans_list.prev = &trans_list;
}

/*
 * Finalize the transaction manager.
 */
void trans_fini(void)
{
    debug("Finalize transaction manager");
    TRANSACTION *next = trans_list.next;

    while(next != &trans_list)
    {
        DEPENDENCY * nextdp = next -> depends;
        void *temp;

        while(nextdp != NULL)
        {
            temp = nextdp -> next;
            free(nextdp);
            nextdp = (DEPENDENCY*)temp;
        }

        temp = next -> next;
        free(next);
        next = (TRANSACTION*)temp;
    }
}

/*
 * Create a new transaction.
 *
 * @return  A pointer to the new transaction (with reference count 1)
 * is returned if creation is successful, otherwise NULL is returned.
 */
TRANSACTION *trans_create(void)
{
    debug("Create new transaction");
    TRANSACTION *trans = Malloc(sizeof(TRANSACTION));
    int cur_id = trans_list.next -> id;
    trans -> id = ++cur_id;
    trans -> refcnt = 1;
    trans -> status = TRANS_PENDING;
    trans -> depends = NULL;
    trans -> waitcnt = 0;
    sem_init(&(trans -> sem), 0, 0);
    pthread_mutex_init(&trans -> mutex, NULL);
    trans -> prev = trans_list.prev;
    trans -> next = &trans_list;
    trans_list.prev -> next = trans;
    trans_list.prev = trans;

    return trans;
}

/*
 * Increase the reference count on a transaction.
 *
 * @param tp  The transaction.
 * @param why  Short phrase explaining the purpose of the increase.
 * @return  The transaction pointer passed as the argument.
 */
TRANSACTION *trans_ref(TRANSACTION *tp, char *why)
{
    debug("Increase refcnt of transaction [%d]: %s", tp -> id, why);
    pthread_mutex_lock(&tp -> mutex);
    ++tp -> refcnt;
    pthread_mutex_unlock(&tp -> mutex);

    return tp;
}

/*
 * Decrease the reference count on a transaction.
 * If the reference count reaches zero, the transaction is freed.
 *
 * @param tp  The transaction.
 * @param why  Short phrase explaining the purpose of the decrease.
 */
void trans_unref(TRANSACTION *tp, char *why)
{
    debug("Decrease refcnt of transaction [%d]: %s", tp -> id, why);
    pthread_mutex_lock(&tp -> mutex);
    --tp -> refcnt;
    pthread_mutex_unlock(&tp -> mutex);
}

/*
 * Add a transaction to the dependency set for this transaction.
 *
 * @param tp  The transaction to which the dependency is being added.
 * @param dtp  The transaction that is being added to the dependency set.
 */
void trans_add_dependency(TRANSACTION *tp, TRANSACTION *dtp)
{
    if(dtp -> status == TRANS_PENDING)
    {
        debug("Add dependency [%d]", dtp -> id);
        DEPENDENCY *dependee = Malloc(sizeof(DEPENDENCY));
        dependee -> trans = dtp;
        dependee -> next = NULL;

        if(tp -> depends != NULL)
        {
            tp -> depends = dependee;
        }
        else
        {
            /* IF DEPENDEE DOES NOT ALREADY EXIST IN DEPENDS*/
            if(!doesNotExist(dtp, tp -> depends))
            {
                dependee -> next = tp -> depends;
                tp -> depends = dependee;
            }
        }

        trans_ref(dtp, "Added to a dependency list");

        /* INCREAMENT THE DEPENDEE'S WAITCNT*/
        pthread_mutex_lock(&dtp -> mutex);
        ++dtp -> waitcnt;
        pthread_mutex_unlock(&dtp -> mutex);
    }
}

/*
 * Try to commit a transaction.  Committing a transaction requires waiting
 * for all transactions in its dependency set to either commit or abort.
 * If any transaction in the dependency set abort, then the dependent
 * transaction must also abort.  If all transactions in the dependency set
 * commit, then the dependent transaction may also commit.
 *
 * In all cases, this function consumes a single reference to the transaction
 * object.
 *
 * @param tp  The transaction to be committed.
 * @return  The final status of the transaction: either TRANS_ABORTED,
 * or TRANS_COMMITTED.
 */
TRANS_STATUS trans_commit(TRANSACTION *tp)
{
    debug("Commit transaction [%d]", tp -> id);
    if(tp -> depends != NULL)
    {
        DEPENDENCY *next = tp -> depends;

        while(next != NULL)
        {
            P(&next -> trans -> sem);
            if(next -> trans -> status == TRANS_ABORTED)
            {
                tp -> status = TRANS_ABORTED;
                notify_waiters(tp);
                trans_unref(tp, "Transaction aborted");
                return TRANS_ABORTED;
            }
            next = next -> next;
        }
    }

    tp -> status = TRANS_COMMITTED;
    notify_waiters(tp);
    trans_unref(tp, "Transaction commited");

    return TRANS_COMMITTED;
}

/*
 * Abort a transaction.  If the transaction has already committed, it is
 * a fatal error and the program crashes.  If the transaction has already
 * aborted, no change is made to its state.  If the transaction is pending,
 * then it is set to the aborted state, and any transactions dependent on
 * this transaction must also abort.
 *
 * In all cases, this function consumes a single reference to the transaction
 * object.
 *
 * @param tp  The transaction to be aborted.
 * @return  TRANS_ABORTED.
 */
TRANS_STATUS trans_abort(TRANSACTION *tp)
{
    debug("Abort transaction [%d]", tp -> id);

    if(tp -> status == TRANS_COMMITTED)
    {
        fprintf(stderr,"Transaction already committed");
        trans_unref(tp, "Transaction already committed");
        exit(EXIT_FAILURE);
    }
    else if(tp -> status == TRANS_ABORTED)
    {
        tp -> status = TRANS_ABORTED;
        notify_waiters(tp);
        trans_unref(tp, "Transaction aborted");
    }

    return TRANS_ABORTED;
}

/*
 * Get the current status of a transaction.
 * If the value returned is TRANS_PENDING, then we learn nothing,
 * because unless we are holding the transaction mutex the transaction
 * could be aborted at any time.  However, if the value returned is
 * either TRANS_COMMITTED or TRANS_ABORTED, then that value is the
 * stable final status of the transaction.
 *
 * @param tp  The transaction.
 * @return  The status of the transaction, as it was at the time of call.
 */
TRANS_STATUS trans_get_status(TRANSACTION *tp)
{
    debug("Get transaction [%d] status -> %d", tp -> id, tp -> status);

    return tp -> status;
}

/*
 * Print information about a transaction to stderr.
 * No locking is performed, so this is not thread-safe.
 * This should only be used for debugging.
 *
 * @param tp  The transaction to be shown.
 */
void trans_show(TRANSACTION *tp)
{
    fprintf(stderr, "[id=%d, status=%d, refcnt=%d\n", tp -> id, tp -> status, tp -> refcnt);
}

/*
 * Print information about all transactions to stderr.
 * No locking is performed, so this is not thread-safe.
 * This should only be used for debugging.
 */
void trans_show_all(void)
{
    TRANSACTION *next = trans_list.next;

    while(next != &trans_list)
    {
        trans_show(next);
        next = next -> next;
    }
}
