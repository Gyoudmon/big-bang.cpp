# Windows 下使用 vcpkg 安装 SDL2

下载 vcpkg 放到 `C:\opt\vcpkg`

```cmd
> git clone https://github.com/microsoft/vcpkg C:\opt\vcpkg
> C:\opt\vcpkg\bootstrap-vcpkg.bat
```

安装 64bit SDL2

```cmd
> C:\opt\vcpkg\vcpkg install sdl2 sdl2_ttf sdl2_image sdl2_gfx sdl2_mixer sdl2_net --triplet=x64-windows
```

用管理员身份运行(普通用户运行时可现场授权，无需另开终端)

```cmd
> .\vcpkg\vcpkg integrate install
```

搞定
