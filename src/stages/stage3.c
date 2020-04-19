/*
 * This software is licensed under the terms of the MIT License.
 * See COPYING for further information.
 * ---
 * Copyright (c) 2011-2019, Lukas Weber <laochailan@web.de>.
 * Copyright (c) 2012-2019, Andrei Alexeyev <akari@taisei-project.org>.
 */

#include "taisei.h"

#include "stage3.h"
#include "stage3_events.h"

#include "global.h"
#include "stage.h"
#include "stageutils.h"

PRAGMA(message "Remove when this stage is modernized")
DIAGNOSTIC(ignored "-Wdeprecated-declarations")

/*
 *  See the definition of AttackInfo in boss.h for information on how to set up the idmaps.
 *  To add, remove, or reorder spells, see this stage's header file.
 */

struct stage3_spells_s stage3_spells = {
	.mid = {
		.deadly_dance = {
			{ 0,  1,  2,  3}, AT_SurvivalSpell, "Venom Sign “Deadly Dance”", 14, 40000,
			scuttle_deadly_dance, scuttle_spellbg, BOSS_DEFAULT_GO_POS, 3
		},
	},

	.boss = {
		.moonlight_rocket = {
			{ 6,  7,  8,  9}, AT_Spellcard, "Firefly Sign “Moonlight Rocket”", 40, 40000,
			wriggle_moonlight_rocket, wriggle_spellbg, BOSS_DEFAULT_GO_POS, 3
		},
		.wriggle_night_ignite = {
			{10, 11, 12, 13}, AT_Spellcard, "Light Source “Wriggle Night Ignite”", 50, 46000,
			wriggle_night_ignite, wriggle_spellbg, BOSS_DEFAULT_GO_POS, 3
		},
		.firefly_storm = {
			{14, 15, 16, 17}, AT_Spellcard, "Bug Sign “Firefly Storm”", 45, 45000,
			wriggle_firefly_storm, wriggle_spellbg, BOSS_DEFAULT_GO_POS, 3
		},
	},

	.extra.light_singularity = {
		{ 0,  1,  2,  3}, AT_ExtraSpell, "Lamp Sign “Light Singularity”", 75, 45000,
		wriggle_light_singularity, wriggle_spellbg, BOSS_DEFAULT_GO_POS, 3
	},
};

static uint stage3_bg_pos(Stage3D *s3d, vec3 pos, float maxrange) {
	vec3 p = {
		0,
		0,
		0,
	};
	// minus sign for drawing order
	vec3 r = {0, -1000, -500};

	return linear3dpos(s3d, pos, maxrange, p, r);
}

static void stage3_bg_ground_draw(vec3 pos) {

	r_mat_mv_push();
	r_mat_mv_translate(pos[0], pos[1], pos[2]);
	r_mat_mv_scale(50,50,50);

	r_shader_standard();
	r_uniform_sampler("tex", "stage3/ground");
	r_draw_model("stage3/ground");
	r_uniform_sampler("tex", "stage3/rocks");
	r_draw_model("stage3/rocks");
	r_mat_mv_pop();
}

static void stage3_bg_leaves_draw(vec3 pos) {
	r_mat_mv_push();
	r_mat_mv_translate(pos[0], pos[1], pos[2]);
	r_mat_mv_scale(50,50,50);
	r_mat_mv_translate(0,0,-0.01);
	r_uniform_sampler("tex", "stage3/leaves");
	r_draw_model("stage3/leaves");

	r_mat_mv_pop();
}

static bool stage3_fog(Framebuffer *fb) {
	r_shader("zbuf_fog");
	r_uniform_sampler("depth", r_framebuffer_get_attachment(fb, FRAMEBUFFER_ATTACH_DEPTH));
	r_uniform_vec4("fog_color", 0.5, 0.7, 1, 1.0);
	r_uniform_float("start", 0.6);
	r_uniform_float("end", 5);
	r_uniform_float("exponent", 10);
	r_uniform_float("sphereness", 0);
	draw_framebuffer_tex(fb, VIEWPORT_W, VIEWPORT_H);
	r_shader_standard();
	return true;
}

static bool stage3_glitch(Framebuffer *fb) {
	float strength;

	if(global.boss && global.boss->current && ATTACK_IS_SPELL(global.boss->current->type) && !strcmp(global.boss->name, "Scuttle")) {
		strength = 0.05 * max(0, (global.frames - global.boss->current->starttime) / (double)global.boss->current->timeout);
	} else {
		strength = 0.0;
	}

	if(strength > 0) {
		r_shader("glitch");
		r_uniform_float("strength", strength);
		r_uniform_float("frames", global.frames + 15 * nfrand());
	} else {
		return false;
	}

	draw_framebuffer_tex(fb, VIEWPORT_W, VIEWPORT_H);
	r_shader_standard();
	return true;
}

