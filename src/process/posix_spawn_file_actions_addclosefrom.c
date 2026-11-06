/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <spawn.h>
#include <stdlib.h>
#include <errno.h>
#include "fdop.h"
#include <unsupported_api.h>

int posix_spawn_file_actions_addclosefrom_np(posix_spawn_file_actions_t *fa, int from)
{
	if (from < 0) return EBADF;
	UNSUPPORTED_API_VOID(LITEOS_A);
	struct fdop *op = malloc(sizeof *op);
	if (!op) return ENOMEM;
	op->cmd = FDOP_CLOSEFROM;
	op->fd = from;
	if ((op->next = fa->__actions)) op->next->prev = op;
	op->prev = 0;
	fa->__actions = op;
	return 0;
}
