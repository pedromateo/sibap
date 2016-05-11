// -*- mode: c++; c-basic-offset: 4; c-basic-style: bsd; -*-
/*
 *   This program is free software; you can redistribute it and/or
 *   modify
 *   it under the terms of the GNU Lesser General Public License as
 *   published by the Free Software Foundation; either version 3.0 of
 *   the License, or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this library; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *   02111-1307 USA
 *
 *   This file is part of the Cátedra-SAES work,
 *   http://catedrasaes.inf.um.es/trac/wiki/CSULog
 *
 */
// $Id: log.hpp 5886 2011-02-21 09:43:40Z dsevilla $
// $Revision: 5886 $
// $Date: 2011-02-21 10:43:40 +0100 (lun 21 de feb de 2011) $
#ifndef CSU_LOG_HPP
#define CSU_LOG_HPP

// Determine a basic 64 bit type. Any ideas on how to do this better?
namespace csu {
namespace base {
namespace log {
#include <climits>
#if INT_MAX > 2147483647
typedef int basic_id_t;
#elif LONG_MAX > 2147483647L
typedef long basic_id_t;
#else
typedef long long basic_id_t;
#endif
}
}
}

#include <sstream>
#include <string>
#include <ctime>
#include <locale>
#include <list>
#include <algorithm>
#include <cstring>
#include <memory>
#include <map>
#include <fstream>
#include <exception> // Exceptions
#include <stdexcept>
#include <iostream> // for std::cout, std::cerr, std::clog

// Use the unordered map directly if using GCC >= 4
#if defined __GNUG__
#define GCC_VERSION (__GNUC__ * 10000           \
                     + __GNUC_MINOR__ * 100     \
                     + __GNUC_PATCHLEVEL__)

/* Test for GCC >= 4.2.0 */
#if GCC_VERSION >= 40200
#include <tr1/unordered_map>
#define INNER_USE_UNORDERED_MAP
#elif defined CSU_LOG_USE_UNORDERED_MAP
#if defined _MSC_VER // TODO: look for version numbers of MS VC
#include <unordered_map>
#define INNER_USE_UNORDERED_MAP
#endif // _MSC_VER
#endif // CSU_LOG_USE_UNORDERED_MAP
#endif // __GNUG__

// Constants & implementaiton definitions
#if !defined CSU_LOG_THREAD_POOL_SIZE
#define CSU_LOG_THREAD_POOL_SIZE 43
#endif

#if !defined CSU_LOG_STREAM_POOL_SIZE
#define CSU_LOG_STREAM_POOL_SIZE 41
#endif

namespace csu {
namespace base {
namespace log {
namespace impl {

/// default null_id_value.
template <typename T>
struct null_id_value
{
    typedef T value_type;
    static const value_type value() { return 0; }
};

// define the special case of std::string. Used elsewhere
template <>
struct null_id_value<std::string>
{
    typedef std::string value_type;

    static const value_type value()
    {
        static std::string str_; // empty string
        return str_;
    }
};

}
}
}
}

namespace csu {
namespace base {
namespace log {

// TODO: Update to different typedefs when remote_controller is
// modified.

/// Static list
template <typename T>
struct static_list
{
    typedef std::list<T*> plist;

    static plist& list(T* l)
    {
        static plist ll;
        if (l)
            ll.push_back (l);
        return ll;
    }
};

namespace detail {


#if defined INNER_USE_UNORDERED_MAP

// Hashing traits. This is neccessary because some
// types given to the store are not hashable.
template <typename T>
struct hashing_traits
{
    typedef std::tr1::hash<T> type;
};

#endif

// XXX this should be the generic case, however...
// // Specialized for default_thread_id
// template<>
// struct hashing_traits<impl::default_threading_model::default_thread_id>
// {
//     typedef impl::default_threading_model::default_thread_id::hashing_t type;
// };

// G++ doesn't like so many template aliases
#if defined CSU_LOG_THREADING_BOOST
// Specialized for default_thread_id
template<>
struct hashing_traits<boost::thread::id>
{
    typedef impl::default_threading_model::default_thread_id::hashing_t type;
};
#endif

// Stable Store: a store of fixed N elements in an
// array, used in a round-robin manner
template<size_t N, typename Id, typename NullId, typename Store>
class stable_store
{
    // the store
    Store store_[N];
    // mapped elements
    Id _assoc[N];
    // counter of elements in the store (modulo N)
    size_t n_;

    // map: Id -> n
#if defined INNER_USE_UNORDERED_MAP
    typedef std::tr1::unordered_map<Id,
                                    size_t,
                                    typename hashing_traits<Id>::type> EMap;
#else
    typedef std::map<Id, size_t> EMap;
#endif
    EMap idmap_;

public:
    stable_store() : n_ (0)
    {
        std::fill (_assoc, _assoc+N, NullId::value());
    }

    inline bool exists (Id const& id) const
    {
        return idmap_.end() != idmap_.find (id);
    }

    inline Store& element_for (Id const& id)
    {
        typename EMap::iterator it;
        if (idmap_.end() == (it = idmap_.find (id)))
        {
            size_t r = n_; // opt

            // check if the element about to overwrite
            // exists in the map
            const Id& iid = _assoc[n_];
            if (!(iid == NullId::value()))
                idmap_.erase (iid);

            // And then store the new id
            idmap_[id] = n_;
            // also the assoc map
            _assoc[n_] = id;
            // modulo
            ++n_; n_ %= N;

            return store_[r];
        }

        return store_[it->second];
    }
};

// Stable Store: (specialization for one element)
template<typename Id, typename NullId, typename Store>
class stable_store<1, Id, NullId, Store>
{
    // the store
    Store store_;

public:
    stable_store()
    {
    }

    inline bool exists (Id const& id) const
    {
        return true;
    }

    inline Store& element_for (Id const&)
    {
        return store_;
    }
};

// A static stable store
template <size_t N, typename Id, typename NullId, typename Store>
struct static_store
{
    static inline Store& element_for (Id const& c)
    {
        static stable_store<N,Id,NullId,Store> inner_store;
        return inner_store.element_for (c);
    }
};

// A static stable store with element checking
template <size_t N, typename Id, typename NullId, typename Store>
struct static_store_with_check
{
    typedef std::pair<bool, Store*> result_t;

    static inline result_t check_element_for (Id const& c)
    {
        static stable_store<N,Id,NullId,Store> inner_store;
        bool exists = inner_store.exists (c);
        return std::make_pair (exists, &(inner_store.element_for (c)));
    }
};

// Ordering for levels
struct ordering
{
    template <typename ID>
    static ID& max()
    {
        static ID _o = static_cast<ID>(0);
        return ++_o;
    }
};

typedef csu::base::log::basic_id_t type_id_t;

// Type identifier class
template <typename T>
struct type_id
{
    static type_id_t id()
    {
        static char c;
        return reinterpret_cast<type_id_t>( &c );
    }
};

// Poor man's any class
struct any_t
{
    any_t()
        : store_ (0)
    {
    }

    template <typename T>
    any_t (T const& a)
    {
        store_ = new holder<T>(a);
    }

    any_t (any_t const& a)
        : store_ ( a.store_->copy())
    {
    }

    ~any_t()
    {
        delete store_;
    }

    any_t& operator=(any_t const& a)
    {
        delete store_;
        store_ = a.store_->copy();
        return *this;
    }

    template< typename T >
    any_t& operator=(T const& t)
    {
        delete store_;
        store_ = new holder<T>(t);
        return *this;
    }

    type_id_t
    type() const
    {
        if (!store_)
            return 0;
        return store_->type__id();
    }

    template <typename T>
    static T&
    any_cast(any_t& a)
    {
        if (type_id<T>::id() != a.type())
            throw 0;

        return dynamic_cast<holder<T>*>(a.store_)->v_;
    }

    template <typename T>
    static T&
    is_a(any_t const& a)
    {
        return type_id<T>::id() == a.type();
    }

    // Inner classes
    struct holder_base
    {
        virtual ~holder_base()
        {}

        virtual type_id_t type__id() const = 0;
        virtual holder_base* copy() const = 0;
    };

    template<typename T>
    struct holder : holder_base
    {
        holder (T const& v)
            : v_(v)
        {
        }

        type_id_t type__id() const
        {
            return type_id<T>::id();
        }

        holder_base* copy() const
        {
            return new holder<T>(v_);
        }

        // Value
        T v_;
    };

    // storage
    holder_base* store_;
};


} // detail
} // log
} // base
} // csu



// -*- mode: c++; c-basic-offset: 4; c-basic-style: bsd; -*-

namespace csu {
namespace base {
namespace log {
namespace impl {

#if defined CSU_LOG_THREADING_PTHREADS

#include <pthread.h>

struct pthreads_threading_model
{
    // Pthread Lock
    struct pthread_lock
    {
        pthread_mutex_t m_;

        pthread_lock()
        {
            pthread_mutex_init (&m_, NULL);
        }

        ~pthread_lock()
        {
            pthread_mutex_destroy (&m_);
        }

        inline void lock() { pthread_mutex_lock (&m_); }
        inline void unlock() { pthread_mutex_unlock(&m_); }
    };

    // PThread Thread ID
    struct pthread_thread_id
    {
        typedef pthread_t thread_id_t;

        static thread_id_t this_thread_id()
        {
            return pthread_self();
        }
    };

    typedef pthread_lock default_lock;
    typedef pthread_thread_id default_thread_id;
};

// Default threading model
typedef pthreads_threading_model default_threading_model;

// Pool constants
const size_t default_thread_pool_size = CSU_LOG_THREAD_POOL_SIZE;
const size_t default_stream_pool_size = CSU_LOG_STREAM_POOL_SIZE;

#elif defined CSU_LOG_THREADING_BOOST

#include <boost/thread/thread.hpp>

struct boost_threading_model
{
    // Boost Lock is not defined as it is equivalent to boost::mutex

    // Boost Thread ID
    struct boost_thread_id
    {
        typedef boost::thread::id thread_id_t;

        // Convert a thread id to a string, for the
        // unordered_map hashing.
        struct boost_thread_id_hash
        {
            inline size_t operator()(thread_id_t const& id) const
            {
                std::ostringstream ostr;
                ostr << id;
                std::tr1::hash<std::string> h;
                return h(ostr.str());
            }
        };
        typedef boost_thread_id_hash hashing_t;

        static thread_id_t this_thread_id()
        {
            return boost::this_thread::get_id();
        }
    };

