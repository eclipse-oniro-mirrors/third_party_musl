/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <stdint.h>
#include "functionalext.h"
#include "math.h"

const int32_t NUM_ZERO = 0;
const int32_t NUM_ONE = 1;

/**
 * @tc.name      : finitef_0100
 * @tc.desc      : Validate that the input parameter is a finite value (parameter is 0.0).
 * @tc.level     : Level 0
 */
void finitef_0100(void)
{
    float ret = finitef(0.0);
    EXPECT_EQ("finitef_0100", ret, NUM_ONE);
}

/**
 * @tc.name      : finitef_0200
 * @tc.desc      : Validate that the input parameter is a finite value (parameter is DBL_MIN/2).
 * @tc.level     : Level 0
 */
void finitef_0200(void)
{
    float ret = finitef(DBL_MIN / 2);
    EXPECT_EQ("finitef_0100", ret, NUM_ONE);
}

/**
 * @tc.name      : finitef_0300
 * @tc.desc      : Verify that the input parameter is not a finite value (parameter is NAN).
 * @tc.level     : Level 2
 */
void finitef_0300(void)
{
    float ret = finitef(NAN);
    EXPECT_EQ("finitef_0100", ret, NUM_ZERO);
}

/**
 * @tc.name      : finitef_0400
 * @tc.desc      : Verify that the input parameter is not a finite value (parameter is INFINITY).
 * @tc.level     : Level 2
 */
void finitef_0400(void)
{
    float ret = finitef(INFINITY);
    EXPECT_EQ("finitef_0100", ret, NUM_ZERO);
}

/**
 * @tc.name      : finitef_0500
 * @tc.desc      : Verify that the input parameter is not a finite value (parameter is sqrt(-1.0)).
 * @tc.level     : Level 2
 */
void finitef_0500(void)
{
    float ret = finitef(sqrt(-1.0));
    EXPECT_EQ("finitef_0100", ret, NUM_ZERO);
}

/**
 * @tc.name      : finitef_0600
 * @tc.desc      : Verify that the input parameter is not a finite value (exp(800)).
 * @tc.level     : Level 2
 */
void finitef_0600(void)
{
    float ret = finitef(exp(800));
    EXPECT_EQ("finitef_0100", ret, NUM_ZERO);
}

int main(int argc, char *argv[])
{
    finitef_0100();
    finitef_0200();
    finitef_0300();
    finitef_0400();
    finitef_0500();
    finitef_0600();
    return t_status;
}