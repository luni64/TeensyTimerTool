#pragma once

// Helper class to 'statify' legacy callback providers like e.g. DMAChannel, attachInterrupt, Intervaltimer...
// might go into a dedicated library

#include <array>
#include <utility>

template <typename Derived, unsigned N>
class CallbackStateHelper
{
    using callback_t = typename Derived::callback_t;
    using state_t    = typename Derived::state_t;

 public:
    static callback_t* callbacks[N];               // storage for the callback function pointers
    static state_t states[N];                      // storage for the corresponding state variables
    static const std::array<void (*)(), N> relays; // relays for attaching to an NVIC interrupt (type: void(*)())

 protected:
    template <unsigned nr> // actual relay function #nr
    static void relay()    // relay simply invokes the stored callback with its corresponding state as parameter
    {
        callbacks[nr](states[nr]);
    }

    template <std::size_t... I>                                                      // Recursive helper function to generate an array of N void(*)() relays
    static constexpr std::array<void (*)(), N> MakeRelays(std::index_sequence<I...>) // the array will be intialized with (n=0..N) relay<n> functions.
    {
        return std::array<void (*)(), N>{relay<I>...}; // The array will be generated and initialized at compile time
    }
};

// intialize static variables --------------------
template <typename Derived, unsigned N>
typename CallbackStateHelper<Derived, N>::callback_t* CallbackStateHelper<Derived, N>::callbacks[N];

template <typename Derived, unsigned N>
typename CallbackStateHelper<Derived, N>::state_t CallbackStateHelper<Derived, N>::states[N];

template <typename Derived, unsigned N>
const std::array<void (*)(), N> CallbackStateHelper<Derived, N>::relays = CallbackStateHelper::MakeRelays(std::make_index_sequence<N>{});
