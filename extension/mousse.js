async function start() {
        const [tab] = await chrome.tabs.query({
                active: true,
                lastFocusedWindow: true
        });

        // Capture before adding the game frame so the screenshot only contains
        // the page. Removing the previous frame also destroys its WASM runtime.
        await browser.scripting.executeScript({
                target: { tabId: tab.id },
                func: () => {
                        globalThis.__mousseRevealCleanup?.();
                        document.getElementById("mousse-frame")?.remove();
                }
        });
        const pngURI = await browser.tabs.captureVisibleTab();

        await browser.scripting.executeScript({
                target: { tabId: tab.id },
                func: (runnerUrl, wasmUrl, pngURI) => {
                        try {
                                const width = document.documentElement.clientWidth;
                                const height = document.documentElement.clientHeight;
                                let bg = getComputedStyle(document.body).backgroundColor;
                                if (bg === "transparent" || bg === "rgba(0, 0, 0, 0)" || bg.endsWith(", 0)")) {
                                        bg = getComputedStyle(document.documentElement).backgroundColor;
                                }
                                if (bg === "transparent" || bg === "rgba(0, 0, 0, 0)" || bg.endsWith(", 0)")) {
                                        bg = "rgb(255, 255, 255)";
                                }
                                const [r, g, b] = bg.match(/[\d.]+/g).map(Number);

                                const frame = document.createElement("iframe");
                                frame.id = "mousse-frame";
                                frame.src = runnerUrl;
                                frame.style.cssText = "position:fixed;inset:0;width:100vw;height:100vh;border:0;z-index:2147483647;background:transparent;opacity:0;pointer-events:none";
                                const revealFrame = event => {
                                        if (event.source !== frame.contentWindow || event.data?.type !== "mousse-ready") return;
                                        window.removeEventListener("message", revealFrame);
                                        delete globalThis.__mousseRevealCleanup;
                                        frame.style.opacity = "1";
                                        frame.style.pointerEvents = "auto";
                                        frame.contentWindow.focus();
                                };
                                window.addEventListener("message", revealFrame);
                                globalThis.__mousseRevealCleanup = () => window.removeEventListener("message", revealFrame);
                                frame.addEventListener("load", () => {
                                        frame.contentWindow.postMessage({
                                                type: "mousse-start",
                                                pngURI, wasmUrl, width, height, r, g, b
                                        }, new URL(runnerUrl).origin);
                                }, { once: true });
                                document.documentElement.appendChild(frame);
                        } catch (error) {
                                console.error("Could not start Mousse", error);
                        }
                },
                args: [
                        browser.runtime.getURL("runner.html"),
                        browser.runtime.getURL("game.wasm"),
                        pngURI
                ]
        });

        window.close();
}

document.getElementById("start").addEventListener("click", start);
