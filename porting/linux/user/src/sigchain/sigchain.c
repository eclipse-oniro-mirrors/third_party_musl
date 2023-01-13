#include <sigchain.h>
#include <locale.h>
#include <pthread.h>
#include <errno.h>
#include <stdio.h>

extern int __libc_sigaction(int sig, const struct sigaction *restrict sa,
                            struct sigaction *restrict old);

#define SIG_CHAIN_KEY_VALUE_1 1
#define SIGNAL_CHAIN_SPECIAL_ACTION_MAX 2

#if SIGCHAIN_DEBUG
#ifndef SIGCHAIN_PRINT_DEBUG
#define SIGCHAIN_PRINT_DEBUG(fmt, ...) printf("SIGCHAIN D " fmt "\n", ##__VA_ARGS__)
#endif
#else
#define SIGCHAIN_PRINT_DEBUG(fmt, ...)
#endif
#ifndef SIGCHAIN_PRINT_INFO
#define SIGCHAIN_PRINT_INFO(fmt, ...) printf("SIGCHAIN I " fmt "\n", ##__VA_ARGS__)
#endif
#ifndef SIGCHAIN_PRINT_ERROR
#define SIGCHAIN_PRINT_ERROR(fmt, ...) printf("SIGCHAIN E " fmt "\n", ##__VA_ARGS__)
#endif

struct sc_signal_chain {
    bool marked;
    struct sigaction sig_action;
    struct signal_chain_action sca_special_actions[SIGNAL_CHAIN_SPECIAL_ACTION_MAX];
};

/* Signal chain set, from 0 to 63. */
static struct sc_signal_chain sig_chains[_NSIG - 1];
/* static thread Keyword */
static pthread_key_t sigchain_key;

/**
  * @brief Get the key of the signal thread.
  * @retval int32_t, the value of the sigchain key.
  */
static pthread_key_t get_handling_signal_key() {
    static bool isCreated = false;
    if (!isCreated) {
        /* Create a thread key. */
        int rc = pthread_key_create(&sigchain_key, NULL);
        if (rc != 0) {
            SIGCHAIN_PRINT_ERROR("%s failed to create sigchain pthread key",
                    __func__,  rc);
        } else {
            isCreated = true;
            int32_t value = 0;
            pthread_setspecific(sigchain_key, &value);
        }
    }

    return sigchain_key;
}

/**
  * @brief Get the value of the sigchain key
  * @retval int32_t, the value of the sigchain key.
  */
static int32_t get_handling_signal() {
  int32_t *result = pthread_getspecific(get_handling_signal_key());
  return *result;
}

/**
  * @brief Set the value of the sigchain key
  * @param[in] value, the value of the sigchain key
  * @retval void.
  */
static void set_handling_signal(int32_t value) {
    pthread_setspecific(get_handling_signal_key(),
                      &value);
}

/**
  * @brief Judge whether the signal is marked
  * @param[in] signo, the value of the signal.
  * @retval true if the signal is marked, or false.
  */
bool ismarked(int signo)
{
    SIGCHAIN_PRINT_DEBUG("%s signo: %d", __func__, signo);
    return sig_chains[signo - 1].marked;
}

/**
  * @brief This is a callback function, which is registered to the kernel
  * @param[in] signo, the value of the signal.
  * @param[in] siginfo, the information of the signal.
  * @param[in] ucontext_raw, the context of the signal.
  * @retval void
  */
