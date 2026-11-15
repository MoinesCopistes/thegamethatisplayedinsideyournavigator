async function start(tabId, levelId = 1) {
        const tab = tabId ? await browser.tabs.get(tabId) : (await browser.tabs.query({
                active: true,
                lastFocusedWindow: true
        }))[0];

        await browser.tabs.update(tab.id, { active: true });

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
                func: (runnerUrl, wasmUrl, pngURI, levelId) => {
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
                                        frame.style.opacity = "1";
                                        frame.style.pointerEvents = "auto";
                                        frame.contentWindow.focus();
                                };
                                const removeFrame = event => {
                                        if (event.source !== frame.contentWindow || event.data?.type !== "mousse-dead") return;
                                        globalThis.__mousseRevealCleanup?.();
                                        frame.remove();
                                        /*browser.runtime.sendMessage({ type: "mousse-dead" }); */
                                };
                                const nextLevelFrame = event => {

                                        if (event.source !== frame.contentWindow || event.data?.type !== "mousse-next") return;

                                        globalThis.__mousseRevealCleanup?.();
                                        frame.remove();
                                        browser.runtime.sendMessage({
                                                type: "mousse-next",
                                                levelId: levelId + 1
                                        });
                                }
                                window.addEventListener("message", revealFrame);
                                window.addEventListener("message", nextLevelFrame);
                                window.addEventListener("message", removeFrame);
                                globalThis.__mousseRevealCleanup = () => {
                                        window.removeEventListener("message", revealFrame);
                                        window.removeEventListener("message", nextLevelFrame);
                                        window.removeEventListener("message", removeFrame);
                                        delete globalThis.__mousseRevealCleanup;
                                };
                                frame.addEventListener("load", () => {
                                        frame.contentWindow.postMessage({
                                                type: "mousse-start",
                                                pngURI, wasmUrl, width, height, r, g, b, levelId
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
                        pngURI,
                        levelId
                ]
        });

        if (typeof window !== "undefined") window.close();
}

browser.runtime.onMessage.addListener((message, sender) => {
        if (message?.type !== "mousse-next" || !sender.tab) return;

        return browser.tabs.query({ windowId: sender.tab.windowId }).then(tabs => {
                const candidates = tabs
                        .filter(tab => tab.id !== sender.tab.id && !tab.url?.startsWith("about:"))
                        .sort(() => Math.random() - 0.5);
                const nextTab = candidates.pop();
                if (nextTab) return start(nextTab.id, message.levelId);
        });
});

if (typeof document !== "undefined") {
        document.getElementById("start")?.addEventListener("click", () => start());
}
