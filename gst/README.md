## Prerequisite for Windows

### Install pkg-config

Let `%PKG_CONFIG_INSTALL%` be the directory you will install pkg-config

```bash
winget install bloodrock.pkg-config-lite --location %PKG_CONFIG_INSTALL%
```

This usually update Path environment variable and add command line alias `pkg-config`.

### Install vcpkg

Let `%VCPKG_INSTALL%/vcpkg` be the directory you will install vcpkg

```bash
mkdir %VCPKG_INSTALL% && cd %VCPKG_INSTALL%
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
```
### Install dependencies of gendc gst-plugin

Let `%GENDC%` be the root of this repository. i.e. This file is `%GENDC%/gst/README.md`

```bash
cd %GENDC%/gst
%VCPKG_INSTALL%/vcpkg/vcpkg.exe install
```

This will generate `%GENDC%/gst/vcpkg_installed` and `%GENDC%/gst/vcpkg_installed/x64-windows` contains all dependencies.

## Build Gst Plugin

Let `%GENDC%` be the root of this repository. i.e. This file is `%GENDC%/gst/README.md`

Also, `%GENDC_INSTALL%` is where you want to install gst-plugin in the next step. It has to be the absolute path.

```bash
set PATH=%GENDC%/vcpkg_installed/x64-windows/bin;%PATH%
set PKG_CONFIG_PATH=%GENDC%/gst/vcpkg_installed/x64-windows/lib/pkgconfig;%PKG_CONFIG_PATH%
cd %GENDC%/gst
meson setup --prefix %GENDC_INSTALL% build
```

If pkg-config is not found, add it to PATH.
```bash
set PATH=%PKG_CONFIG_INSTALL%/pkg-config-lite-0.28-1/bin;%PATH%
```

## Install Gst Plugin
```bash
cd build
meson install
set GST_PLUGIN_PATH=%GENDC_INSTALL%
```

## Test 

### Environment variables

```bash
set GST_PLUGIN_PATH=%GENDC_INSTALL%;%GST_PLUGIN_PATH%
set GST_PLUGIN_PATH=%GENDC%/gst/vcpkg_installed/x64-windows/plugins/gstreamer;%GST_PLUGIN_PATH%
```

### Tools:

* `%GENDC%/gst/vcpkg_installed/x64-windows/tools/gstreamer/gst-inspect-1.0.exe`
* `%GENDC%/gst/vcpkg_installed/x64-windows/tools/gstreamer/gst-launch-1.0.exe`

```bash
set PATH=%GENDC%/gst/vcpkg_installed/x64-windows/tools/gstreamer;%PATH%
```

### Data:

* Single-container file
  * Single valid component: https://drive.google.com/drive/folders/1R8J__41lxCdlHPWs30wdqVF1phZpIB0w?usp=drive_link
  * Multiple valid componens: `test/generated_stub/output.bin`

* Multiple-container file
  * Single valid component: https://drive.google.com/drive/folders/1R8J__41lxCdlHPWs30wdqVF1phZpIB0w?usp=drive_link 
  * Multiple valid componens: TBD

* Non-gendc file
  * 120-gendc.bin: https://drive.google.com/drive/folders/1R8J__41lxCdlHPWs30wdqVF1phZpIB0w?usp=drive_link 

```bash
python test/test.py -i  <gendc file bin> -o <output directory> -n <num components>
```