static void signal_chain_handler(int signo, siginfo_t* siginfo, void* ucontext_raw)
{
    SIGCHAIN_PRINT_DEBUG("%s signo: %d", __func__, signo);
    /* Try to call the special handlers first. */
    if (get_handling_signal() == 0){
        int len = SIGNAL_CHAIN_SPECIAL_ACTION_MAX;
        for (int i = 0; i < len; i++) {
            if (sig_chains[signo - 1].sca_special_actions[i].sca_sigaction == NULL) {
                break;
            }
            /* The native bridge signal handler might not return. */
            bool noreturn = (sig_chains[signo - 1].sca_special_actions[i].sca_flags &
                             SIGCHAIN_ALLOW_NORETURN);
            sigset_t previous_mask;
            pthread_sigmask(SIG_SETMASK, &sig_chains[signo - 1].sca_special_actions[i].sca_mask,
                            &previous_mask);

            int32_t previous_value =  get_handling_signal();
            if (!noreturn) {
                set_handling_signal(SIG_CHAIN_KEY_VALUE_1);
            }

            if (sig_chains[signo - 1].sca_special_actions[i].sca_sigaction(signo,
                                                            siginfo, ucontext_raw)) {
                set_handling_signal(previous_value);
                return;
            }

            pthread_sigmask(SIG_SETMASK, &previous_mask, NULL);
            set_handling_signal(previous_value);
        }
    }

    int sa_flags = sig_chains[signo - 1].sig_action.sa_flags;
    ucontext_t* ucontext = (ucontext_t*)(ucontext_raw);

    sigset_t mask;
    sigorset(&mask, &ucontext->uc_sigmask, &sig_chains[signo - 1].sig_action.sa_mask);

    if (!(sa_flags & SA_NODEFER)) {
        sigaddset(&mask, signo);
    }

    pthread_sigmask(SIG_SETMASK, &mask, NULL);

    if ((sa_flags & SA_SIGINFO)) {
        sig_chains[signo - 1].sig_action.sa_sigaction(signo, siginfo, ucontext_raw);
    } else {
        if (sig_chains[signo - 1].sig_action.sa_handler == SIG_IGN) {
            return;
        } else if (sig_chains[signo - 1].sig_action.sa_handler == SIG_DFL) {
            SIGCHAIN_PRINT_INFO("%s exiting due to SIG_DFL handler for signal: %d",
                    __func__, signo);
        } else {
            sig_chains[signo - 1].sig_action.sa_handler(signo);
        }
    }

    return;
}

/**
  * @brief Register the signal chain with the kernel if needed
  * @param[in] signo, the value of the signal.
  * @retval void
  */
void sigchain_register(int signo)
{
    SIGCHAIN_PRINT_DEBUG("%s signo: %d", __func__, signo);
    struct sigaction signal_action = {};
    sigfillset(&signal_action.sa_mask);

    signal_action.sa_sigaction = signal_chain_handler;
    signal_action.sa_flags = SA_RESTART | SA_SIGINFO | SA_ONSTACK;
    __libc_sigaction(signo, &signal_action, &sig_chains[signo - 1].sig_action);
}

/**
  * @brief Mark the signal to the sigchain.
  * @param[in] signo, the value of the signal.
  * @retval void
  */
void mark(int signo)
{
    SIGCHAIN_PRINT_DEBUG("%s signo: %d", __func__, signo);
    if (!sig_chains[signo - 1].marked) {
        sigchain_register(signo);
        sig_chains[signo - 1].marked = true;
    }
}

/**
  * @brief Set the action of the signal.
  * @param[in] signo, the value of the signal.
  * @param[in] new_sa, the new action of the signal.
  * @retval void
  */
void setaction(int signo, const struct sigaction *restrict new_sa)
{
    SIGCHAIN_PRINT_DEBUG("%s signo: %d", __func__, signo);
    sig_chains[signo - 1].sig_action = *new_sa;
}

/**
  * @brief Get the action of the signal.
  * @param[in] signo, the value of the signal.
  * @retval The current action of the signal
  */
struct sigaction getaction(int signo)
{
    SIGCHAIN_PRINT_DEBUG("%s signo: %d", __func__, signo);
    return sig_chains[signo - 1].sig_action;
}

/**
  * @brief Add the special handler to the sigchain.
  * @param[in] signo, the value of the signal.
  * @param[in] sa, the action with special handler.
  * @retval void
  */
void add_special_handler(int signo, struct signal_chain_action* sa)
{
    SIGCHAIN_PRINT_DEBUG("%s signo: %d", __func__, signo);
    int len = SIGNAL_CHAIN_SPECIAL_ACTION_MAX;
    for (int i = 0; i < len; i++) {
        if (sig_chains[signo - 1].sca_special_actions[i].sca_sigaction == NULL) {
            sig_chains[signo - 1].sca_special_actions[i] = *sa;
            return;
        }
    }
}

