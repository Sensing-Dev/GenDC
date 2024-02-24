# Test executable
test_exe = executable('test_gstplugin',
    sources: ['test-gendcparse.cpp'],
    dependencies: [check_dep, gst_dep, gst_base_dep, plugins]
)

# GTest and GStreamer initialization
test('gendcparse-test', test_exe)
