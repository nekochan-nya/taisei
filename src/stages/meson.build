
stages_src = files(
    'stage1.c',
    'stage1_events.c',
    'stage2.c',
    'stage2_events.c',
    'stage3.c',
    'stage3_events.c',
    'stage4.c',
    'stage4_events.c',
    'stage5.c',
    'stage5_events.c',
    'stage6.c',
    'stage6_events.c',
    'extra.c',
)

if is_developer_build
    stages_src += files(
        'dpstest.c',
    )
endif

stages_src += files('corotest.c')