    typedef boost::mutex default_lock;
    typedef boost_thread_id default_thread_id;
};

// Default threading model
typedef boost_threading_model default_threading_model;

// Null thread-id value
template<>
struct null_id_value<default_threading_model::default_thread_id::thread_id_t>
{
    static default_threading_model::default_thread_id::thread_id_t value()
    {
        // In the case of boost, a non-initialized thread
        // is a "not-a-thread" value (null value)
        static default_threading_model::default_thread_id::thread_id_t null_;
        return null_;
    }
};

// Pool constants
const size_t default_thread_pool_size = CSU_LOG_THREAD_POOL_SIZE;
const size_t default_stream_pool_size = CSU_LOG_STREAM_POOL_SIZE;


#elif defined CSU_LOG_THREADING_QT

#include <QMutex>
#include <QThread>

struct qt_threading_model
{
    // Qt Lock is not defined as it is equivalent to QMutex

    // Qt Thread ID
    struct qt_thread_id
    {
        typedef Qt::HANDLE thread_id_t;

        static thread_id_t this_thread_id()
        {
            return QThread::currentThreadId();
        }
    };

    typedef QMutex default_lock;
    typedef qt_thread_id default_thread_id;
};

// Default threading model
typedef qt_threading_model default_threading_model;

// Pool constants
const size_t default_thread_pool_size = CSU_LOG_THREAD_POOL_SIZE;
const size_t default_stream_pool_size = CSU_LOG_STREAM_POOL_SIZE;


#else // None of the above, no locking strategy

struct no_threading_model
{
    // None Lock (default)
    struct no_lock
    {
        inline void lock() {}
        inline void unlock() {}
    };

    // None Thread ID
    struct no_thread_id
    {
        typedef int thread_id_t;

        static thread_id_t this_thread_id() { return -1; }
    };

    typedef no_lock default_lock;
    typedef no_thread_id default_thread_id;
};

// Default threading model
typedef no_threading_model default_threading_model;

// Pool constants
// In the case of no threading, we can be more
// conservative on these numbers, as there is only one
// thread and one ostream wrapper can be shared with no
// problem but a little overhead in changing the pointer
// of a wrapper.
const size_t default_thread_pool_size = 1; // one thread
const size_t default_stream_pool_size = 1; // one stream wrapper

#endif // end of implementation-dependant part

} // impl
} // log
} // base
} // csu



// -*- mode: c++; c-basic-offset: 4; c-basic-style: bsd; -*-
namespace csu {
namespace base {
namespace log {
namespace impl {
namespace parser {

// match pair
typedef std::pair<const char*, size_t> match_pair;

////////////////////////////////////////////////////////////////////////
// parser state
template <typename SemanticState>
struct State
{
    // For reference, the SemanticState itself.
    typedef SemanticState SemType;

    // buffer
    const char* buf_;
    // actual pos
    const char* pos_;
    // buf_fer length
    size_t len_;

    // The inner semantic state.
    SemanticState& ss_;

    // State stack, for backtracking
    std::list< const char* > pos_stack_;

    // ctor
    State (SemanticState& ss, const char* b, size_t l)
        : buf_ (b), pos_ (b),  len_(l), ss_ (ss)
    {
    }

    SemanticState&
    semantic_state()
    {
        return ss_;
    }

    bool
    at_end() const
    {
        return static_cast<size_t>(pos_ - buf_) == len_;
    }

    bool
    match_at_pos_advance (char c)
    {
        if (!at_end() && (*pos_ == c))
        {
            ++pos_;
            return true;
        }
        return false;
    }

    bool
    match_at_pos (char c)
    {
        return !at_end() && (*pos_ == c);
    }

    char
    char_at_pos() const
    {
        return *pos_;
    }

    const char* pos() const
    {
        return pos_;
    }

    void
    advance()
    {
        if (!at_end())
            ++pos_;
    }

    // Common interface
    void
    push_state()
    {
        pos_stack_.push_front (pos_);
        // propagate call
        ss_.push_state();
    }

    void
    rollback()
    {
        pos_ = pos_stack_.front();
        pos_stack_.pop_front();
        // propagate call
        ss_.rollback();
    }

    void
    commit()
    {
        pos_stack_.pop_front();
        // propagate call
        ss_.commit();
    }
};

////////////////////////////////////////////////////////////////////////
// parser
// based on the wonderful yard parser http://code.google.com/p/yardparser/
template <bool b>
struct identity
{
    template <typename whatever>
    static inline bool match (whatever&)
    {
        return b;
    }
};

typedef identity<true> true__;
typedef identity<false> false__;

// lambda, empty rule
typedef true__ empty__;

template <char c>
struct char__
{
    template <typename S>
    static inline bool match (S& state)
    {
        return state.match_at_pos_advance (c);
    }
};

// NOTE: I could have implemented this thing other way
// but it would need a negative match with another Truth Environment
// or something...
template <char c>
struct notchar__
{
    template <typename S>
    static inline bool match (S& state)
    {
        if (state.at_end() || state.match_at_pos (c))
            return false;
        state.advance();
        return true;
    }
};

// character range, not inclusive
template <char c1, char c2>
struct crange__
{
    template <typename S>
    static inline bool match (S& state)
    {
        if (state.at_end())
            return false;

        char c = state.char_at_pos();
        if (c >= c1 && c < c2)
        {
            state.advance();
            return true;
        }
        return false;
    }
};

// character range, inclusive
template <char c1, char c2>
struct cirange__
{
    template <typename S>
    static inline bool match (S& state)
    {
        if (state.at_end())
            return false;

        char c = state.char_at_pos();
        if (c >= c1 && c <= c2)
        {
            state.advance();
            return true;
        }
        return false;
    }
};

// NOTE: cannot be done this way
// template <typename C0>
// struct not__
// {
//     template <typename S>
//     static inline bool match (S& state)
//     {
//         return !C0::template match (state);
//     }
// };

// Semantic Rule: for rules that want a process_match operation to be
// called in their A type. Usually tends to be the class itself, but
// I'll try different approaches using the state...
template <typename A, typename C0>
struct semantic_rule
{
    template <typename S>
    static inline bool match (S& state)
    {
        const char* p = state.pos();

        // Try the rule itself
        bool result;
        if (true == (result = C0::match (state)))
            A::process_match (state, std::make_pair (p, state.pos() - p));

        return result;
    }
};

// Ordered sequence of elements: abc
template <typename C0,
          typename C1,
          typename C2 = true__,
          typename C3 = true__,
          typename C4 = true__,
          typename C5 = true__,
          typename C6 = true__,
          typename C7 = true__,
          typename C8 = true__,
          typename C9 = true__,
          typename C10 = true__,
          typename C11 = true__,
          typename C12 = true__,
          typename C13 = true__,
          typename C14 = true__,
          typename C15 = true__,
          typename C16 = true__,
          typename C17 = true__,
          typename C18 = true__,
          typename C19 = true__>
struct seq__
{
    template <typename S>
    static inline bool match (S& state)
    {
        state.push_state();

        bool var = C0::template match (state)
            && C1::template match (state)
            && C2::template match (state)
            && C3::template match (state)
            && C4::template match (state)
            && C5::template match (state)
            && C6::template match (state)
            && C7::template match (state)
            && C8::template match (state)
            && C9::template match (state)
            && C10::template match (state)
            && C11::template match (state)
            && C12::template match (state)
            && C13::template match (state)
            && C14::template match (state)
            && C15::template match (state)
            && C16::template match (state)
            && C17::template match (state)
            && C18::template match (state)
            && C19::template match (state);
        var ? state.commit() : state.rollback();

        return var;
    }
};

// Element decission: a|b
template <typename C0,
          typename C1,
          typename C2 = false__,
          typename C3 = false__,
          typename C4 = false__,
          typename C5 = false__,
          typename C6 = false__,
          typename C7 = false__,
          typename C8 = false__,
          typename C9 = false__,
          typename C10 = false__,
          typename C11 = false__,
          typename C12 = false__,
          typename C13 = false__,
          typename C14 = false__,
          typename C15 = false__,
          typename C16 = false__,
          typename C17 = false__,
          typename C18 = false__,
          typename C19 = false__>
struct or__
{
    template <typename S>
    static inline bool match (S& state)
    {
        state.push_state();

        bool var = C0::template match (state)
            || C1::template match (state)
            || C2::template match (state)
            || C3::template match (state)
            || C4::template match (state)
            || C5::template match (state)
            || C6::template match (state)
            || C7::template match (state)
            || C8::template match (state)
            || C9::template match (state)
            || C10::template match (state)
            || C11::template match (state)
            || C12::template match (state)
            || C13::template match (state)
            || C14::template match (state)
            || C15::template match (state)
            || C16::template match (state)
            || C17::template match (state)
            || C18::template match (state)
            || C19::template match (state);

        var ? state.commit() : state.rollback();

        return var;
    }
};

// One or more repetitions: a+
template <typename C0>
struct plus__
{
    template <typename S>
    static inline bool match (S& state)
    {
        if (!C0::match (state))
            return false;
        while (C0::match (state))
            ;
        return true;
    }
};

// Zero or more repetitions: a*
template <typename C0>
struct star__
{
    template <typename S>
    static inline bool match (S& state)
    {
        while (C0::match (state))
            ;
        return true;
    }
};

// Optional (special star__ case): a?
template <typename C0>
struct opt__
{
    template <typename S>
    static inline bool match (S& state)
    {
        C0::match (state);
        return true;
    }
};

} // parser
} // impl
} // log
} // base
} // csu


// -*- mode: c++; c-basic-offset: 4; c-basic-style: bsd; -*-
namespace csu {
namespace base {
namespace log {
namespace impl {
namespace s_exp {

// fwd
struct s_val;

typedef std::list<const s_val*> s_list;

namespace detail
{
template <typename T>
void deletor (const T* t)
{
    delete t;
}
}

struct s_val
{
    enum _type
    {
        UNDEF, // Not defined
        SYMBOL, // Symbol
        BOOL, // Boolean value
        STRING, // String value
        INT,    // Integer value
        LIST    // A list
    };

    _type t;

    union
    {
        char* s; // STRING, SYMBOL
        bool b; // BOOL
        int i;  // INT
        s_list* l; // LIST
    } d;

    // ctor
    s_val()
        : t (UNDEF)
    {
        d.s = 0; d.b = false; d.i = 0; d.l = NULL;
    }

    s_val (_type _t)
        : t (_t)
    {
        d.s = 0; d.b = false; d.i = 0; d.l = NULL;
    }

