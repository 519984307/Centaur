////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Created by Ricardo Romero on 15/04/22.
//  Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#ifndef FINANCEABLE_TOTP_HPP
#define FINANCEABLE_TOTP_HPP

#include "Centaur.hpp"
#include <string>

BEGIN_CENTAUR_NAMESPACE

extern int getTOTPCode(const std::string &secret);

END_CENTAUR_NAMESPACE

#endif // FINANCEABLE_TOTP_HPP
