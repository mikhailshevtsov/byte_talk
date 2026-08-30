#ifndef BYTETALK_CONFIG_HPP
#define BYTETALK_CONFIG_HPP

#include "jarl.hpp"

#include <string>

namespace bt
{

JARL_STRUCT(
    config,
    JARL_FIELD(host, std::string, "127.0.0.1")
    JARL_FIELD(port, short, 8080)
    JARL_FIELD(ssl, bool, true)
    JARL_FIELD(cert, std::string)
    JARL_FIELD(pkey, std::string)
    JARL_FIELD(max_conn, std::size_t, 10000)
);

}


#endif //BYTETALK_CONFIG_HPP