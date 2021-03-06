/*
 * SimplePulseTx.hpp
 *
 *  Created on: Apr 25, 2015
 *      Author: jan
 */

#ifndef SIMPLEPULSETX_HPP_
#define SIMPLEPULSETX_HPP_

#include "PulseTx.hpp"

namespace Serial {

constexpr int simplePulseTxDefaultQueueSize = 16;

template <int fifoSize=simplePulseTxDefaultQueueSize>
class SimplePulseTxSource {
    Fifo<fifoSize> fifo;
    bool idleHigh;
public:
    SimplePulseTxSource(bool _idleHigh): idleHigh(_idleHigh) {}

    Pulse getNextPulse() {
        Pulse pulse;
        return fifo.read(&pulse) ? pulse : Pulse::empty();
    }

    bool isHighOnIdle() {
        return idleHigh;
    }

    void append(Pulse const &pulse) {
        fifo.write(&pulse);
    }
};

template <typename comparator_t, typename target_wrapper_t, int fifoSize=simplePulseTxDefaultQueueSize>
class SimplePulseTx: public PulseTx<comparator_t, target_wrapper_t, SimplePulseTxSource<fifoSize>> {
    typedef PulseTx<comparator_t, target_wrapper_t, SimplePulseTxSource<fifoSize>> Super;
    SimplePulseTxSource<fifoSize> source;
public:
    SimplePulseTx(comparator_t &_comparator, target_wrapper_t &_target, bool idleHigh):
        Super(_comparator, _target, source), source(idleHigh) {}

    void send(Pulse const &pulse) {
        source.append(pulse);
        Super::sendFromSource();
    }

    template <typename Value>
    inline void sendHigh(Value duration) {
        send(Pulse(true, toCountsOn<comparator_t>(duration)));
    }
};

template <typename comparator_t, typename target_t, int fifoSize=simplePulseTxDefaultQueueSize>
using SimpleCallbackPulseTx = SimplePulseTx<comparator_t, PulseTxCallbackTarget<target_t>, fifoSize>;

template <typename comparator_t, typename target_t, int fifoSize=simplePulseTxDefaultQueueSize>
inline SimpleCallbackPulseTx<comparator_t, PulseTxCallbackTarget<target_t>, fifoSize> simplePulseTx(comparator_t &comparator, target_t &target, bool idleHigh) {
    return SimpleCallbackPulseTx<comparator_t, PulseTxCallbackTarget<target_t>, fifoSize>(comparator, PulseTxCallbackTarget<target_t>(target), idleHigh);
}

template <typename pin_t, int fifoSize=simplePulseTxDefaultQueueSize>
using SimpleComparatorPinPulseTx = SimplePulseTx<typename pin_t::comparator_t, PulseTxComparatorPinTarget<pin_t>, fifoSize>;

template <typename pin_t, int fifoSize=simplePulseTxDefaultQueueSize>
inline SimpleComparatorPinPulseTx<pin_t, fifoSize> simplePulseTx(pin_t &pin, bool idleHigh) {
    return SimpleComparatorPinPulseTx<pin_t, fifoSize>(pin.timerComparator(), pin, idleHigh);
}

}


#endif /* SIMPLEPULSETX_HPP_ */