    explicit s_val (int i)
        :t (INT)
    {
        d.i = i;
    }

    explicit s_val (bool b)
        :t (BOOL)
    {
        d.b = b;
    }

    explicit s_val (const char* s)
        :t (STRING)
    {
        d.s = ::strdup (s);
    }

    // copy
    s_val(const s_val& s)
    {
        t = s.t;
        d.s = 0; d.b = false; d.i = 0; d.l = NULL;

        switch (t)
        {
        case STRING:
        case SYMBOL:
            d.s = ::strdup (s.d.s);
            break;
        case LIST:
            d.l = new s_list;
            for (s_list::const_iterator it = s.d.l->begin();
                 it != s.d.l->end();
                 ++it)
                d.l->push_back (new s_val (**it));
            break;
        case BOOL:
            d.b = s.d.b;
            break;
        case INT:
            d.i = s.d.i;
            break;
        default:
            break;
        }
    }

    // dtor
    ~s_val()
    {
        switch (t)
        {
        case STRING:
        case SYMBOL:
            ::free (d.s);
            break;
        case LIST:
            std::for_each (d.l->begin(), d.l->end(), detail::deletor<s_val>);
            delete d.l;
            break;
        default:
            break;
        }
    }

    // Is this value primitive?
    static inline bool is_primitive(const s_val* s)
    {
        _type t = s->t;
        return t == BOOL || t == INT || t == STRING;
    }

    // Is this value compound?
    static inline bool is_compound(const s_val* s)
    {
        _type t = s->t;
        return t != UNDEF && !is_primitive (s);
    }
};


// s-exp semantic state
struct SemanticState
{
    // element stack
    typedef std::list<s_val> tok_stack_t;
    tok_stack_t tok_stack_;
    // State stack, for backtracking
    std::list< tok_stack_t::iterator > pos_stack_;

    void
    push_tok (s_val const& v)
    {
        tok_stack_.push_front (v);
    }

    tok_stack_t&
    tok_stack()
    {
        return tok_stack_;
    }

    // Common interface
    void
    push_state()
    {
        pos_stack_.push_front (tok_stack_.begin());
    }

    void
    rollback()
    {
        tok_stack_t::iterator& ppp = pos_stack_.front();
        // Remove from begin() to ppp.second
        tok_stack_.erase (tok_stack_.begin(), ppp);
        pos_stack_.pop_front();
    }

    void
    commit()
    {
        pos_stack_.pop_front();
    }
};


// or
struct s_op_or
{
    static inline s_val* update (const s_val* new_val, s_val* previous)
    {
        previous->t = s_val::BOOL;
        previous->d.b = previous->d.b || new_val->d.b;
        return previous;
    }
    static inline bool may_continue (const s_val* v)
    {
        return !v->d.b;
    }
};

// and
struct s_op_and
{
    static inline s_val* update (const s_val* new_val, s_val* previous)
    {
        // if the value has not been initialized, put it to true to
        // allow the and logic.
        if (previous->t == s_val::UNDEF)
            previous->d.b = true;
        previous->t = s_val::BOOL;
        previous->d.b = previous->d.b && new_val->d.b;
        return previous;
    }
    static inline bool may_continue (const s_val* v)
    {
        return v->d.b;
    }
};

// not
struct s_op_not
{
    static inline s_val* update (const s_val* new_val, s_val* previous)
    {
        previous->t = s_val::BOOL;
        previous->d.b = !new_val->d.b;
        return previous;
    }
    static inline bool may_continue (const s_val*)
    {
        return false;
    }
};

// Generic operator. OP is supposed to support the update operation as
// follows:
// static inline s_val* update (const s_val* new_val, s_val* previous);
// and:
// static inline bool continue(const s_val* val);
template<typename OP>
struct generic_operator
{
    static inline const s_val* apply (const s_val* _l)
    {
        s_val* s = new s_val;

        // Get the inner list. Check that _l is actually a list
        s_list* l = _l->d.l;

        // skip the first element. it would be the operator itself
        s_list::const_iterator it = l->begin();
        ++it;

        while (it != l->end())
        {
            s = OP::update (*it ,s);
            if (!OP::may_continue(s))
                break;
            ++it;
        }

        return s;
    }
};

// <
struct s_op_less
{
    static inline bool calc (const s_val* l_val, const s_val* r_val)
    {
        switch(l_val->t)
        {
        case s_val::STRING:
            return strcmp (l_val->d.s, r_val->d.s) < 0;
            break;
        case s_val::INT:
            return l_val->d.i < r_val->d.i;
            break;
        case s_val::BOOL:
            return l_val->d.b < r_val->d.b;
            break;
        default:
            return false;
        }
    }
};

// >
struct s_op_greater
{
    static inline bool calc (const s_val* l_val, const s_val* r_val)
    {
        switch(l_val->t)
        {
        case s_val::STRING:
            return strcmp (l_val->d.s, r_val->d.s) > 0;
            break;
        case s_val::INT:
            return l_val->d.i > r_val->d.i;
            break;
        case s_val::BOOL:
            return l_val->d.b > r_val->d.b;
            break;
        default:
            return false;
        }
    }
};

// =
struct s_op_equal
{
    static inline bool calc (const s_val* l_val, const s_val* r_val)
    {
        switch(l_val->t)
        {
        case s_val::STRING:
            return !strcmp (l_val->d.s, r_val->d.s);
            break;
        case s_val::INT:
            return l_val->d.i == r_val->d.i;
            break;
        case s_val::BOOL:
            return l_val->d.b == r_val->d.b;
            break;
        default:
            return false;
        }
    }
};

// >=
struct s_op_greater_equal
{
    static inline bool calc (const s_val* l_val, const s_val* r_val)
    {
        return s_op_greater::calc (l_val, r_val)
            || s_op_equal::calc (l_val, r_val);
    }
};

// <=
struct s_op_less_equal
{
    static inline bool calc (const s_val* l_val, const s_val* r_val)
    {
        return s_op_less::calc (l_val, r_val)
            || s_op_equal::calc (l_val, r_val);
    }
};


// Generic operator. OP is supposed to support the update operation as
// follows:
// static inline bool calc (const s_val* l_val, const s_val* r_val);
template<typename OP>
struct generic_relational_operator
{
    static inline const s_val* apply (const s_val* _l)
    {
        s_val* s = new s_val;

        // Get the inner list. Check that _l is actually a list
        s_list* l = _l->d.l;

        // skip the first element. it would be the operator itself
        s_list::const_iterator it = l->begin();
        ++it;

        s->t = s_val::BOOL;
        s->d.b = false;
        if (it != l->end())
        {
            s_list::const_iterator it2 = it;
            ++it2;
            if (it2 != l->end())
                s->d.b = OP::calc (*it, *it2);
        }

        return s;
    }
};

// s_exp
typedef s_val s_exp;

// apply func.
typedef const s_val* (*apply_f)(const s_val*);

// Symbol. Either operator or an aliased s_val
struct s_symbol
{
    s_symbol()
        :  ownership_ (false), af (NULL), aliased_val (NULL)
    {
    }

    s_symbol (apply_f _af)
    {
        af = _af;
        aliased_val = NULL;
        ownership_ = false;
    }

    s_symbol (const s_val* s)
    {
        af = NULL;
        aliased_val = s;
        ownership_ = true;
    }

private:
    s_symbol& _copy(s_symbol const& s)
    {
        af = s.af;
        aliased_val = s.aliased_val;
        ownership_ = s.ownership_;

        // other won't delete
        s_symbol& ss = const_cast<s_symbol&> (s);
        ss.ownership_ = false;

        return *this;
    }

public:
    s_symbol& operator=(s_symbol const& s)
    {
        if (ownership_)
            delete aliased_val;

        return _copy (s);
    }

    s_symbol (s_symbol const& s)
    {
        _copy (s);
    }

    ~s_symbol()
    {
        if (ownership_)
            delete aliased_val;
    }

    bool ownership_; // ownership?
    apply_f af; // operator
    const s_val* aliased_val; // other
};

// Evaluation exception
struct eval_exception : public std::runtime_error
{
public:
    explicit eval_exception(const std::string& what)
        : std::runtime_error(what)
    {}

    virtual ~eval_exception() throw() {}
};

// Evaluation environment. The eval function is attempted within an
// evaluation environment that maps symbol names to s_vals or to
// operations, depending on the position within the list.
struct eval_environment
{
    // name -> symbol
    typedef std::pair<std::string, s_symbol> env_symbol;

    // map, actually
    typedef std::map<std::string, s_symbol> EM;
    EM env_map_;

    eval_environment()
    {
    }

    apply_f
    op_func (const char* o) const
    {
        EM::const_iterator it = env_map_.find (std::string (o));
        if (it != env_map_.end())
            return it->second.af;
        return NULL;
    }

    const s_val*
    alias_val (const char* str) const
    {
        EM::const_iterator it = env_map_.find (std::string (str));
        if (it != env_map_.end())
            return new s_val (*(it->second.aliased_val));
        return NULL;
    }

    eval_environment&
    add_symbol (const char* str, s_symbol const& s)
    {
        // In the special case the symbol still exists, delete it
        EM::iterator it = env_map_.find (std::string (str));
        if (it != env_map_.end())
            it->second = s;
        else
            env_map_ [std::string (str)] = s;
        return *this;
    }

