//
//  GlobalContext2.h
//
//  Created by yutaAsai on 2018/06/12.
//
//

#ifndef GlobalContext2_h
#define GlobalContext2_h
#include <type_traits>
#include <typeinfo>
#include <cxxabi.h>
#include <string>
#include <stdlib.h>
#include <map>
#include <string>
#include "ofEventUtils.h"

//#define debug_ofxgc2

namespace aaa{
    namespace global_context2{
        namespace detail{
            namespace utils{
                namespace rtti{
                    template<typename T>
                    static const std::string get_name(const T& obj){
                        const std::type_info& id = typeid(obj);
                        int stat(0);
                        std::string name_buffer;
                        char *name = abi::__cxa_demangle(id.name(),0,0,&stat);
                        if(name!=NULL) {
                            if(stat == 0) {
                                name_buffer = name;
                                free(name);
                                return name_buffer;
                            }
                            free(name);
                        }
                        return "";
                    }
                };
                
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
            
            template<class T> using has_before_update_op = decltype(std::declval<T>().beforeUpdate());
            template<class T> using has_before_update = is_detected<has_before_update_op, T>;
            template<class T> using has_after_update_op = decltype(std::declval<T>().afterUpdate());
            template<class T> using has_after_update = is_detected<has_after_update_op, T>;
            
            template<class T> using has_before_draw_op = decltype(std::declval<T>().beforeDraw());
            template<class T> using has_before_draw = is_detected<has_before_draw_op, T>;
            template<class T> using has_after_draw_op = decltype(std::declval<T>().afterDraw());
            template<class T> using has_after_draw = is_detected<has_after_draw_op, T>;
#ifdef debug_ofxgc2
            template<typename T>
            static void type_log(const std::string message){
                ofLogNotice("GlobalContext2") << " : " << utils::rtti::get_name(T()) << " : " << message;
            }
#else
            template<typename T>
            static void type_log(const std::string message){}
#endif
        };
        
        struct wrapper_base{
            wrapper_base(){}
            virtual ~wrapper_base(){}
            virtual void call_setup() = 0;
            virtual void call_update() = 0;
            static std::map<std::string, wrapper_base*>& context_map(){
                static std::map<std::string, wrapper_base*> _ ;
                return _;
            }
        };
        
        struct accessor;
        
        template<typename T>
        struct context_wrapper : wrapper_base{
            friend class accessor;
            
            void call_setup(){ setup(); }
            void call_update(){ update(); }
            
            template <typename U = T>
            static auto setup()
            -> typename std::enable_if<detail::has_setup<U>::value, void>::type
            {
                get().setup();
            }
            
            template <typename U = T>
            static auto setup()
            -> typename std::enable_if<!detail::has_setup<U>::value, void>::type
            {
                get();
            }
            
            template <typename U = T>
            static auto update()
            -> typename std::enable_if<detail::has_update<U>::value, void>::type
            {
                get().update();
            }
            
            template <typename U = T>
            static auto update()
            -> typename std::enable_if<!detail::has_update<U>::value, void>::type
            {}
            
            T t;
        private:
            static context_wrapper<T>& get_ref(){
                static context_wrapper<T> _;
                return _;
            }
            
            static T& get(){
                return get_ref().t;
            }
            
            context_wrapper()
            {
                init_extension();
                add_update();
            }
            
            ~context_wrapper(){}
            
            static void init_extension(){
                add_beforeUpdate();
                add_afterUpdate();
                add_beforeDraw();
                add_afterDraw();
            }
            
#pragma mark function_wrapp originals
            
            template <typename U = T>
            static auto setup(ofEventArgs &)
            -> typename std::enable_if<detail::has_setup<U>::value, void>::type
            {
                get().setup();
            }
            
            template <typename U = T>
            static auto update(ofEventArgs& event)
            -> typename std::enable_if<detail::has_update<U>::value, void>::type
            {
                get().update();
            }
            
#pragma mark function_wrapp extensions
            
            template <typename U = T>
            static auto beforeUpdate(ofEventArgs &)
            -> typename std::enable_if<detail::has_before_update<U>::value, void>::type
            {
                get().beforeUpdate();
            }
            
            template <typename U = T>
            static auto afterUpdate(ofEventArgs &)
            -> typename std::enable_if<detail::has_after_update<U>::value, void>::type
            {
                get().afterUpdate();
            }
            
            template <typename U = T>
            static auto beforeDraw(ofEventArgs &)
            -> typename std::enable_if<detail::has_before_draw<U>::value, void>::type
            {
                get().beforeDraw();
            }
            
            template <typename U = T>
            static auto afterDraw(ofEventArgs &)
            -> typename std::enable_if<detail::has_after_draw<U>::value, void>::type
            {
                get().afterDraw();
            }
            
#pragma mark add_listner_functions originals
            
