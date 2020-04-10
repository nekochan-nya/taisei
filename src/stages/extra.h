/*
 * This software is licensed under the terms of the MIT License.
 * See COPYING for further information.
 * ---
 * Copyright (c) 2011-2019, Lukas Weber <laochailan@web.de>.
 * Copyright (c) 2012-2019, Andrei Alexeyev <akari@taisei-project.org>.
 */

#ifndef IGUARD_stages_extra_h
#define IGUARD_stages_extra_h

#include "taisei.h"

#include "stage.h"

extern StageProcs extra_procs;

Boss *stagex_spawn_yumemi(cmplx pos);
void extra_draw_yumemi_portrait_overlay(SpriteParams *sp);

#endif // IGUARD_stages_extra_h
