browser.action.onClicked.addListener(async (tab) => {
        let pngURI = await browser.tabs.captureVisibleTab()


        await browser.scripting.executeScript({
                target: { tabId: tab.id },
                func: (wasmUrl, pngURI) => {
                        try {

                                let png = atob(pngURI.replace("data:image/png;base64,", ""));
                                // png is the image data hehe

                                const len = png.length;
                                const bytes = new Uint8Array(len);
                                for (let i = 0; i < len; i++) {
                                        bytes[i] = png.charCodeAt(i);
                                }
                                console.log(bytes.length);
                                let canvas = document.getElementById("mousse");

                                if (!canvas) {
                                        canvas = document.createElement("canvas");
                                        canvas.id = "mousse";
                                        canvas.style.position = "fixed";
                                        canvas.style.top = "0";
                                        canvas.style.left = "0";
                                        canvas.style.zIndex = "2147483647";
                                        document.body.appendChild(canvas);
                                }


                                var width = document.documentElement["clientWidth"];
                                var height = document.documentElement["clientHeight"];

                                let bg = getComputedStyle(document.body).backgroundColor;
                                if (bg === 'transparent' || bg === 'rgba(0, 0, 0, 0)' || bg.endsWith(', 0)')) {
                                        bg = getComputedStyle(document.documentElement).backgroundColor;
                                }

                                const matches = bg.match(/[\d.]+/g);
                                const r = parseInt(matches[0], 10);
                                const g = parseInt(matches[1], 10);
                                const b = parseInt(matches[2], 10);
                                const a = matches[3] ? parseFloat(matches[3]) : 1.0;
                                const a255 = Math.round(a * 255);

                                // https://emscripten.org/docs/api_reference/module.html
                                globalThis.Module = {
                                        canvas,
                                        arguments: [
                                                "" + height,
                                                "" + width,
                                                "" + r,
                                                "" + g,
                                                "" + b,
                                                "" + a255
                                        ],
                                        locateFile: (path) => path.endsWith(".wasm") ? wasmUrl : path,
                                        preRun: [() => {

                                                try {
                                                        FS.writeFile("/screenshot.png", bytes);
                                                } catch (e) {
                                                        console.log(e)
                                                }
                                        }]
                                };
                        } catch (e) { console.log(e) }

                },
                args: [browser.runtime.getURL("game.wasm"), pngURI]

        });


        await browser.scripting.executeScript({
                target: { tabId: tab.id },
                files: ["game.js"]
        });


});
