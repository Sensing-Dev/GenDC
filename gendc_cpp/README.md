
## Build

- Use visual studio as back end
```powershell
 meson setup ../build --backend vs2022
 ```

 ## Test
 ```
.\vcpkg.exe install check
```

```
 meson setup ../build --backend vs2022  --reconfigure -Dtests=enabled --pkg-config-path "C:\dev\vcpkg\installed\x64-windows\lib\pkgconfig"  
```


 - Inspect
	- 
```powershell
gst-inspect-1.0 gendcparse
```