    eval_environment&
    delete_symbol (const char* str)
    {
        // If the symbol exists, delete it.
        EM::iterator it = env_map_.find (std::string (str));
        if (it != env_map_.end())
            env_map_.erase (it);
        return *this;
    }
};

// Eval an expression to its more simplified form.
// It is inefficient, but it works :)
struct evaluator
{
    template <typename Env>
    static inline const s_val*
    eval (const s_val* s, Env const& ev) throw (eval_exception)
    {
        const s_val* _tmp_op;
        const s_val* _tmp2;
        s_val* _tmp;
        int i;
        apply_f f; // op apply func.

        switch (s->t)
        {
        case s_val::BOOL:
        case s_val::STRING:
        case s_val::INT:
            return new s_val (*s);
            break;  // well...

        case s_val::SYMBOL:
            // look for an alias in the environment
            _tmp2 = ev.alias_val (s->d.s);
            return _tmp2 ? _tmp2 : new s_val (*s);
            break;

        case s_val::LIST:
            _tmp = new s_val;
            _tmp->t = s_val::LIST;
            _tmp->d.l = new s_list;

            i = 0;
            for (s_list::const_iterator it = s->d.l->begin();
                 it != s->d.l->end();
                 ++it, ++i)
            {
                const s_val* inner;

                switch ((*it)->t)
                {
                case s_val::LIST:
                    inner = eval (*it, ev);
                    break;

                    // look again for an alias if it is not the first
                    // element (the operator, supposedly)
                case s_val::SYMBOL:
                    inner = NULL;
                    if (i)
                        inner = ev.alias_val ((*it)->d.s);
                    if (!inner)
                        inner = new s_val (**it);
                    break;

                default:
                    inner = new s_val (**it);
                    break;
                }

                _tmp->d.l->push_back (inner);
            }

            // Now: apply the operator. It should be the first one in the
            // list
            _tmp_op = *(_tmp->d.l->begin());

            // apply
            f = ev.op_func (_tmp_op->d.s);
            if (!f)
            {
                // cleanup
                delete _tmp;
                throw eval_exception (std::string ("First element in "
                                                   "list is not an operator"));
            }
            _tmp2 = f (_tmp);
            delete _tmp;

            return _tmp2;
            break;

        default:
            break;
        }

        // never reached
        return NULL;
    }

    // Simplify is like eval, but it does not evaluate symbols. Only
    // lists composed of an operator and a series of terminals
    template <typename Env>
    static inline const s_val*
    simplify (const s_val* s, Env const& ev)  throw (eval_exception)
    {
        const s_val* _tmp_op;
        const s_val* _tmp2;
        s_val* _tmp;
        int i;
        apply_f f; // op apply func.

        switch (s->t)
        {
        case s_val::BOOL:
        case s_val::STRING:
        case s_val::INT:
            return new s_val (*s);
            break;  // No simplification possible

        case s_val::SYMBOL:
            // look for an alias in the environment
            _tmp2 = ev.alias_val (s->d.s);
            return _tmp2 ? _tmp2 : new s_val (*s);
            break;

        case s_val::LIST:
            _tmp = new s_val;
            _tmp->t = s_val::LIST;
            _tmp->d.l = new s_list;

            i = 0;
            for (s_list::const_iterator it = s->d.l->begin();
                 it != s->d.l->end();
                 ++it, ++i)
            {
                const s_val* inner;

                switch ((*it)->t)
                {
                case s_val::LIST:
                    inner = simplify (*it, ev);
                    break;

                    // look again for an alias if it is not the first
                    // element (the operator, supposedly)
                case s_val::SYMBOL:
                    inner = NULL;
                    if (i)
                        inner = ev.alias_val ((*it)->d.s);
                    if (!inner)
                        inner = new s_val (**it);
                    break;

                default:
                    inner = new s_val (**it);
                    break;
                }

                _tmp->d.l->push_back (inner);
            }

            // Now: apply the operator. It should be the first one in the
            // list. Difference with eval: do not apply the operator
            // unless the list has only primitive data
            {
                s_list::const_iterator it = _tmp->d.l->begin(),
                    end = _tmp->d.l->end();
                _tmp_op = *it;
                // Fail gracefuly
                if (_tmp_op->t != s_val::SYMBOL)
                {
                    // cleanup
                    delete _tmp;
                    throw eval_exception (std::string ("Operator must be the "
                                                       "first symbol in an ev"
                                                       "aluation list"));
                }
                ++it;

                // Only primitive values?
                if (end == std::find_if (it, end,
                                         std::ptr_fun (s_val::is_compound)))
                {
                    // apply
                    f = ev.op_func (_tmp_op->d.s);
                    // Fail gracefuly
                    if (!f)
                    {
                        // cleanup
                        delete _tmp;
                        throw eval_exception (std::string ("First element in "
                                                           "list is not an op"
                                                           "erator"));
                    }
                    _tmp2 = f (_tmp);
                    delete _tmp;
                }
                else
                    _tmp2 = _tmp;
            }

            return _tmp2;
            break;

        default:
            break;
        }

        // never reached
        return NULL;
    }
};

/// Simplified s-exp grammar
namespace grammar
{
using namespace parser;

// space
struct space : or__ < char__<' '>, char__<'\t'>, char__<'\n'>, char__<'\r'> >
{
};

// One or more spaces
struct spaces : plus__ < space >
{
};

// identifier
struct idchars_start : or__ <
    cirange__<'a', 'z'>,
    cirange__<'A', 'Z'>,
    char__<'?'>, char__<'@'>, char__<'&'>, char__<'%'>,
    char__<'$'>, char__<'!'>, char__<'\\'>, char__<'~'>,
    char__<'='>, char__<'*'>, char__<'_'>, char__<'+'>,
    char__<'<'>, char__<'>'> >
{
};

struct idchars : or__ < idchars_start,
                        cirange__<'0', '9'>,
                        char__<'#'>,
                        char__<'-'> >
{
};

// identifier
struct id :
        semantic_rule <id,
                       seq__ < idchars_start, star__< idchars > > >
{
    // Semantic rule
    template <typename S>
    static inline void process_match (S& state, match_pair const& mp)
    {
        s_val s (s_val::SYMBOL);
        s.d.s = static_cast<char*>( ::malloc (mp.second + 1));
        strncpy (s.d.s, mp.first, mp.second);
        s.d.s[mp.second] = '\0';
        state.semantic_state().push_tok (s);
    }
};

// bool
struct bool_ :
        semantic_rule < bool_,
                        seq__ < char__<'#'>, or__ < char__<'t'>, char__< 'f' > > > >
{
    // Semantic rule
    template <typename S>
    static inline void process_match (S& state, match_pair const& mp)
    {
        s_val s (s_val::BOOL);
        s.d.b = *(mp.first + 1) == 't';
        state.semantic_state().push_tok (s);
    }
};

// int
struct int_ :
        semantic_rule < int_,
                        seq__ < opt__ < char__<'-'> >, plus__< cirange__ <'0', '9'> > > >
{
    // Semantic rule
    template <typename S>
    static inline void process_match (S& state, match_pair const& mp)
    {
        s_val s (s_val::INT);
        s.d.i = atoi (mp.first); // FIXME atoi?
        state.semantic_state().push_tok (s);
    }
};

// string
struct string_ :
        semantic_rule< string_,
                       seq__ < char__ <'"'>,
                               star__ < notchar__<'"'> > ,
                               char__ <'"'> > >
{
    // Semantic rule
    template <typename S>
    static inline void process_match (S& state, match_pair const& mp)
    {
        s_val s (s_val::STRING);
        s.d.s = static_cast<char*>( ::malloc (mp.second - 1));
        strncpy (s.d.s, mp.first + 1, mp.second - 2); // skip ""
        s.d.s[mp.second-1] = '\0';
        state.semantic_state().push_tok (s);
    }
};

// lists
// start
struct list_start :
        semantic_rule<list_start,
                      or__ < char__<'('>, char__<'['> > >
{
    // Semantic rule
    template <typename S>
    static inline void process_match (S& state, match_pair const&)
    {
        // Push an empty list
        s_val s; // UNDEF
        state.semantic_state().push_tok (s);
    }
};

// end
struct list_end : or__ <char__<')'>, char__<']'> >
{
};

// fwd, as a list is also an atom
struct list;

// atom, also a list, of course
struct atom : or__ < id , bool_, int_, string_, list>
{
};

// list body
struct list_body : seq__ <
    opt__ <spaces>,
    or__ < seq__ <atom, star__< seq__< spaces, atom> > >,
           empty__>,
    opt__ <spaces> >
{
};

struct list: semantic_rule< list,
                            seq__<list_start, list_body, list_end> >
{
    // Semantic rule
    template <typename S>
    static inline void process_match (S& state, match_pair const&)
    {
        // OK, walk the list till we find a list element
        typename S::SemType::tok_stack_t& vs =
            state.semantic_state().tok_stack();
        typename S::SemType::tok_stack_t::iterator it = vs.begin();

        // List that will take part on the s_val
        s_list* tmp_lst = new s_list;

        // List element is marked as UNDEF on purpose
        while (it->t != s_val::UNDEF)
            tmp_lst->push_front (new s_val (*it++));
        s_val& tmp_v = *it;

        tmp_v.d.l = tmp_lst;

        // remove tok_stack elements that take part on the list.
        vs.erase (vs.begin(), it);

        // Set finally this one as a list.
        tmp_v.t = s_val::LIST;
    }
};

struct gram : or__<bool_, list>
{
};

} // grammar
} // s_exp
} // impl
} // log
} // base
} // csu


// -*- mode: c++; c-basic-offset: 4; c-basic-style: bsd; -*-
namespace csu {
namespace base {
namespace log {
namespace impl {

// scoped lock (RAII)
template<typename Lock>
class scoped_lock
{
private:
    Lock& l_;

    scoped_lock( scoped_lock const & );
    scoped_lock & operator=( scoped_lock const & );

public:
    scoped_lock( Lock& l ) : l_ (l)
    {
        l_.lock();
    }

    ~scoped_lock()
    {
        l_.unlock();
    }
};


//
// ostream thread-safe wrapper
//
template <typename Lock>
class ostream_ts_wrapper
{
private:
    ostream_ts_wrapper (ostream_ts_wrapper const &);
    ostream_ts_wrapper& operator= (ostream_ts_wrapper const &);

public:
    ostream_ts_wrapper() : stream_ (0)
    {}

    ostream_ts_wrapper& set_ostream(std::ostream* o)
    {
        stream_ = o;
        return *this;
    }

    ///
    /// thread safe dumping
    ostream_ts_wrapper& operator<< (const std::string& s)
    {
        // dump the string into the main ostream, wrap
        // around mutex_.
        scoped_lock<Lock> sl (mutex_);
        *stream_ << s << ::std::flush;

        return *this;
    }

private:
    Lock mutex_;
    std::ostream* stream_;
};

template <typename Lock>
class ots_manager
{
public:
    static ostream_ts_wrapper<Lock>&
    ostream_ts_wrapper_for(std::ostream& os)
    {
        static Lock mutex_;
        scoped_lock<Lock> sl (mutex_);

        std::ptrdiff_t v = reinterpret_cast<std::ptrdiff_t>(&os);

        return detail::static_store<impl::default_stream_pool_size,
            std::ptrdiff_t,
            null_id_value<std::ptrdiff_t>,
            ostream_ts_wrapper<Lock> >
            ::element_for (v).set_ostream (&os);
    }
};

} // impl
} // log
} // base
} // csu



