#ifndef BYTETALK_SIGNAL_HPP
#define BYTETALK_SIGNAL_HPP

#include "client.hpp"
#include <boost/signals2.hpp>

namespace bt
{

using signal = boost::signals2::signal<void(client)>;

}

#endif //BYTETALK_SIGNAL_HPP