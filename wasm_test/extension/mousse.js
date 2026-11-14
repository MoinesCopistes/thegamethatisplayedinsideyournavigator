browser.action.onClicked.addListener(async (tab) => {
        await browser.scripting.executeScript({
                target: { tabId: tab.id },
                func: (wasmUrl) => {
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
                        // https://emscripten.org/docs/api_reference/module.html
                        globalThis.Module = {
                                canvas,
                                arguments: ["" + height, "" + width],
                                locateFile: (path) => path.endsWith(".wasm") ? wasmUrl : path
                        };

                },
                args: [browser.runtime.getURL("game.wasm")]
        });


        await browser.scripting.executeScript({
                target: { tabId: tab.id },
                files: ["game.js"]
        });


});