namespace csu {
namespace base {
namespace log {


/// Levels

// level_id_t
typedef csu::base::log::basic_id_t level_id_t;
// order_id_t
typedef size_t level_order_t;

/// Information of a level
struct level_info
{
    level_id_t id_; // this a pointer to the level itself.
    level_order_t order_;
    std::string name_;
    bool enabled_;  // enabled?
};

/// Level representation
struct level
{
    // default ctor.
    level()
    {
        d.order_ = detail::ordering::max<level_id_t>();
        _init();
    }

    level(const char* s)
    {
        d.order_ = detail::ordering::max<level_id_t>();
        d.name_ = s;
        _init();
    }

    level(level_id_t order, const char* s)
    {
        d.order_ = order;
        d.name_ = s;
        _init();
    }

    void enable()
    {
        d.enabled_ = true;
    }

    void disable()
    {
        d.enabled_ = false;
    }

    // d-pointer
    level_info d;

private:
    void _init()
    {
        d.id_ = reinterpret_cast<level_id_t>(this);
        d.enabled_ = true;
        static_list<level_info>::list (&d);
    }
};


// group_id_t
typedef csu::base::log::basic_id_t group_id_t;

/// Information of a group
struct group_info
{
    group_id_t id_; // this a pointer to the group itself.
    std::string name_;
    bool enabled_;  // enabled?
};

// Group representation
struct group
{
    // default ctor.
    group()
    {
        _init();
    }

    group(const char*s)
    {
        d.name_ = s;
        _init();
    }

    void enable()
    {
        d.enabled_ = true;
    }

    void disable()
    {
        d.enabled_ = false;
    }

    // d-pointer
    group_info d;

private:
    void _init()
    {
        d.id_ = reinterpret_cast<group_id_t>(this);
        d.enabled_ = true;
        static_list<group_info>::list (&d);
    }
};

}
}
}

// -*- mode: c++; c-basic-offset: 4; c-basic-style: bsd; -*-
namespace csu {
namespace base {
namespace log {
namespace impl {

/// Level filter
struct level_filter
{
    std::string filter_;
    level_filter()
    {
    }

    level_filter (const char* s)
        : filter_ (s)
    {
    }

    level_filter (const std::string& s)
        : filter_ (s)
    {
    }
};

/// Group filter
struct group_filter
{
    std::string filter_;
    group_filter()
    {
    }

    group_filter (const char* s)
        : filter_ (s)
    {
    }

    group_filter (const std::string& s)
        : filter_ (s)
    {
    }
};

template <typename GoL>
struct level_group_trait
{
};

template <>
struct level_group_trait<level>
{
    typedef level_order_t id_t;
    typedef level_filter filter_t;
    typedef level_info info_t;

    static inline id_t value(info_t const& li)
    {
        return li.order_;
    }
};

template <>
struct level_group_trait<group>
{
    typedef group_id_t id_t;
    typedef group_filter filter_t;
    typedef group_info info_t;

    static inline id_t value(info_t const& gi)
    {
        return gi.id_;
    }
};


// Filter applicators
template <typename GoL>
struct filter_applicator
{
    s_exp::eval_environment ee_;
    const s_exp::s_val* exp_;
    bool environment_initted;
    bool exp_simplified;
    typedef level_group_trait<GoL> trait;

#if defined INNER_USE_UNORDERED_MAP
    typedef std::tr1::unordered_map<typename trait::id_t,
                                    bool> result_cache_t;
#else
    typedef std::map<typename trait::id_t, bool> result_cache_t;
#endif
    result_cache_t result_cache_;

    filter_applicator()
        : exp_ (NULL), environment_initted (false), exp_simplified (false)
    {
    }

    ~filter_applicator()
    {
        delete exp_;
    }

    // Update the exp given a filter (string)
    bool
    update(const typename trait::filter_t& f)
    {
        // Parse the s-exp. If a false is returned, make exp_=0 to
        // signal an invalid filter (equal to true, #t)
        delete exp_;
        exp_ = NULL;
        exp_simplified = false;

        s_exp::SemanticState ss;
        parser::State<s_exp::SemanticState> s
            (ss, f.filter_.c_str(), f.filter_.size());
        bool match = s_exp::grammar::gram::match(s);
        if (match)
            exp_ = new s_exp::s_val (ss.tok_stack().front());

        result_cache_.clear();
        return match;
    }

    // Eval the exp with a given level
    inline bool
    check (typename trait::id_t o)
    {
        // If nothing set, allow all.
        if (!exp_)
            return true;

        // First of all, check if this case has been evaluated before
        // using the result_cache_
        typename result_cache_t::const_iterator it;
        if ((it = result_cache_.find (o)) != result_cache_.end())
            return it->second;

        // Trick to init the enviroment on the first check
        // requested. All levels are supposed to have been declared
        // before.
        if (!environment_initted)
        {
            using namespace s_exp;

            ee_.add_symbol ("or", s_symbol (generic_operator<s_op_or>::apply))
                .add_symbol ("and", s_symbol (generic_operator<s_op_and>::apply))
                .add_symbol ("not", s_symbol (generic_operator<s_op_not>::apply))
                .add_symbol ("!", s_symbol (generic_operator<s_op_not>::apply))
                .add_symbol ("<", s_symbol (generic_relational_operator<s_op_less>::apply))
                .add_symbol (">", s_symbol (generic_relational_operator<s_op_greater>::apply))
                .add_symbol (">=", s_symbol (generic_relational_operator<s_op_greater_equal>::apply))
                .add_symbol ("<=", s_symbol (generic_relational_operator<s_op_less_equal>::apply))
                .add_symbol ("=", s_symbol (generic_relational_operator<s_op_equal>::apply));

            // init the environment with the available levels (using
            // their order)
            typename csu::base::log::static_list<typename trait::info_t>::plist& g =
                csu::base::log::static_list<typename trait::info_t>::list (NULL);
            typename csu::base::log::static_list<typename trait::info_t>::plist::const_iterator lit =
                g.begin();
            while (lit != g.end())
            {
                // Assign SYMBOL -> order (for levels)
                ee_.add_symbol ((*lit)->name_.c_str(),
                                s_symbol (new s_val ((int)trait::value (**lit))));
                ++lit;
            }

            environment_initted = true;
        }

        // Simplify the expression (only once)
        if (!exp_simplified)
        {
            // If the expression cannot be simplified, just delete it
            try
            {
                const s_exp::s_val* opt =
                    s_exp::evaluator::simplify<s_exp::eval_environment>(exp_, ee_);
                if (opt)
                {
                    delete exp_;
                    exp_ = opt;
                }
            } catch (...) // eval_exception
            {
                delete exp_;
                exp_ = NULL;
                return true; // The next call won't work, and it will
                // be only reset with a new expression
            }

            exp_simplified = true;
        }

        // Add the current order as the special "_" parameter
        ee_.add_symbol ("_", s_exp::s_symbol (new s_exp::s_val ((int)o)));

        std::auto_ptr <const s_exp::s_val> result_v (NULL);
        try
        {
            result_v
                .reset (s_exp::evaluator::eval<s_exp::eval_environment>(exp_, ee_));
        } catch (...) // eval_exception
        {
            result_v.reset (NULL);
        }

        // Avoid a latter expression optimization to rely on this value
        ee_.delete_symbol ("_");

        bool result;
        if ((result_v.get() == NULL) || result_v->t != s_exp::s_val::BOOL)
            result = true; // Expression does not evaluate well: invalid
        else
            result = result_v->d.b;

        result_cache_[o] = result;
        return result;
    }
};


} // impl
} // log
} // base
} // csu


namespace csu {
namespace base {
namespace log {

/// logger ID typedef
typedef csu::base::log::basic_id_t logger_id_t;
/// Logger data. Inner logger configurable data
struct logger_state
{
    // Enabled by default
    logger_state()
        : enabled (true)
    {
    }

    // log ID (it is a pointer)
    logger_id_t id;

    // log name
    std::string name;

    // log format holder
    std::string log_format;

    // enabled?
    bool enabled;

    // s-expr
    impl::level_filter level_s_exp;

    // s-expr
    impl::group_filter group_s_exp;

    // Logger streams
    typedef std::list<std::ostream*> OstreamList;
    OstreamList streams;
};

} // log
} // base
} // csu