            template <typename U = T>
            static auto add_update()
            -> typename std::enable_if<detail::has_update<U>::value, void>::type
            {
                ofEvents().update.add((void(*)(ofEventArgs &))(&context_wrapper<T>::update), OF_EVENT_ORDER_BEFORE_APP);
                detail::type_log<T>("success add listner update");
            }
            
            template <typename U = T>
            static auto add_update()
            -> typename std::enable_if<!detail::has_update<U>::value, void>::type
            {
                detail::type_log<T>("failure add listner, has not update");
            }
            
            template <typename U = T>
            static auto remove_update()
            -> typename std::enable_if<detail::has_update<U>::value, void>::type
            {
                ofEvents().update.remove((void(*)(ofEventArgs &))(&context_wrapper<T>::update), OF_EVENT_ORDER_BEFORE_APP);
                detail::type_log<T>("success remove listner update");
            }
            
            template <typename U = T>
            static auto remove_update()
            -> typename std::enable_if<!detail::has_update<U>::value, void>::type
            {
                detail::type_log<T>("failure remove listner, has not update");
            }
            
            template <typename U = T, typename E>
            static auto chenge_update(ofEvent<E>& event, ofEventOrder order)
            -> typename std::enable_if<detail::has_update<U>::value, void>::type
            {
                remove_update();
                event.add((void(*)(ofEventArgs &))(&context_wrapper<T>::update), OF_EVENT_ORDER_BEFORE_APP);
                detail::type_log<T>("success chenge listner update");
            }
            
            template <typename U = T, typename E>
            static auto chenge_update(ofEvent<E>& event, ofEventOrder order)
            -> typename std::enable_if<!detail::has_update<U>::value, void>::type
            {
                detail::type_log<T>("failure change listner, has not update");
            }
            
#pragma mark add_listner_functions extensions
            
            template <typename U = T>
            static auto add_beforeUpdate()
            -> typename std::enable_if<detail::has_before_update<U>::value, void>::type
            {
                ofEvents().update.add((void(*)(ofEventArgs &))&context_wrapper<T>::beforeUpdate, OF_EVENT_ORDER_BEFORE_APP);
                detail::type_log<T>("success add listner beforeUpdate");
            }
            
            template <typename U = T>
            static auto add_beforeUpdate()
            -> typename std::enable_if<!detail::has_before_update<U>::value, void>::type
            {
                detail::type_log<T>("failure add listner, has not beforeUpdate");
            }
            
            template <typename U = T>
            static auto add_afterUpdate()
            -> typename std::enable_if<detail::has_after_update<U>::value, void>::type
            {
                ofEvents().update.add((void(*)(ofEventArgs &))&context_wrapper<T>::afterUpdate, OF_EVENT_ORDER_AFTER_APP);
                detail::type_log<T>("success add listner afterUpdate");
            }
            
            template <typename U = T>
            static auto add_afterUpdate()
            -> typename std::enable_if<!detail::has_after_update<U>::value, void>::type
            {
                detail::type_log<T>("failure add listner, has not afterUpdate");
            }
            
            template <typename U = T>
            static auto add_beforeDraw()
            -> typename std::enable_if<detail::has_before_draw<U>::value, void>::type
            {
                ofEvents().draw.add((void(*)(ofEventArgs &))&context_wrapper<T>::beforeDraw, OF_EVENT_ORDER_BEFORE_APP);
                detail::type_log<T>("success add listner beforeDraw");
            }
            
            template <typename U = T>
            static auto add_beforeDraw()
            -> typename std::enable_if<!detail::has_before_draw<U>::value, void>::type
            {
                detail::type_log<T>("failure add listner, has not beforeDraw");
            }
            
            template <typename U = T>
            static auto add_afterDraw()
            -> typename std::enable_if<detail::has_after_draw<U>::value, void>::type
            {
                ofEvents().draw.add((void(*)(ofEventArgs &))&context_wrapper<T>::afterDraw, OF_EVENT_ORDER_AFTER_APP);
                detail::type_log<T>("success add listner afterDraw");
            }
            
            template <typename U = T>
            static auto add_afterDraw()
            -> typename std::enable_if<!detail::has_after_draw<U>::value, void>::type
            {
                detail::type_log<T>("failure add listner, has not afterDraw");
            }
            
        };
            
        struct accessor{
            template<typename T>
            static void setup(){
                context_wrapper<T>::setup();
            }
            
            template<typename T, typename E>
            static void setup(ofEvent<E>& event, ofEventOrder order){
                context_wrapper<T>::setup();
                context_wrapper<T>::change_update(event, order);
            }
            
            template<typename T>
            static void update(){
                context_wrapper<T>::update();
            }
            
            template<typename T>
            static T& get(){
                return context_wrapper<T>::get();
            }
        };
    };
};

using context = aaa::global_context2::accessor;


#endif /* GlobalContext2_h */