/**
  * @brief Remove the special handler from the sigchain.
  * @param[in] signo, the value of the signal.
  * @param[in] fn, the special handler of the signal.
  * @retval void
  */
void rm_special_handler(int signo, bool (*fn)(int, siginfo_t*, void*))
{
    SIGCHAIN_PRINT_DEBUG("%s signo: %d", __func__, signo);
    int len = SIGNAL_CHAIN_SPECIAL_ACTION_MAX;
    for (int i = 0; i < len; i++) {
        if (sig_chains[signo - 1].sca_special_actions[i].sca_sigaction == fn) {
            for (int j = i; j < len - 1; ++j) {
                sig_chains[signo - 1].sca_special_actions[j] = 
                        sig_chains[signo - 1].sca_special_actions[j + 1];
            }
            sig_chains[signo - 1].sca_special_actions[len - 1].sca_sigaction = NULL;
            return;
        }
    }

    SIGCHAIN_PRINT_INFO("%s failed to find special handler!. signo: %d",
            __func__, signo);
}

/**
  * @brief This is an external interface, 
  *        Mark the signal to sigchain ,add the special handler to the sigchain.
  * @param[in] signo, the value of the signal.
  * @param[in] sa, the action with special handler.
  * @retval void
  */
void add_special_signal_handler(int signo, struct signal_chain_action* sa)
{
    SIGCHAIN_PRINT_DEBUG("%s signo: %d", __func__, signo);
    if (signo <= 0 || signo >= _NSIG) {
        SIGCHAIN_PRINT_ERROR("%s Invalid signal %d", __func__, signo);
        return;
    }

    // Set the special handler.
    add_special_handler(signo, sa);
    mark(signo);
}

/**
  * @brief This is an external interface, remove the special handler from the sigchain.
  * @param[in] signo, the value of the signal.
  * @param[in] fn, the special handler of the signal.
  * @retval void
  */
void remove_special_signal_handler(int signo, bool (*fn)(int, siginfo_t*, void*))
{
    SIGCHAIN_PRINT_DEBUG("%s signo: %d", __func__, signo);
    if (signo <= 0 || signo >= _NSIG) {
        SIGCHAIN_PRINT_ERROR("%s Invalid signal %d", __func__, signo);
        return;
    }
    // remove the special handler.
    rm_special_handler(signo, fn);
}

/**
  * @brief Intercept the signal and sigaction.
  * @param[in] signo, the value of the signal.
  * @param[in] sa, the new action with the signal handler.
  * @param[out] old, the old action with the signal handler.
  * @retval true if the signal if intercepted, or false.
  */
bool intercept_sigaction(int signo, const struct sigaction *restrict sa,
                         struct sigaction *restrict old)
{
    SIGCHAIN_PRINT_DEBUG("%s signo: %d", __func__, signo);
    if (signo <= 0 || signo >= _NSIG) {
        SIGCHAIN_PRINT_ERROR("%s Invalid signal %d", __func__, signo);
        return false;
    }

    if (ismarked(signo)) {
        struct sigaction saved_action = getaction(signo);

        if (sa != NULL) {
            setaction(signo, sa);
        }
        if (old != NULL) {
            *old = saved_action;
        }
        return true;
    }

    return false;
}

/**
  * @brief Intercept the sigprocmask.
  * @param[in] how, the value of the mask operation .
  * @param[out] set, the value of the sigset.
  * @retval void.
  */
void intercept_sigprocmask(int how, sigset_t *restrict set)
{
    SIGCHAIN_PRINT_DEBUG("%s how: %d", __func__, how);
    if (get_handling_signal() != 0) {
        return;
    }

    sigset_t tmpset;
    if (set != NULL) {
        tmpset = *set;
        if (how == SIG_BLOCK || how == SIG_SETMASK) {
            for (int i = 1; i < _NSIG; ++i) {
                if (ismarked(i) && sigismember(&tmpset, i)) {
                    sigdelset(&tmpset, i);
                }
            }
        }
        *set = tmpset;
    }

    return;
}
