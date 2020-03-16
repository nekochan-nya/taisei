/*
 * This software is licensed under the terms of the MIT License.
 * See COPYING for further information.
 * ---
 * Copyright (c) 2011-2019, Lukas Weber <laochailan@web.de>.
 * Copyright (c) 2012-2019, Andrei Alexeyev <akari@taisei-project.org>.
*/

#ifndef IGUARD_dialog_dialog_interface_h
#define IGUARD_dialog_dialog_interface_h

#include "taisei.h"

#include "dialog.h"


#define DIALOG_SCRIPTS \
	WITH_EVENTS    (Stage1PreBoss,     (boss_appears, music_changes)) \
	WITHOUT_EVENTS (Stage1PostBoss)                                   \
	WITH_EVENTS    (Stage2PreBoss,     (boss_appears, music_changes)) \
	WITHOUT_EVENTS (Stage2PostBoss)                                   \
	WITH_EVENTS    (Stage3PreBoss,     (boss_appears, music_changes)) \
	WITHOUT_EVENTS (Stage3PostBoss)                                   \
	WITH_EVENTS    (Stage4PreBoss,     (boss_appears, music_changes)) \
	WITHOUT_EVENTS (Stage4PostBoss)                                   \
	WITH_EVENTS    (Stage5PreBoss,     (boss_appears, music_changes)) \
	WITHOUT_EVENTS (Stage5PostMidBoss)                                \
	WITHOUT_EVENTS (Stage5PostBoss)                                   \
	WITH_EVENTS    (Stage6PreBoss,     (boss_appears, music_changes)) \
	WITHOUT_EVENTS (Stage6PreFinal)                                   \
	WITH_EVENTS    (StageExPreBoss,    (boss_appears, music_changes)) \
	WITHOUT_EVENTS (StageExPostBoss)                                  \



#define WITH_EVENTS(_name, _events) \
	typedef COEVENTS_ARRAY _events _name##DialogEvents; \
	DEFINE_TASK_INTERFACE(_name##Dialog, { _name##DialogEvents **out_events; });

#define WITHOUT_EVENTS(_name) \
	WITH_EVENTS(_name, (_dummy_fake_event_))

/*
#define WITHOUT_EVENTS(_name) \
	typedef struct { char there_are_no_events; } _name##DialogEvents; \
	DEFINE_TASK_INTERFACE(_name##Dialog, { _name##DialogEvents **out_events; });
*/

DIALOG_SCRIPTS
#undef WITH_EVENTS
#undef WITHOUT_EVENTS

#define WITH_EVENTS(_name, _events) WITHOUT_EVENTS(_name)
#define WITHOUT_EVENTS(_name) \
	TASK_INDIRECT_TYPE(_name##Dialog) _name;

typedef struct PlayerDialogTasks {
	DIALOG_SCRIPTS
} PlayerDialogTasks;

#undef WITH_EVENTS
#undef WITHOUT_EVENTS

#endif // IGUARD_dialog_dialog_interface_h
