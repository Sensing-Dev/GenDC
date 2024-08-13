## Simple

./gst-gendc-launch.in filesrc location=input.bin ! gendcseparator ! filesink location=output/

## Separator

./gst-gendc-launch.in filesrc location=input.bin ! gendcseparator name=sep ! queue ! filesink location=output/descriptor.bin \
sep.component_src0 ! queue max-size-buffers=1000 ! filesink location=output/component0.bin \
sep.component_src1 ! queue max-size-buffers=1000 ! filesink location=output/component1.bin \
sep.component_src2 ! queue max-size-buffers=1000 ! filesink location=output/component2.bin \
sep.component_src3 ! queue max-size-buffers=1000 ! filesink location=output/component3.bin \
sep.component_src4 ! queue max-size-buffers=1000 ! filesink location=output/component4.bin \
sep.component_src5 ! queue max-size-buffers=1000 ! filesink location=output/component5.bin