namespace csu {
namespace base {
namespace log {
namespace conf {

// internal type to descbribe the static store of the
// configuration. 100 logs max.
typedef csu::base::log::detail::
    static_store_with_check<100,
                            std::string,
                            impl::null_id_value<std::string>,
                            logger_state> logger_store_t;

namespace parser
{
using namespace csu::base::log::impl::parser;

// space
struct space : or__ < char__<' '>, char__<'\t'>, char__<'\n'>, char__<'\r'> >
{
};

struct spaces : plus__<space>
{
};

// Comment
struct comment_p : seq__ < char__<'#'>, star__< notchar__<'\n'> >, char__<'\n'> >
{
};

struct ignore_p : star__< or__ < comment_p, spaces> >
{
};

struct true_p : seq__ < char__<'t'>, char__<'r'>, char__<'u'>, char__<'e'> >
{
};

struct false_p : seq__ < char__<'f'>, char__<'a'>, char__<'l'>, char__<'s'>, char__<'e'> >
{
};

struct boolean_p : semantic_rule < boolean_p,
                                   or__ < true_p,
                                          false_p,
                                          char__ <'0'>,
                                          char__< '1'> > >
{
    // Semantic rule
    template <typename S>
    static inline void process_match (S& state, match_pair const& mp)
    {
        bool b;
        b = mp.first[0] == 't' || mp.first[0] == '1'; // I know, ugly hack
        state.semantic_state().push_tok (b);
    }
};

// enabled?
struct enabled_p : seq__ < char__ <'e'>,
                           char__ <'n'>,
                           char__ <'a'>,
                           char__ <'b'>,
                           char__ <'l'>,
                           char__ <'e'>,
                           char__ <'d'> >
{
};

struct level_filter_p :  seq__ < char__ <'l'>,
                                 char__ <'e'>,
                                 char__ <'v'>,
                                 char__ <'e'>,
                                 char__ <'l'>,
                                 char__ <'_'>,
                                 char__ <'f'>,
                                 char__ <'i'>,
                                 char__ <'l'>,
                                 char__ <'t'>,
                                 char__ <'e'>,
                                 char__ <'r'> >
{
};

struct group_filter_p :  seq__ < char__ <'g'>,
                                 char__ <'r'>,
                                 char__ <'o'>,
                                 char__ <'u'>,
                                 char__ <'p'>,
                                 char__ <'_'>,
                                 char__ <'f'>,
                                 char__ <'i'>,
                                 char__ <'l'>,
                                 char__ <'t'>,
                                 char__ <'e'>,
                                 char__ <'r'> >
{
};

struct log_format_p :  seq__ < char__ <'l'>,
                               char__ <'o'>,
                               char__ <'g'>,
                               char__ <'_'>,
                               char__ <'f'>,
                               char__ <'o'>,
                               char__ <'r'>,
                               char__ <'m'>,
                               char__ <'a'>,
                               char__ <'t'> >
{
};

struct string_p : semantic_rule<string_p,
                                seq__ < char__ <'"'>,
                                        star__ < notchar__<'"'> > ,
                                        char__ <'"'> > >
{
    // Semantic rule
    template <typename S>
    static inline void process_match (S& state, match_pair const& mp)
    {
        std::string s (mp.first + 1, mp.second - 2); // remove the "
        state.semantic_state().push_tok (s);
    }
};

///
/// streams (and streams sub options) string
struct streams_p :  seq__ < char__ <'s'>,
                            char__ <'t'>,
                            char__ <'r'>,
                            char__ <'e'>,
                            char__ <'a'>,
                            char__ <'m'>,
                            char__ <'s'> >
{
};

struct std_prefix_p : seq__ < char__ <'s'>,
                              char__ <'t'>,
                              char__ <'d'>,
                              char__ <':'>,
                              char__ <':'> >
{
};

struct streams_cout_p :
    semantic_rule<streams_cout_p,
                  seq__ < std_prefix_p,
                          char__ <'c'>,
                          char__ <'o'>,
                          char__ <'u'>,
                          char__ <'t'> > >
{
    // Semantic rule
    template <typename S>
    static inline void process_match (S& state, match_pair const&)
    {
        state.semantic_state().ls.streams.push_back(&std::cout);
    }
};

struct streams_cerr_p :
    semantic_rule<streams_cerr_p,
                  seq__ < std_prefix_p,
                          char__ <'c'>,
                          char__ <'e'>,
                          char__ <'r'>,
                          char__ <'r'> > >
{
    // Semantic rule
    template <typename S>
    static inline void process_match (S& state, match_pair const&)
    {
        state.semantic_state().ls.streams.push_back(&std::cerr);
    }
};

struct streams_clog_p :
    semantic_rule<streams_clog_p,
                  seq__ < std_prefix_p,
                          char__ <'c'>,
                          char__ <'l'>,
                          char__ <'o'>,
                          char__ <'g'> > >
{
    // Semantic rule
    template <typename S>
    static inline void process_match (S& state, match_pair const&)
    {
        state.semantic_state().ls.streams.push_back(&std::clog);
    }
};

struct streams_file_p :
    semantic_rule<streams_file_p,
                  seq__ < char__ <'f'>,
                          char__ <'i'>,
                          char__ <'l'>,
                          char__ <'e'>,
                          char__ <'('>,
                          ignore_p,
                          string_p,
                          ignore_p,
                          char__ <')'> > >
{
    // Semantic rule
    template <typename S>
    static inline void process_match (S& state, match_pair const&)
    {
        // The recognized string_p element is in the top of the
        // stack. Get it.
        std::string s = detail::any_t::any_cast<std::string>(
            state.semantic_state().tok_stack().front());

        // 200 output files configured. Note that this doesn't allow
        // files with the same name when the program changes its cwd.
        // Note that the whole set of open files will be closed when
        // the program finishes.
        typedef detail::static_store_with_check<200,
            std::string,
            impl::null_id_value<std::string>,
            std::ofstream>
                ofstream_store_t;

        ofstream_store_t::result_t res =
            ofstream_store_t::check_element_for(s);

        // not found? open it appending (default behavior)
        if (!res.first)
            res.second->open(s.c_str(), std::ios::app);

        // Store the pointer. The `second' element is returned by
        // pointer.
        state.semantic_state().ls.streams.push_back(res.second);

        // Pop the string from the stack.
        state.semantic_state().tok_stack().pop_front();
    }
};

struct log_p : seq__ < char__ <'l'> , char__<'o'>, char__<'g'> >
{
};

struct conf_enabled :
        semantic_rule<conf_enabled,
                      seq__ < enabled_p,
                              ignore_p,
                              char__<'='>,
                              ignore_p,
                              boolean_p > >
{
    // Semantic rule
    template <typename S>
    static inline void process_match (S& state, match_pair const&)
    {
        bool b = detail::any_t::any_cast<bool>(
            state.semantic_state().tok_stack().front());
        state.semantic_state().ls.enabled = b;
        state.semantic_state().tok_stack().pop_front();
    }
};

struct conf_group_filter :
        semantic_rule < conf_group_filter,
                        seq__ < group_filter_p,
                                ignore_p,
                                char__<'='>,
                                ignore_p,
                                string_p > >
{
    // Semantic rule
    template <typename S>
    static inline void process_match (S& state, match_pair const&)
    {
        std::string s = detail::any_t::any_cast<std::string>(
            state.semantic_state().tok_stack().front());
        state.semantic_state().ls.group_s_exp = impl::group_filter (s);
        state.semantic_state().tok_stack().pop_front();
    }
};

struct conf_log_format :
        semantic_rule < conf_log_format,
                        seq__ < log_format_p,
                                ignore_p,
                                char__<'='>,
                                ignore_p,
                                string_p > >
{
    // Semantic rule
    template <typename S>
    static inline void process_match (S& state, match_pair const&)
    {
        std::string s = detail::any_t::any_cast<std::string>(
            state.semantic_state().tok_stack().front());
        state.semantic_state().ls.log_format = s;
        state.semantic_state().tok_stack().pop_front();
    }
};


struct conf_level_filter :
        semantic_rule<conf_level_filter,
                      seq__ < level_filter_p,
                              ignore_p,
                              char__<'='>,
                              ignore_p,
                              string_p > >
{
    // Semantic rule
    template <typename S>
    static inline void process_match (S& state, match_pair const&)
    {
        std::string s = detail::any_t::any_cast<std::string>(
            state.semantic_state().tok_stack().front());
        state.semantic_state().ls.level_s_exp = impl::level_filter (s);
        state.semantic_state().tok_stack().pop_front();
    }
};

struct conf_streams_opts : or__ < streams_cerr_p,
                                  streams_clog_p,
                                  streams_cout_p,
                                  streams_file_p
                                  >
{
};

struct conf_streams :
    seq__ < streams_p,
            ignore_p,
            char__<'='>,
            ignore_p,
            opt__ < seq__< char__ <'{'>,
                           ignore_p,
                           or__ <seq__ <conf_streams_opts,
                                        star__ < seq__ <spaces, ignore_p,
                                                        conf_streams_opts>
                                                 >
                                        >,
                                 empty__>,
                           ignore_p,
                           char__ <'}'>
                           >
                    >
            >
{
};

struct log_config_body : seq__ < char__<'{'>, ignore_p,
                                 star__ <
                                     seq__ < or__ < conf_level_filter,
                                                    conf_group_filter,
                                                    conf_enabled,
                                                    conf_log_format,
                                                    conf_streams
                                                    >,
                                             spaces,
                                             ignore_p
                                             >
                                     >,
                                 char__<'}'>
                                 >
{
};

struct service_p : seq__<char__<'s'>,char__<'e'>,char__<'r'>,
                         char__<'v'>,char__<'i'>,char__<'c'>,char__<'e'> >
{
};

// Service config. Ignored
struct service_config: seq__< service_p, ignore_p,
                              char__<'{'>, star__< notchar__<'}'> >, char__<'}'> >
{
};

// log "name" { xxx=yyy }
// Notes:
// * the log_config_body will have the log state filled
// * the string_p will be the top of the token stack as an any_t
// * The string will be used to store the config for the log in
//   question
struct log_config :
        semantic_rule<log_config,
                      seq__ < log_p, spaces, string_p, spaces, log_config_body> >
{
    // Semantic rule
    template <typename S>
    static inline void process_match (S& state, match_pair const&)
    {
        std::string logger_name = detail::any_t::any_cast<std::string>(
            state.semantic_state().tok_stack().front());
        logger_state& ls = state.semantic_state().ls;
        ls.name = logger_name;
        state.semantic_state().tok_stack().pop_front();

        logger_store_t::result_t r =
            logger_store_t::check_element_for(logger_name);

        // supposedly, r.first == false, but we don't care. Yes, we're bad
        *(r.second) = ls;

        // Clean the logger_state for forthcomming logs
        ls = logger_state();
    }
};

// Global configuration.
struct configuration : seq__< ignore_p,
                              opt__ <service_config>,
                              ignore_p,
                              star__ < seq__< log_config, ignore_p > > >
{
};

// Configuration semantic state
struct SemanticState
{
    // element stack
    typedef std::list<detail::any_t> any_stack_t;
    any_stack_t any_stack_;
    // State stack, for backtracking
    std::list< any_stack_t::iterator > pos_stack_;
    // Temporal logger_state
    logger_state ls;

    void
    push_tok (detail::any_t const& v)
    {
        any_stack_.push_front (v);
    }

    any_stack_t&
    tok_stack()
    {
        return any_stack_;
    }

    // Common interface
    void
    push_state()
    {
        pos_stack_.push_front (any_stack_.begin());
    }

    void
    rollback()
    {
        any_stack_t::iterator& p = pos_stack_.front();
        // Remove from begin() to ppp.second
        any_stack_.erase (any_stack_.begin(), p);
        pos_stack_.pop_front();
    }

    void
    commit()
    {
        pos_stack_.pop_front();
    }
};

} // parser
} // conf
} // log
} // base
} // csu


namespace csu {
namespace base {
namespace log {

///
/// Logger base
///
/// This class is the logger base.
/// It includes all the functionality related to:
/// - stream list management
/// - log formating
/// - enable/disable options
/// - logging filters
///
/// This is the class the template loggers inherit from.
///
class logger_base
{
protected:
    // d-pointer, state.
    logger_state d;

public:
    /**
     * Base empty constructor
     */
    logger_base()
    {
        d.enabled = true;
    }

    /**
     * This method is the base constructor.
     * @param s the stream the logger is associated to.
     */
    logger_base (std::ostream& os)
    {
        d.enabled = true;
        d.streams.push_back (&os);
    }

    virtual ~logger_base()
    {
    }

    logger_base& enable()
    {
        d.enabled = true;
        return *this;
    }

