# Super Mario Bros themed T-Watch Firmware

[![Preview](simulator/mariowasm_preview.png)](https://htmlpreview.github.io/?https://raw.githubusercontent.com/wizche/ttgo-smb/master/simulator/lvgl.html)

[Check it out directly in your browser thanks to WebAssembly!](https://htmlpreview.github.io/?https://raw.githubusercontent.com/wizche/ttgo-smb/master/simulator/lvgl.html)

If the link doesnt work try this altenative (which uses CDN cache and may not be up-to-date): https://rawcdn.githack.com/wizche/ttgo-smb/cfadd569e8d008482849b5b52bc6d1bd94ee15d6/simulator/lvgl.html

## Build

### TTGO T-Watch 2020

This project uses [PlatformIO](https://platformio.org/), so building and deploying the firmware should be as simple as selecting `Build` and `Upload` in PlatformIO tasks.


### WebAssembly LVGL simulator
In order to build the webassembly target we first need to install the emscripten toolchain:
```
# Get the emsdk repo
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
# install it
./emsdk install latest
 # Activate latest SDK for the current user
./emsdk activate latest
# activate PATH and other envs
source ./emsdk_env.sh
```

Then we can go into the repo `wasm` folder and just run `make -j`.

If everything goes well it should generate `lvgl.{html, js, wasm}` assets in the same directory. You should use the `lvgl.hml` since it contains initialization javascript for the wasm module. Just copy the other two assets: `cp lvgl.{js,wasm} ../simulator`.


## Credits
* Emulogic - Truetype Font from Freaky Fonts
* Sprites from [The Spriters Resource](https://www.spriters-resource.com/nes/supermariobros/)
* [PineTime Watch Faces in your Web Browser with WebAssembly](https://lupyuen.github.io/pinetime-rust-mynewt/articles/simulator)