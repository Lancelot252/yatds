const trackEl = document.querySelector("#track");
const launcherEl = document.querySelector("#launcher");
const messageEl = document.querySelector("#message");
const scoreEl = document.querySelector("#score");
const turnsEl = document.querySelector("#turns");
const maxTurnsEl = document.querySelector("#maxTurns");
const bombBtn = document.querySelector("#bombBtn");
const shuffleBtn = document.querySelector("#shuffleBtn");
const undoBtn = document.querySelector("#undoBtn");
const resetBtn = document.querySelector("#resetBtn");

let selectedIndex = 0;

const colorName = {
  R: "红",
  G: "绿",
  B: "蓝",
  Y: "黄",
  P: "紫",
  M: "玫"
};

async function api(path) {
  const response = await fetch(path);
  const payload = await response.json();
  if (!payload.ok) {
    render(payload.state);
    throw new Error(payload.error || "请求失败");
  }
  render(payload.state);
}

function ball(color, index, selectable) {
  const item = document.createElement("button");
  item.type = "button";
  item.className = `ball ${color}${selectable && index === selectedIndex ? " selected" : ""}`;
  item.textContent = colorName[color] || color;
  item.title = `${index}：${colorName[color] || color}色彩球`;
  if (selectable) {
    item.addEventListener("click", () => {
      selectedIndex = index;
      render(window.currentState);
    });
  }
  return item;
}

function slot(position) {
  const item = document.createElement("button");
  item.type = "button";
  item.className = "slot";
  item.title = `插入到位置 ${position}`;
  item.addEventListener("click", () => api(`/api/insert?pos=${position}`).catch(showError));
  return item;
}

function render(state) {
  window.currentState = state;
  selectedIndex = Math.min(selectedIndex, Math.max(state.track.length - 1, 0));

  trackEl.innerHTML = "";
  for (let i = 0; i <= state.track.length; i += 1) {
    trackEl.appendChild(slot(i));
    if (i < state.track.length) trackEl.appendChild(ball(state.track[i], i, true));
  }

  launcherEl.innerHTML = "";
  state.launcher.slice(0, 18).forEach((color) => launcherEl.appendChild(ball(color, 0, false)));
  if (state.launcher.length > 18) {
    const more = document.createElement("span");
    more.textContent = `+${state.launcher.length - 18}`;
    more.className = "queue-more";
    launcherEl.appendChild(more);
  }

  messageEl.textContent = state.message;
  scoreEl.textContent = state.score;
  turnsEl.textContent = state.turns;
  maxTurnsEl.textContent = state.maxTurns;

  bombBtn.textContent = `使用炸弹 (${state.bombs})`;
  shuffleBtn.textContent = `重排队列 (${state.shuffles})`;
  bombBtn.disabled = state.gameOver || state.bombs <= 0 || state.track.length === 0;
  shuffleBtn.disabled = state.gameOver || state.shuffles <= 0;
  undoBtn.disabled = !state.undo;

  if (state.gameOver) {
    messageEl.textContent = state.won ? `${state.message} 本局胜利。` : `${state.message} 本局失败。`;
  }
}

function showError(error) {
  messageEl.textContent = error.message;
}

bombBtn.addEventListener("click", () => api(`/api/bomb?pos=${selectedIndex}`).catch(showError));
shuffleBtn.addEventListener("click", () => api("/api/shuffle").catch(showError));
undoBtn.addEventListener("click", () => api("/api/undo").catch(showError));
resetBtn.addEventListener("click", () => api("/api/reset").catch(showError));

api("/api/state").catch(showError);
