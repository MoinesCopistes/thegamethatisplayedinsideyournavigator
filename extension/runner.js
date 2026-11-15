window.addEventListener("message", (event) => {
        if (event.source !== parent || event.data?.type !== "mousse-start") return;

        const { pngURI, wasmUrl, width, height, r, g, b } = event.data;
        const png = atob(pngURI.replace("data:image/png;base64,", ""));
        const bytes = Uint8Array.from(png, character => character.charCodeAt(0));
        const canvas = document.getElementById("mousse");
        canvas.width = width;
        canvas.height = height;
        canvas.oncontextmenu = event => event.preventDefault();
        canvas.addEventListener("click", () => canvas.focus());
        canvas.focus();

        globalThis.Module = {
                canvas,
                arguments: [String(height), String(width), String(r), String(g), String(b)],
                locateFile: path => path.endsWith(".wasm") ? wasmUrl : path,
                preRun: [() => FS.writeFile("/screenshot.png", bytes)],
                postRun: [() => {
                        // Let the first game frame paint before replacing the
                        // live webpage with the iframe.
                        requestAnimationFrame(() => requestAnimationFrame(() => {
                                parent.postMessage({ type: "mousse-ready" }, "*");
                        }));
                }],
                keyboardListeningElement: canvas
        };

        const script = document.createElement("script");
        script.src = "game.js";
        document.body.appendChild(script);
}, { once: true });