    logger_base& disable()
    {
        d.enabled = false;
        return *this;
    }

    virtual logger_base& set_group_filter (impl::group_filter const& f) = 0;

    virtual logger_base& set_level_filter (impl::level_filter const& f) = 0;

    /**
     * This method is used to set the log format.
     *
     * @param s is the string which defines the format.
     * @return the logger object (it is modeled as an operator)
     *
     * If no format is set, the logger's output is the payload
     * by default
     *
     * Log format:
     * It is applied if it is not empty. The options that
     * can be chosen are the following:
     * - %i -> thread_id
     * - %p -> payload (body)
     * - %t -> timestamp
     * - %n -> line number
     * - %g -> group name
     * - %l -> string rep. of the warning level (DEBUG, INFO, etc.)
     */
    logger_base& set_log_format (std::string const& lf)
    {
        d.log_format = lf;
        return *this;
    }

    std::string const& get_log_format() const
    {
        return d.log_format;
    }

    /**
     * This method adds a new stream to the stream list.
     * It allows the developer to have a multistream logger.
     *
     * @param s the stream
     * @return the logger
     */
    logger_base& add_stream(std::ostream& os)
    {
        d.streams.push_back(&os);
        return *this;
    }

    /**
     * Removes a existing stream from the stream list.
     *
     * @param o the stream
     * @return the logger
     */
    logger_base& remove_stream (std::ostream& o)
    {
        std::remove_if ( d.streams.begin(),
                         d.streams.end(),
                         std::bind2nd (
                             std::equal_to<logger_state::OstreamList::value_type>(),
                             &o));
        return *this;
    }

    /**
     * Returns the internal state
     */
    logger_state& state()
    {
        return d;
    }

    /**
     * Returns the internal state
     */
    logger_state const& state() const
    {
        return d;
    }

    /**
     * Output a timestamp in the %log
     */
    virtual logger_base& output_timestamp() = 0;

    /**
     * Output the thread ID in the %log
     */
    virtual logger_base& output_thread_id() = 0;

    /**
     * Output the line number in the %log
     */
    virtual logger_base& output_line_num() = 0;

    /**
     * Output the warning level in the %log
     */
    virtual logger_base& output_warning_level() = 0;

    /**
     * Output the group to the log.
     */
    virtual logger_base& output_group() = 0;
};


///
/// own modifiers
///

/// enable %log
inline logger_base& on(logger_base& l)
{
    l.enable();
    return l;
}

/// disable %log
inline logger_base& off(logger_base& l)
{
    l.disable();
    return l;
}

/// add a timestamp
inline logger_base& timestamp(logger_base& l)
{
    l.output_timestamp();
    return l;
}

/// thread id
inline logger_base& thread_id(logger_base& l)
{
    l.output_thread_id();
    return l;
}

/// line number
inline logger_base& line_num (logger_base& l)
{
    l.output_line_num();
    return l;
}

/// warning %level
inline logger_base& w_level_name (logger_base& l)
{
    l.output_warning_level();
    return l;
}

/// %group
inline logger_base& group_name (logger_base& l)
{
    l.output_group();
    return l;
}

/// generic modifier.
template<typename Mod>
struct modifier
{
    Mod m_;

    modifier (Mod m) : m_ (m)
    { }

    /// This is called with the current logger and the
    /// current stream
    /// @param l the logger
    /// @param o the output stream
    void operator()(logger_base& l, std::ostream& o)
    {
        m_ (l, o);
    }
};

/// Set the given format to a logger
class log_line_format_modifier
{
    std::string lf_;
public:
    log_line_format_modifier (std::string const& lf) : lf_ (lf)
    {}

    void operator()(logger_base& l, std::ostream&)
    {
        l.set_log_format (lf_);
    }
};

/// set log format as a modifier
inline modifier<log_line_format_modifier> log_format (std::string const& f)
{
    return modifier<log_line_format_modifier>(log_line_format_modifier (f));
}

///
/// logger<bool>
///
template <bool b,
          typename ThreadModel = impl::default_threading_model>
class logger : public logger_base
{
    typedef typename ThreadModel::default_lock Lock;
    typedef typename ThreadModel::default_thread_id ThreadID;

public:
    logger(bool register_p = true)
        : logger_base ()
    {}

    logger (std::ostream& os, bool register_p = true)
        : logger_base (os)
    {}

    logger (const char* /*name*/, bool register_p = true)
        : logger_base ()
    {}

    logger (const char* /*name*/, std::ostream& os, bool register_p = true)
        : logger_base (os)
    {}

    ~logger()
    {}

    logger_base& set_group_filter (impl::group_filter const&)
    {
        return *this;
    }

    logger_base& set_level_filter (impl::level_filter const&)
    {
        return *this;
    }

    ///
    /// custom modifiers handle, specialization of the generic
    /// operator<<
    template<typename T>
    friend logger<b,ThreadModel>&
    operator<< (logger<b,ThreadModel>& l, modifier<T>)
    {
        return l;
    }

    template <typename T>
    friend logger<b,ThreadModel>&
    operator<< (logger<b,ThreadModel>& l, T const&)
    {
        // Default: do nothing
        return l;
    }

    ///
    /// Standard std modifiers with one parameter (endl, ends)
    friend logger<b,ThreadModel>&
    operator<< (logger<b,ThreadModel>& l, std::ostream& (*)(std::ostream&))
    {
        return l;
    }

    ///
    /// Group filtering
    friend logger<b,ThreadModel>&
    operator<< (logger<b,ThreadModel>& l, impl::group_filter const&)
    {
        return l;
    }

    ///
    /// Level filtering
    friend logger<b,ThreadModel>&
    operator<< (logger<b,ThreadModel>& l, impl::level_filter const&)
    {
        return l;
    }

    ///
    /// Own modifiers
    friend logger<b,ThreadModel>&
    operator<< (logger<b,ThreadModel>& l, logger_base& (*)(logger_base&))
    {
        return l;
    }

    /**
     * Output a timestamp in the log (do nothing)
     */
    logger_base& output_timestamp()
    {
        return *this;
    }

    /**
     * Output the thread ID in the log
     */
    virtual logger_base& output_thread_id()
    {
        return *this;
    }

    /**
     * Output the line number in the log
     */
    virtual logger_base& output_line_num()
    {
        return *this;
    }

    /**
     * Output the warning level in the log
     */
    virtual logger_base& output_warning_level()
    {
        return *this;
    }

    /**
     * Output the group to the log.
     */
    virtual logger_base& output_group()
    {
        return *this;
    }
};

///
/// log_configuration
///
struct log_configuration
{
    log_configuration()
    {
    }

    log_configuration(const std::string& file)
    {
        read_config_from_file (file);
    }

    void read_config_from_file (const std::string& file)
    {
        std::ifstream lc (file.c_str(), std::ios::ate);
        if (lc.is_open())
        {
            std::ifstream::pos_type size = lc.tellg();
            char* memblock = new char [size];
            lc.seekg (0, std::ios::beg);
            lc.read (memblock, size);
            lc.close();

            // Parse conf
            conf::parser::SemanticState ss;
            impl::parser::State<conf::parser::SemanticState> s
                (ss, memblock, size);

            conf::parser::configuration::match(s);
            // bool match = conf::parser::configuration::match(s)
            //    && (s.pos() - memblock == size);

            // Process all loggers
            csu::base::log::static_list<logger_state>::plist& l =
                csu::base::log::static_list<logger_state>::list (NULL);
            for (csu::base::log::static_list<logger_state>::plist::const_iterator it =
                     l.begin();
                 it != l.end();
                 ++it)
            {
                conf::logger_store_t::result_t r =
                    conf::logger_store_t::check_element_for((*it)->name);

                // Found? Apply data
                if (r.first)
                {
                    logger_state const& ls = *(r.second);

                    (*it)->enabled = ls.enabled;
                    (*it)->log_format = ls.log_format;

                    // Update then the s-exp compilations
                    logger_base* lb =
                        reinterpret_cast<logger_base*>((*it)->id);
                    lb->set_level_filter (ls.level_s_exp);
                    lb->set_group_filter (ls.group_s_exp);

                    // Append configured streams
                    (*it)->streams.insert((*it)->streams.end(),
                                          ls.streams.begin(),
                                          ls.streams.end());
                }
            }

            delete[] memblock;
        }
    }

    friend std::ostream& operator<< (std::ostream& os,
                                     log_configuration const&)
    {
        // Process all loggers
        csu::base::log::static_list<logger_state>::plist& l =
            csu::base::log::static_list<logger_state>::list (NULL);
        for (csu::base::log::static_list<logger_state>::plist::const_iterator it =
                 l.begin();
             it != l.end();
             ++it)
        {
            logger_state& ls = **it;
            os << "log \"" << ls.name << "\" {" << std::endl
               << "\tenabled = " << (ls.enabled ? "true" : "false") << std::endl
               << "\tlog_format = \"" << ls.log_format << "\"" << std::endl
               << "\tgroup_filter = \"" << ls.group_s_exp.filter_ << "\"" << std::endl
               << "\tlevel_filter = \"" << ls.level_s_exp.filter_ << "\"" << std::endl
               << "}" << std::endl << std::endl;

            // TODO add here code for output streams
            // it is not possible implement this behavior
            // if the streams are configured in that way
        }

        return os;
    }
};

//
// TSS class (thread-specific data for each log in each
// thread)
//
struct log_tss
{
    const level* actual_level;
    const group* actual_group;
    std::ostringstream ss;

    // output value valid?
    bool cached_output_valid;
    bool cached_output;

    log_tss()
        : actual_level (NULL), actual_group (NULL), cached_output_valid (false)
    {
    }
};

///
/// logger<true>
///
template<typename ThreadModel>
class logger<true, ThreadModel> : public logger_base
{
    typedef typename ThreadModel::default_lock Lock;
    typedef typename ThreadModel::default_thread_id ThreadID;

protected:
    // TSS map
    detail::stable_store<impl::default_thread_pool_size,
                         typename ThreadID::thread_id_t,
                         impl::null_id_value<typename ThreadID::thread_id_t>,
                         log_tss> tss_;

    // mutex
    Lock mutex_;
    // line number
    size_t linenum_;

    // filter applicators
    impl::filter_applicator<level> lf_;
    impl::filter_applicator<group> gf_;

public:
    logger(bool registration_p = true)
        : logger_base (), linenum_ (1)
    {
        _init(0, registration_p);
    }

