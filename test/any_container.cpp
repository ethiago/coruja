
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <coruja/object/object.hpp>
#include <coruja/object/view/any_object.hpp>
#include <coruja/object/view/object.hpp>
#include <coruja/container/view/any_container.hpp>
#include <coruja/container/vector.hpp>
#include <coruja/container/list.hpp>
#include <coruja/container/map.hpp>
#include <coruja/container/view/filter.hpp>
#include <coruja/container/view/transform.hpp>

#include <boost/core/lightweight_test.hpp>
#include <sstream>

#include "run_container.hpp"
#include "check_equal.hpp"

using namespace coruja;

template<typename T>
using vec = std::vector<T>;
using ivec = vec<int>;
using svec = vec<std::string>;
using sivec = vec<std::pair<std::string, int>>;

using ivector = vector<int>;
using svector = vector<std::string>;
using ilist = list<int>;
using slist = list<std::string>;
using simap = map<std::string,int>;
using iany_t = view::any_container<int>;
using sany_t = view::any_container<std::string>;
using siany_t = view::any_container<std::pair<std::string,int>>;

template<typename Cont>
struct Change;

template<>
struct Change<void>
{
    template<typename V>
    V& operator()(V& c)
    { return c; }
};

template<typename T>
struct Change<vector<T>>
{
    template<typename V>
    V& operator()(V& c)
    {
        for(auto&& p : toRemove)
            cont->erase(cont->begin() + p);

        return c;
    }


    ivec toRemove;
    vector<T>* cont;
};

template<typename K, typename V>
struct Change<map<K,V>>
{
    template<typename View>
    View& operator()(View& c)
    {
        for(auto&& p : toRemove)
            cont->erase(p);

        return c;
    }


    std::vector<K> toRemove;
    map<K,V>* cont;
};

template<typename Cont, typename Rem>
Change<Cont> change(Cont& c, Rem toRemove)
{ return Change<Cont>{toRemove,&c}; }


struct keys
{
    template<typename pair>
    auto operator()(const pair& p) -> decltype(p.first)
    { return p.first; }
};

struct is_vowel
{
    template<typename T>
    bool operator()(const T& v)
    { return v == "a" || v == "e" || v == "i" || v == "o" || v == "u"; }
};

int main()
{
    //default ctor
    {
        iany_t iany;
        siany_t siany;
    }

    //from vector range
    {
        ivector originator = { 1, 3, 5, 7, 9};
        iany_t any(originator);
        
        check_equal(any, originator);

        svector originator2 = { "1", "3", "5", "7", "9"};
        sany_t any2(originator2);
        check_equal(any2, originator2);
    }

    //from list range
    {
        ilist originator = { 1, 3, 5, 7, 9};
        iany_t any(originator);
        
        check_equal(any, {1,3,5,7,9});
    }

    //from vector
    {
        ivector originator = { 1, 3, 5, 7, 9};
        iany_t any(originator);
        ivec expected_foreach = { 1, 3, 5, 7, 9};
        ivec keys_toRemove = {4};
        ivec erased_expected = {9};

        run_view(any, expected_foreach, change(originator, keys_toRemove), erased_expected );
    }

    //from map
    {
        simap originator = { {"a",1}, {"b", 3}, {"c",5}, {"d",7}, {"e", 9} };
        siany_t any(originator);
        sivec expected_foreach = { {"a",1}, {"b", 3}, {"c",5}, {"d",7}, {"e", 9} };
        svec keys_toRemove = { "c", "e" };
        sivec erased_expected = {{"c",5}, {"e", 9}};

        run_view(any, expected_foreach, change(originator, keys_toRemove), erased_expected);
    }

    //from transform
    {
        simap originator = { {"a",1}, {"b", 3}, {"c",5}, {"d",7}, {"e", 9} };
        auto t = view::transform(originator, keys{} );
        sany_t any(t);
        svec expected_foreach = { "a", "b", "c", "d", "e" };
        svec keys_toRemove = { "c", "e" };
        svec erased_expected = { "c", "e" };

        run_view(any, expected_foreach, change(originator, keys_toRemove), erased_expected);
    }
    //from transform lambda
    {
        simap originator = { {"a",1}, {"b", 3}, {"c",5}, {"d",7}, {"e", 9} };
        auto t = view::transform(originator, [](const simap::value_type& t ){ return t.first; });
        sany_t any(t);
        svec expected_foreach = { "a", "b", "c", "d", "e" };
        svec keys_toRemove = { "c", "e" };
        svec erased_expected = { "c", "e" };

        run_view(t, expected_foreach, change(originator, keys_toRemove), erased_expected);
    }

    //from filter
    {
        ivector originator = { 1, 3, 5, 7, 9};
        iany_t any(view::filter(originator, [](int i){ return i>5; } ) );
        ivec expected_foreach = { 7, 9};
        ivec keys_toRemove = {4};
        ivec erased_expected = {9};

        run_view(any, expected_foreach, change(originator, keys_toRemove), erased_expected );
    }
    
     //from filter map
    {
        simap originator = { {"a",1}, {"b", 3}, {"c",5}, {"d",7}, {"e", 9} };
        siany_t any(originator | view::filter([](simap::value_type t ){ return true; }) );
        sivec expected_foreach = { {"a",1}, {"b", 3}, {"c",5}, {"d",7}, {"e", 9} };
        svec keys_toRemove = { "c", "e" };
        sivec erased_expected = {{"c",5}, {"e", 9}};

        run_view(any, expected_foreach, change(originator, keys_toRemove), erased_expected);
    }

    //from filter transform
    {
        simap originator = { {"a",1}, {"b", 3}, {"c",5}, {"d",7}, {"e", 9} };
        iany_t any(view::transform(originator, [](simap::value_type t){ return t.second; })
                   | view::filter([](int i){ return i > 5; })
        );
        ivec expected_foreach = { 7, 9};
        svec keys_toRemove = { "e" };
        ivec erased_expected = {9};

        run_view(any, expected_foreach, change(originator, keys_toRemove), erased_expected);
    }

    //from transform filter
   {
       simap originator = { {"a",1}, {"b", 3}, {"c",5}, {"d",7}, {"e", 9} };
       iany_t any(view::transform(originator
                                  | view::filter([](simap::value_type t){ return t.second > 5; })
                                  , [](simap::value_type t){ return t.second; }));
       ivec expected_foreach = { 7, 9};
       svec keys_toRemove = { "e" };
       ivec erased_expected = {9};

       run_view(any, expected_foreach, change(originator, keys_toRemove), erased_expected);
   }

   {
       list<object<int>> list;
       
       iany_t any(view::transform(list, [](object<int>& o) { return o.observed(); }));

       check_equal(any, list);
   }

   {
       list<object<int>> list;
       list.emplace_back(1);
       list.emplace_back(2);
       
       view::any_container<view::object<object<int>>> any;

       auto trans = view::transform(list, [](object<int>& o) { return view::view(o); });
       any = trans;

       check_equal(any, list);
   }

   {
       list<object<int>> list;
       list.emplace_back(1);
       list.emplace_back(2);
       
       view::any_container<view::any_object<int>> any;

       auto trans = view::transform(list, [](object<int>& o) { return view::any_object<int>(o); });
       any = trans;

       check_equal(any, list);
       ivec expected_foreach = { 1, 2};

       run_view(any, expected_foreach);
   }

    return 0;
}
