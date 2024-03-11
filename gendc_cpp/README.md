
## Build

- Use ninja

```
meson setup ../build
ninja -C build
```
- Use visual studio as back end
```powershell
 meson setup ../build --backend vs2022
 ```

- reconfigure 
 ```
 meson setup ../build --backend vs2022  --reconfigure -Dtests=enabled --pkg-config-path "C:\dev\vcpkg\installed\x64-windows\lib\pkgconfig"  
```

 ## Test

```
.\vcpkg.exe install check
```
- Run tests

```
meson test -C ../build/
```

 - Inspect

```powershell
$env:GST_PLUGIN_PATH="$env:GST_PLUGIN_PATH;d:\Workspace\Sensing-Dev\GenDC\build\gst\gendcparse"
```
```powershell
gst-inspect-1.0 gendcparse
```