    logger (std::ostream& _s, bool registration_p = true)
        : logger_base (_s), linenum_ (1)
    {
        _init(0, registration_p);
    }

    logger (const char* _name, bool registration_p = true)
        : logger_base (), linenum_ (1)
    {
        _init(_name, registration_p);
    }

    logger (const char* _name, std::ostream& _s, bool registration_p = true)
        : logger_base (_s), linenum_ (1)
    {
        _init(_name, registration_p);
    }

private:
    void _init(const char* name, bool registration_p = true)
    {
        d.id = reinterpret_cast<logger_id_t>(this);

        // if name is not null, set the logger name. Else, use the id
        // as a name. Also, if the name is set, look in the global
        // configuration to see if data for this logger exists.
        if (name)
        {
            d.name = name;

            conf::logger_store_t::result_t r =
                conf::logger_store_t::check_element_for(d.name);

            // Found? Inherit data
            if (r.first)
            {
                logger_state const& ls = *(r.second);

                d.enabled = ls.enabled;
                d.log_format = ls.log_format;

                // Append configured streams
                d.streams.insert(d.streams.end(),
                                 ls.streams.begin(),
                                 ls.streams.end());

                // Update then the s-exp compilations
                set_level_filter (ls.level_s_exp);
                set_group_filter (ls.group_s_exp);
            }
        }
        else
        {
            std::ostringstream ostr;
            ostr << std::hex << static_cast<size_t>(d.id);
            d.name = ostr.str();
        }

        // Register this logger. Only for true loggers that request
        // to. Internal loggers used by the library itself don't get
        // registered.
        if (registration_p)
            static_list<logger_state>::list (&d);
    }

public:
    ~logger()
    {}

    ///
    /// custom modifiers handle, specialization of the generic
    /// operator<<
    template<typename T>
    friend logger<true,ThreadModel>&
    operator<< (logger<true,ThreadModel>& l, modifier<T> m)
    {
        if (!l._may_do_output())
            return l;

        m (l, l._tss_by_thread_id().ss);

        return l;
    }

    ///
    /// data handle
    template <typename T>
    friend logger<true,ThreadModel>&
    operator<< (logger<true,ThreadModel>& l, T const& v)
    {
        //
        // save the string depending on the thread_id
        // if log enabled and current level is >= min level.
        if (! l._may_do_output())
            return l;

        // get the stringStream
        log_tss& t = l._tss_by_thread_id();
        // add the data
        t.ss << v;

        // return the logger
        return l;
    }

    ///
    /// Group selection
    friend logger<true,ThreadModel>&
    operator<< (logger<true,ThreadModel>& l, group const& g)
    {
        // get the TSS
        log_tss& t = l._tss_by_thread_id();
        t.actual_group = &g;
        t.cached_output_valid = false;
        return l;
    }

    ///
    /// Level selection
    friend logger<true,ThreadModel>&
    operator<< (logger<true,ThreadModel>& l, level const& lvl)
    {
        // get the TSS
        log_tss& t = l._tss_by_thread_id();
        t.actual_level = &lvl;
        t.cached_output_valid = false;
        return l;
    }

    logger_base& set_group_filter (impl::group_filter const& f)
    {
        d.group_s_exp = f;
        gf_.update (f);
        log_tss& t = _tss_by_thread_id();
        t.cached_output_valid = false;
        return *this;
    }

    logger_base& set_level_filter (impl::level_filter const& f)
    {
        d.level_s_exp = f;
        lf_.update (f);
        log_tss& t = _tss_by_thread_id();
        t.cached_output_valid = false;
        return *this;
    }

    ///
    /// Group filtering
    friend logger<true,ThreadModel>&
    operator<< (logger<true,ThreadModel>& l, impl::group_filter const& gf)
    {
        l.set_group_filter (gf);
        return l;
    }

    ///
    /// Level filtering
    friend logger<true,ThreadModel>&
    operator<< (logger<true,ThreadModel>& l, impl::level_filter const& lf)
    {
        l.set_level_filter (lf);
        return l;
    }

    ///
    /// Standard std modifiers with one parameter (endl, ends)
    friend logger<true,ThreadModel>&
    operator<< (logger<true,ThreadModel>& l, std::ostream& (*o)(std::ostream&))
    {
        // get the stringstream
        log_tss& t = l._tss_by_thread_id();

        // perform the modifier and commit the string
        // to the stream
        if (l._may_do_output())
        {
            // Dump the string if the modifier is exactly
            // std::endl. Note that we have to select the correct
            // (overloaded) function
            if (o != (std::ostream& (*)(std::ostream&))(::std::endl))
            {
                // apply the modifier
                (*o)(t.ss);

                // and return
                return l;
            }

            // if d.log_format is empty, do the normal path
            if (l.d.log_format.empty())
            {
                // Apply the modifier here also. If the log format
                // is empty, the new line has to be inserted
                // FIXME: it would be better to find a better approach
                (*o)(t.ss);

                l._output_data_to_streams (t.ss.str());
            }
            else
            {
                std::string result = l._build_log_line (t.ss.str());
                l._output_data_to_streams (result);
            }

            // From now on, only std::endl passed.
            // inc. line no.
            ++l.linenum_;
        }

        // endl? Even if the log is not enabled, there is something to reset
        if (o == (std::ostream& (*)(std::ostream&))(::std::endl))
        {
            // reset the actual level & group
            t.actual_level = NULL;
            t.actual_group = NULL;

            t.cached_output_valid = false;

            // clear the string
            t.ss.str("");
        }

        // return the logger
        return l;
    }

    ///
    /// Own modifiers
    friend logger<true,ThreadModel>&
    operator<< (logger<true,ThreadModel>& l, logger_base& (*o)(logger_base&))
    {
        // treat on and off as special cases
        if (o == on || o == off || l._may_do_output())
        {
            // apply the modifier
            (*o)(l);
        }

        return l;
    }

    /**
     * Output a timestamp in the log
     */
    inline logger_base& output_timestamp()
    {
        if (!_may_do_output())
            return *this;

        std::ostream& o = _tss_by_thread_id().ss;
        _inject_timestamp (o);
        return *this;
    }

    /**
     * Output the thread ID in the log
     */
    virtual logger_base& output_thread_id()
    {
        if (!_may_do_output())
            return *this;

        std::ostream& o = _tss_by_thread_id().ss;
        _inject_thread_id (o);

        return *this;
    }

    /**
     * Output the line number in the log
     */
    virtual logger_base& output_line_num()
    {
        if (!_may_do_output())
            return *this;

        _tss_by_thread_id().ss << '[' << linenum_ << ']';
        return *this;
    }

    /**
     * Output the warning level in the log
     */
    virtual logger_base& output_warning_level()
    {
        if (!_may_do_output())
            return *this;

        log_tss& t = _tss_by_thread_id();
        if (t.actual_level)
            t.ss << t.actual_level->d.name_;

        return *this;
    }

    /**
     * Output the group in the log
     */
    virtual logger_base& output_group()
    {
        if (!_may_do_output())
            return *this;

        log_tss& t = _tss_by_thread_id();
        if (t.actual_group)
            t.ss << t.actual_group->d.name_;

        return *this;
    }

private:

    // May this logger produce output?
    inline bool _may_do_output()
    {
        // if log enabled after all...
        if (!d.enabled)
            return false;

        log_tss& t = _tss_by_thread_id();

        if (t.cached_output_valid)
            return t.cached_output;

        t.cached_output =
            (!t.actual_group
                || (t.actual_group->d.enabled_
                    && gf_.check (t.actual_group->d.id_)))
            && (!t.actual_level
                || (t.actual_level->d.enabled_
                    && lf_.check (t.actual_level->d.order_)));
        t.cached_output_valid = true;

        return t.cached_output;
    }

    inline void _inject_timestamp (std::ostream& o)
    {
        // get a timestamp string
        std::locale loc;

        // get the time
        const std::time_put<char>& tmput =
            std::use_facet <std::time_put<char> > (loc);
        time_t timestamp;
        time ( &timestamp );
        const tm * now = localtime ( &timestamp );

        // get the date
        // char pattern[]="%D";
        static const char* pattern="%c";

        // add the timestamp string
        o << '[';
        tmput.put ( o, o, ' ', now, pattern, pattern+2 );
        o << ']';
    }

    // Build a log line honoring the log format.
    // In this operation, body means what the user
    // has actually output as log (payload)
    inline std::string _build_log_line (std::string const& body)
    {
        std::ostringstream result;
        std::string::size_type st = 0, en = st;

        while (std::string::npos !=
               (en = d.log_format.find_first_of ('%', en)))
        {
            // Some part that it is not a control string?
            if (en != st)
                result << d.log_format.substr (st, en - st);

            // Go to the control char (skip '%')
            ++en;

            char c = d.log_format[en];
            switch(c)
            {
                // payload = body
            case 'p':
                result << body;
                break;

                // thread_id
            case 'i':
                _inject_thread_id (result);
                break;

                // line no.
            case 'n':
                result << '[' <<  linenum_ << ']';
                break;

                // level
            case 'l':
            {
                log_tss& t = _tss_by_thread_id();
                if (t.actual_level)
                    result << t.actual_level->d.name_;
            }
            break;

            // group
            case 'g':
            {
                log_tss& t = _tss_by_thread_id();
                if (t.actual_group)
                    result << t.actual_group->d.name_;
            }
            break;

            // timestamp
            case 't':
                _inject_timestamp (result);
                break;

            default:
                result << '%' << c;
            }

            // skip control char
            ++en;

            st = en;
        }

        // Some part that it is not a control string?
        if (en != st)
            result << d.log_format.substr (st, en - st);

        // Finish the string (endl)
        result << ::std::endl;

        return result.str();
    }

    inline void _output_data_to_streams (std::string const& data)
    {
        for(logger_state::OstreamList::iterator it = d.streams.begin();
            it != d.streams.end();
            ++it)
            impl::ots_manager<Lock>::ostream_ts_wrapper_for(**it) << data;
    }

    inline void _inject_thread_id(std::ostream& o)
    {
        // Add the thread Id
        o << '<' << ThreadID::this_thread_id() << '>';
    }

    inline log_tss& _tss_by_thread_id()
    {
        //depending on the thread identifier...
        typename ThreadID::thread_id_t tid =
            ThreadID::this_thread_id();
        impl::scoped_lock<Lock> sl(mutex_);

        return tss_.element_for (tid);
    }
};

} // log
} // base
} // csu

#endif // CSU_LOG_HPP
