//
//  GlobalContext2.h
//
//  Created by yutaAsai on 2018/06/12.
//
//

#ifndef GlobalContext2_h
#define GlobalContext2_h
#include <type_traits>
#include "ofEventUtils.h"

namespace aaa{
    namespace deteil{
            namespace utils{
                template<class...>
                using void_t = void;
                template<class AlwaysVoid, template<class...>class Op, class ...Args>
                struct is_detected_impl :std::false_type{};
                template<template<class...>class Op, class ...Args>
                struct is_detected_impl<void_t<Op<Args...>>, Op, Args...> :std::true_type{};
            };
        template<template<class...>class Op, class ...Args>
        using is_detected = utils::is_detected_impl<void, Op, Args...>;
        
        template<class T> using has_setup_op = decltype(std::declval<T>().setup());
        template<class T> using has_setup = is_detected<has_setup_op, T>;
        template<class T> using has_update_op = decltype(std::declval<T>().update());
        template<class T> using has_update = is_detected<has_update_op, T>;

        struct wrapper_bass{
            virtual void call_setup() = 0;
            virtual void call_update() = 0;
        };
        
        template<typename T>
        struct listner_wrapper : wrapper_bass{
            listner_wrapper(){}
            ~listner_wrapper(){}
            void call_setup(){ setup(); }
            void call_update(){ update(); }
            void update_listner_func(ofEventArgs &args){ update(); }
            T t;
        private:
            template <typename U = T> auto setup() -> typename std::enable_if<has_setup<U>::value, void>::type{ t.setup(); }
            template <typename U = T> auto setup() -> typename std::enable_if<!has_setup<U>::value, void>::type{}
            template <typename U = T> auto update() -> typename std::enable_if<has_update<U>::value, void>::type{ t.update(); }
            template <typename U = T> auto update() -> typename std::enable_if<!has_update<U>::value, void>::type{}
        };
        
        template<typename T>
        struct singletoner{
            static  listner_wrapper<T>& get_ref(){ static listner_wrapper<T> t_a;  return t_a; }
            static void call_setup(){ get_ref().call_setup(); }
            static void call_update(){ get_ref().call_update(); }
            template<typename E>
            static void add_update_listner(ofEvent<E>& event){ add_listner_impl(event); }
            template<typename E>
            static void remove_update_listner(ofEvent<E>& event){ remove_listner_impl(event); }
        private:
            singletoner(){}
            ~singletoner(){}
            template <typename E, typename U = T> static auto add_listner_impl(ofEvent<E>& event) -> typename std::enable_if<has_update<U>::value, void>::type{
                ofAddListener(event, &(get_ref()), &listner_wrapper<T>::update_listner_func);
            }
            template <typename E, typename U = T> static auto add_listner_impl(ofEvent<E>& event) -> typename std::enable_if<!has_update<U>::value, void>::type{}
            
            template <typename E, typename U = T> static auto remove_listner_impl(ofEvent<E>& event) -> typename std::enable_if<has_update<U>::value, void>::type{
                ofRemoveListener(event, &(get_ref()), &listner_wrapper<T>::update_listner_func);
            }
            template <typename E, typename U = T> static auto remove_listner_impl(ofEvent<E>& event) -> typename std::enable_if<!has_update<U>::value, void>::type{}
        };
        
        struct accessor{
            template<typename T>
            static void setup(){
                singletoner<T>::call_setup();
                add_update_listner<T>(ofEvents().draw);

            }
            template<typename T, typename E>
            static void setup(ofEvent<E>& event){
                singletoner<T>::call_setup();
                add_update_listner<T>(event);
            }

            template<typename T>
            static void update(){ singletoner<T>::call_update(); }
            template<typename T, typename E>
            static void add_update_listner(ofEvent<E>& event){ singletoner<T>::add_update_listner(event); }
            template<typename T, typename E>
            static void remove_update_listner(ofEvent<E>& event){ singletoner<T>::remove_update_listner(event); }
            template<typename T, typename E1, typename E2>
            static void change_update_listner(ofEvent<E1>& current_event, ofEvent<E2>& new_event){
                remove_update_listner<T>(current_event);
                add_update_listner<T>(new_event);
            }
            template<typename T>
            static T& get(){ return singletoner<T>::get_ref().t; }
        };
    };
};


typedef aaa::deteil::accessor context;

#endif /* GlobalContext2_h */
