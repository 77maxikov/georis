#include "common_optim.h"

s_type norm2(const v_type &v) {
    return std::sqrt(v.transpose()*v);
}
