/// @file
///	@ingroup 	minexamples
///	@copyright	Copyright 2018 The Min-DevKit Authors. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.


#include "./window.h"

#include "c74_min.h"
#include "c74_min_message.h"
#include <memory>




using namespace c74::min;


class playground : public object<playground>, public vector_operator<> {

private: 
    WindowWithWebview windowWithWebview;
    bool windowVisible = false; 

public:
    MIN_DESCRIPTION	{ "Load a choc window." };
    MIN_TAGS		{ "custom gui" };
    MIN_AUTHOR		{ "RM Estali" };
    MIN_RELATED		{ "index~, buffer~, wave~" };

    inlet<>  m_inlet_index	    { this, "(signal) Sample index" };
    inlet<>  m_inlet_channel	{ this, "(float) Audio channel to use from buffer~" };
    inlet<>  m_window	    { this, "Open / close window" , "bang"};
    
    
    outlet<> m_outlet_main		{ this, "(signal) Sample value at index", "signal" };
    outlet<> m_outlet_changed	{ this, "(symbol) Notification that the content of the buffer~ changed." };


    buffer_reference m_buffer { this,
        MIN_FUNCTION {    // will receive a symbol arg indicating 'binding', 'unbinding', or 'modified'
            m_outlet_changed.send(args);
            return {};
        }
    };

    argument<symbol> m_name_arg {this, "buffer-name", "Initial buffer~ from which to read.",
        MIN_ARGUMENT_FUNCTION {
            m_buffer.set(arg);
        }
    };

    argument<int> m_channel_arg {this, "channel", "Initial channel to read from the buffer~.",
        MIN_ARGUMENT_FUNCTION {
            m_channel = arg;
        }
    };

    message<> m_number { this, "number", "Set the channel to read from the buffer~. The channel number uses 1-based counting.",
        MIN_FUNCTION {
            if (inlet == 1)
                m_channel = args[0];
            return {};
        }
    };

    message<> m_open_close {this, "bang", "Open or close the UI", 
    MIN_FUNCTION { 
        if (inlet == 2){
            windowVisible = !windowVisible;
            windowWithWebview.window.setVisible(windowVisible);
            if (windowVisible) windowWithWebview.window.toFront();
        }
        return {};
        }
    }; 

    attribute<int, threadsafe::no, limit::clamp> m_channel {this, "channel", 1,
        description {"Channel to read from the buffer~. The channel number uses 1-based counting."},
        range {1, buffer_reference::k_max_channels}
    };

    void operator()(audio_bundle input, audio_bundle output) {
        auto          in  = input.samples(0);                                     // get vector for channel 0 (first channel)
        auto          out = output.samples(0);                                    // get vector for channel 0 (first channel)
        buffer_lock<> b(m_buffer);                                                  // gain access to the buffer~ content
        auto          chan = std::min<size_t>(m_channel - 1, b.channel_count());    // convert from 1-based indexing to 0-based

        if (b.valid()) {
            for (auto i = 0; i < input.frame_count(); ++i) {
                auto frame = size_t(in[i] + 0.5);
                out[i]     = b.lookup(frame, chan);
            }
        }
        else {
            output.clear();
        }
    }
};


MIN_EXTERNAL(playground);
