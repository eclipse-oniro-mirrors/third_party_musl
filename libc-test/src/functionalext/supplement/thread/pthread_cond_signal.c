/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <pthread.h>
#include "functionalext.h"

const int SUCCESS = 0;

/**
 * @tc.name      : pthread_cond_signal_0100
 * @tc.desc      : The pthread_cond_signal parameter is valid and returns correctly.
 * @tc.level     : Level 0
 */
void pthread_cond_signal_0100(void)
{
    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
    int back = pthread_cond_signal(&cond);
    EXPECT_EQ("pthread_cond_signal_0100", back, SUCCESS);
}

int main(int argc, char *argv[])
{
    pthread_cond_signal_0100();

    return t_status;
}