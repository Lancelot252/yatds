// 祖玛前端：维护 UI 状态，通过 REST 与 C++ 后端通信。
// 三种交互模式：
//  - shoot   : 点击轨道间隙将当前球插入该处
//  - bomb    : 点击某个球，引爆其周围三球
//  - recolor : 点击某个球，弹出颜色选择器修改其颜色

const state = {
  game: null,           // 后端返回的最新游戏状态
  mode: 'shoot',        // shoot | bomb | recolor
  pendingRecolorPos: -1,
};

const el = (id) => document.getElementById(id);

async function api(path, body) {
  const opt = { method: body ? 'POST' : 'GET' };
  if (body) {
    opt.headers = { 'Content-Type': 'application/json' };
    opt.body = JSON.stringify(body);
  }
  const r = await fetch(path, opt);
  return r.json();
}

function logMessage(msg, cls = '') {
  const box = el('message-log');
  const div = document.createElement('div');
  div.textContent = msg;
  if (cls) div.className = cls;
  box.appendChild(div);
  box.scrollTop = box.scrollHeight;
}

function setMode(mode) {
  state.mode = mode;
  const labels = { shoot: '发射（点击间隙插入）', bomb: '炸弹（点击某球引爆）', recolor: '变色（点击某球选色）' };
  el('mode-hint').textContent = '模式：' + labels[mode];
  for (const id of ['btn-bomb', 'btn-recolor']) el(id).classList.remove('active');
  if (mode === 'bomb') el('btn-bomb').classList.add('active');
  if (mode === 'recolor') el('btn-recolor').classList.add('active');
}

function renderState(s) {
  state.game = s;
  // 状态徽章
  const badge = el('status-label');
  badge.className = 'badge';
  if (s.status === 'won') { badge.classList.add('badge-won'); badge.textContent = '胜利！'; }
  else if (s.status === 'lost') { badge.classList.add('badge-lost'); badge.textContent = '失败'; }
  else { badge.classList.add('badge-playing'); badge.textContent = '游戏中'; }

  el('score').textContent = s.score;
  el('bomb-uses').textContent = s.bombUses;
  el('recolor-uses').textContent = s.recolorUses;
  el('skip-uses').textContent = s.skipUses;
  el('btn-undo').disabled = !s.undoAvailable;
  el('btn-bomb').disabled = s.bombUses <= 0 || s.status !== 'playing';
  el('btn-recolor').disabled = s.recolorUses <= 0 || s.status !== 'playing';
  el('btn-skip').disabled = s.skipUses <= 0 || s.status !== 'playing';

  // 轨道：轨道球之间穿插点击插槽
  const track = el('track');
  track.innerHTML = '';
  const playable = s.status === 'playing';
  const balls = s.track;

  const addSlot = (pos) => {
    if (state.mode !== 'shoot' || !playable) return;
    const slot = document.createElement('div');
    slot.className = 'slot';
    slot.title = `插入到位置 ${pos}`;
    slot.onclick = () => shoot(pos);
    track.appendChild(slot);
  };

  addSlot(0);
  balls.forEach((c, i) => {
    const b = document.createElement('div');
    b.className = `ball color-${c}`;
    if (state.mode === 'bomb' && playable) {
      b.classList.add('bomb-target');
      b.onclick = () => doBomb(i);
    } else if (state.mode === 'recolor' && playable) {
      b.classList.add('recolor-target');
      b.onclick = () => openRecolorPicker(i);
    }
    track.appendChild(b);
    addSlot(i + 1);
  });

  // 当前球
  const cur = el('current-ball');
  cur.className = 'ball ball-current';
  if (s.pending.length > 0) cur.classList.add(`color-${s.pending[0]}`);
  else cur.classList.add('color-0');

  // 队列其余
  const pendingBox = el('pending');
  pendingBox.innerHTML = '';
  s.pending.slice(1).forEach((c) => {
    const b = document.createElement('div');
    b.className = `ball color-${c}`;
    pendingBox.appendChild(b);
  });

  buildRecolorPicker(s.numColors);
}

function buildRecolorPicker(numColors) {
  const opts = el('recolor-options');
  if (opts.dataset.colors == numColors) return;
  opts.dataset.colors = numColors;
  opts.innerHTML = '';
  for (let c = 1; c <= numColors; ++c) {
    const b = document.createElement('div');
    b.className = `ball color-${c}`;
    b.onclick = () => confirmRecolor(c);
    opts.appendChild(b);
  }
}

function openRecolorPicker(idx) {
  state.pendingRecolorPos = idx;
  el('recolor-picker').classList.remove('hidden');
}

async function confirmRecolor(color) {
  el('recolor-picker').classList.add('hidden');
  const idx = state.pendingRecolorPos;
  if (idx < 0) return;
  const r = await api('/api/recolor', { position: idx, color });
  handleActionResult(r);
  setMode('shoot');
}

async function shoot(pos) {
  const r = await api('/api/shoot', { position: pos });
  handleActionResult(r);
}

async function doBomb(idx) {
  const r = await api('/api/bomb', { position: idx });
  handleActionResult(r);
  setMode('shoot');
}

async function undo() {
  const r = await api('/api/undo', {});
  handleActionResult(r);
}

async function skip() {
  const r = await api('/api/skip', {});
  handleActionResult(r);
}

function handleActionResult(r) {
  if (r.message) {
    const cls = r.eliminated > 0 ? 'hit' : (r.success ? '' : 'err');
    logMessage(r.message, cls);
  }
  if (r.state) renderState(r.state);
}

async function restart() {
  const payload = {
    seed: parseInt(el('cfg-seed').value || '0', 10),
    numColors: parseInt(el('cfg-colors').value || '5', 10),
    queueLength: parseInt(el('cfg-queue').value || '20', 10),
    initialTrackLength: parseInt(el('cfg-init').value || '10', 10),
  };
  const s = await api('/api/init', payload);
  el('message-log').innerHTML = '';
  logMessage('新游戏开始');
  renderState(s);
  setMode('shoot');
}

function wireUp() {
  el('btn-undo').onclick = undo;
  el('btn-skip').onclick = skip;
  el('btn-bomb').onclick = () => setMode(state.mode === 'bomb' ? 'shoot' : 'bomb');
  el('btn-recolor').onclick = () => setMode(state.mode === 'recolor' ? 'shoot' : 'recolor');
  el('btn-restart').onclick = restart;
  el('recolor-cancel').onclick = () => {
    el('recolor-picker').classList.add('hidden');
    setMode('shoot');
  };
}

async function init() {
  wireUp();
  const s = await api('/api/state');
  renderState(s);
  setMode('shoot');
}

window.addEventListener('DOMContentLoaded', init);
