const stateEls = {
  score: document.getElementById("score"),
  bombs: document.getElementById("bombs"),
  status: document.getElementById("status"),
  track: document.getElementById("track"),
  queue: document.getElementById("queue"),
  nextBall: document.getElementById("nextBall"),
};

const buttons = {
  undo: document.getElementById("undoBtn"),
  bomb: document.getElementById("bombBtn"),
  reset: document.getElementById("resetBtn"),
};

let bombMode = false;

const colorClass = (c) => `ball ${c}`;

const request = async (path, options = {}) => {
  const res = await fetch(path, {
    headers: { "Content-Type": "application/json" },
    ...options,
  });

  if (!res.ok) {
    const text = await res.text();
    throw new Error(text || "Request failed");
  }

  if (res.headers.get("content-type")?.includes("application/json")) {
    return res.json();
  }
  return null;
};

const render = (state) => {
  stateEls.score.textContent = state.score;
  stateEls.bombs.textContent = state.bombs;
  stateEls.status.textContent = state.status;

  stateEls.track.innerHTML = "";
  const track = state.track.split("");

  for (let i = 0; i <= track.length; i += 1) {
    const slot = document.createElement("div");
    slot.className = "slot";
    slot.dataset.pos = String(i);
    slot.addEventListener("click", () => insertBall(i));
    stateEls.track.appendChild(slot);

    if (i < track.length) {
      const ball = document.createElement("div");
      ball.className = colorClass(track[i]);
      ball.title = `Position ${i}`;
      ball.addEventListener("click", (event) => {
        if (bombMode) {
          event.stopPropagation();
          useBomb(i);
        }
      });
      stateEls.track.appendChild(ball);
    }
  }

  stateEls.queue.innerHTML = "";
  state.queue.split("").forEach((c) => {
    const ball = document.createElement("div");
    ball.className = colorClass(c);
    stateEls.queue.appendChild(ball);
  });

  stateEls.nextBall.className = colorClass(state.next || "");
};

const insertBall = async (pos) => {
  if (bombMode) {
    return;
  }
  const state = await request("/api/shoot", {
    method: "POST",
    body: JSON.stringify({ pos }),
  });
  render(state);
};

const useBomb = async (pos) => {
  const state = await request("/api/bomb", {
    method: "POST",
    body: JSON.stringify({ pos }),
  });
  render(state);
};

const refresh = async () => {
  const state = await request("/api/state");
  render(state);
};

buttons.undo.addEventListener("click", async () => {
  const state = await request("/api/undo", { method: "POST" });
  render(state);
});

buttons.reset.addEventListener("click", async () => {
  const state = await request("/api/reset", { method: "POST" });
  render(state);
});

buttons.bomb.addEventListener("click", () => {
  bombMode = !bombMode;
  buttons.bomb.textContent = bombMode ? "Bomb Mode: ON" : "Bomb Mode";
  buttons.bomb.classList.toggle("danger", bombMode);
});

refresh().catch((err) => {
  stateEls.status.textContent = "Server offline";
  console.error(err);
});