static void stage3_start(void) {
	stage3d_init(&stage_3d_context, 16);

	stage_3d_context.cx[1] = 800;
	stage_3d_context.crot[0] = -95;
	stage_3d_context.cv[1] = -5;
	stage_3d_context.cv[2] = -2.5;

}

static void stage3_preload(void) {
	preload_resources(RES_BGM, RESF_OPTIONAL, "stage3", "stage3boss", NULL);
	preload_resources(RES_SPRITE, RESF_DEFAULT,
		"stage3/ground",
		"stage3/rocks",
		"stage3/leaves",
		"stage3/spellbg1",
		"stage3/spellbg2",
		"stage3/wspellbg",
		"stage3/wspellclouds",
		"stage3/wspellswarm",
		"dialog/wriggle",
		"dialog/scuttle",
	NULL);
	preload_resources(RES_MODEL, RESF_DEFAULT,
		"stage3/ground",
		"stage3/rocks",
		"stage3/leaves",
	NULL);
	preload_resources(RES_SHADER_PROGRAM, RESF_DEFAULT,
		"zbuf_fog",
		"glitch",
		"maristar_bombbg",
	NULL);
	preload_resources(RES_SHADER_PROGRAM, RESF_OPTIONAL,
		"lasers/accelerated",
		"lasers/sine_expanding",
	NULL);
	preload_resources(RES_ANIM, RESF_DEFAULT,
		"boss/scuttle",
		"boss/wriggleex",
	NULL);
	preload_resources(RES_SFX, RESF_OPTIONAL,
		"laser1",
	NULL);
}

static void stage3_end(void) {
	stage3d_shutdown(&stage_3d_context);
}

static void stage3_draw(void) {
	r_mat_proj_perspective(STAGE3D_DEFAULT_FOVY, STAGE3D_DEFAULT_ASPECT, 30, 3000);
	stage3d_draw(&stage_3d_context, 6000, 2, (Stage3DSegment[]) { stage3_bg_ground_draw, stage3_bg_pos, stage3_bg_leaves_draw, stage3_bg_pos });
}

static void stage3_update(void) {
	if(dialog_is_active(global.dialog)) {
		stage3d_update(&stage_3d_context);
		return;
	}

	// 2740 - MIDBOSS

	int midboss_time = STAGE3_MIDBOSS_TIME;

	stage3d_update(&stage_3d_context);
}

void scuttle_spellbg(Boss*, int t);

static void stage3_spellpractice_start(void) {
	stage3_start();

	if(global.stage->spell->draw_rule == scuttle_spellbg) {
		skip_background_anim(stage3_update, 2800, &global.timer, NULL);
		global.boss = stage3_spawn_scuttle(BOSS_DEFAULT_SPAWN_POS);
		stage_unlock_bgm("scuttle");
		stage_start_bgm("scuttle");
	} else {
		skip_background_anim(stage3_update, 5300 + STAGE3_MIDBOSS_TIME, &global.timer, NULL);
		global.boss = stage3_spawn_wriggle_ex(BOSS_DEFAULT_SPAWN_POS);
		stage_start_bgm("stage3boss");
	}

	boss_add_attack_from_info(global.boss, global.stage->spell, true);
	boss_start_attack(global.boss, global.boss->attacks);
}

static void stage3_spellpractice_events(void) {
}

void stage3_skip(int t) {
	skip_background_anim(stage3_update, t, &global.timer, &global.frames);
	audio_music_set_position(global.timer / (double)FPS);
}

ShaderRule stage3_shaders[] = { stage3_fog, NULL };
ShaderRule stage3_postprocess[] = { stage3_glitch, NULL };

StageProcs stage3_procs = {
	.begin = stage3_start,
	.preload = stage3_preload,
	.end = stage3_end,
	.draw = stage3_draw,
	.update = stage3_update,
	.event = stage3_events,
	.shader_rules = stage3_shaders,
	.postprocess_rules = stage3_postprocess,
	.spellpractice_procs = &stage3_spell_procs,
};

StageProcs stage3_spell_procs = {
	.begin = stage3_spellpractice_start,
	.preload = stage3_preload,
	.end = stage3_end,
	.draw = stage3_draw,
	.update = stage3_update,
	.event = stage3_spellpractice_events,
	.shader_rules = stage3_shaders,
	.postprocess_rules = stage3_postprocess,
};
