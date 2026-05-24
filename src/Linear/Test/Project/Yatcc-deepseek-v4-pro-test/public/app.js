/**
 * 祖玛游戏 - 前端交互逻辑
 *
 * 通过 fetch API 与 C++ 后端通信
 */

let selectedPosition = -1; // 当前选中的插入位置

const colorNames = {
    'R': '红', 'G': '绿', 'B': '蓝', 'Y': '黄', 'P': '紫', 'O': '橙'
};

// ==================== API 通信 ====================

async function apiCall(endpoint, body = null) {
    try {
        const options = {
            method: body ? 'POST' : 'GET',
            headers: { 'Content-Type': 'application/json' },
        };
        if (body) options.body = JSON.stringify(body);

        const resp = await fetch(endpoint, options);
        if (!resp.ok) throw new Error(`HTTP ${resp.status}`);
        return await resp.json();
    } catch (err) {
        console.error('API 错误:', err);
        return null;
    }
}

async function getState() {
    return await apiCall('/api/state');
}

// ==================== 游戏初始化 ====================

async function initGame() {
    const trackSize = parseInt(document.getElementById('init-track').value) || 10;
    const queueSize = parseInt(document.getElementById('init-queue').value) || 20;
    const maxTrack  = parseInt(document.getElementById('init-max').value) || 20;

    const state = await apiCall('/api/init', {
        trackSize, queueSize, maxTrack
    });

    if (state) {
        document.getElementById('init-panel').classList.add('hidden');
        document.getElementById('game-panel').classList.remove('hidden');
        updateUI(state);
    }
}

async function resetGame() {
    document.getElementById('game-over-modal').classList.add('hidden');
    document.getElementById('game-panel').classList.add('hidden');
    document.getElementById('init-panel').classList.remove('hidden');
    selectedPosition = -1;
}

// ==================== 游戏操作 ====================

async function fireBall() {
    const state = await getState();
    if (!state || state.track === '' && state.queue === '') {
        // Already won
        return;
    }

    if (selectedPosition < 0) {
        alert('请先点击轨道上方的 ○ 标记来选择插入位置！');
        return;
    }

    const result = await apiCall('/api/fire', { pos: selectedPosition });
    selectedPosition = -1;
    if (result) updateUI(result);
}

async function undoAction() {
    const result = await apiCall('/api/undo');
    if (result) updateUI(result);
}

// ==================== 小道具 ====================

async function useBomb() {
    if (selectedPosition < 0) {
        alert('请先点击轨道上方的 ○ 标记来选择目标位置！');
        return;
    }
    const result = await apiCall('/api/bomb', { pos: selectedPosition });
    selectedPosition = -1;
    if (result) updateUI(result);
}

async function useShuffle() {
    const result = await apiCall('/api/shuffle');
    if (result) updateUI(result);
}

async function usePushBack() {
    const result = await apiCall('/api/pushback');
    if (result) updateUI(result);
}

async function useRainbow() {
    if (selectedPosition < 0) {
        alert('请先点击轨道上方的 ○ 标记来选择目标位置！');
        return;
    }
    const result = await apiCall('/api/rainbow', { pos: selectedPosition });
    selectedPosition = -1;
    if (result) updateUI(result);
}

// ==================== UI 更新 ====================

function updateUI(state) {
    if (!state) return;

    const track = state.track || '';
    const queue = state.queue || '';
    const score = state.score || 0;
    const combo = state.combo || 0;
    const eliminated = state.eliminated || 0;
    const maxTrack = state.maxTrack || 20;

    // 更新队列显示
    const queueDisplay = document.getElementById('queue-display');
    queueDisplay.innerHTML = '';
    for (let i = 0; i < queue.length; i++) {
        const ball = createBallElement(queue[i], i === 0);
        queueDisplay.appendChild(ball);
    }
    document.getElementById('queue-count').textContent = queue.length;

    // 更新轨道显示
    const trackDisplay = document.getElementById('track-display');
    trackDisplay.innerHTML = '';
    for (let i = 0; i < track.length; i++) {
        const ball = createBallElement(track[i], false);
        ball.title = `位置 ${i}: ${colorNames[track[i]]}色球`;
        trackDisplay.appendChild(ball);
    }
    document.getElementById('track-count').textContent = track.length;
    document.getElementById('track-max').textContent = maxTrack;

    // 更新插入位置标记
    const markers = document.getElementById('insert-markers');
    markers.innerHTML = '';
    // 有 trackSize+1 个插入位置（0 .. trackSize）
    for (let i = 0; i <= track.length; i++) {
        const marker = document.createElement('div');
        marker.className = 'insert-marker' + (i === selectedPosition ? ' selected' : '');
        marker.textContent = '○';
        marker.title = `插入到位置 ${i}`;
        marker.onclick = (() => {
            const pos = i;
            return () => {
                selectedPosition = pos;
                updateInsertMarkers();
            };
        })();
        markers.appendChild(marker);
    }

    // 更新状态数字
    document.getElementById('score').textContent = score;
    document.getElementById('combo').textContent = combo;
    document.getElementById('eliminated').textContent = eliminated;

    // 更新按钮状态
    const gameOver = checkGameOver(track, queue, maxTrack);
    const btns = document.querySelectorAll('#game-panel .btn');
    btns.forEach(b => {
        if (gameOver) {
            b.disabled = true;
        } else {
            b.disabled = false;
        }
    });

    // 更新游戏状态文字
    const statusEl = document.getElementById('game-status');
    if (track.length === 0 && queue.length === 0) {
        statusEl.textContent = '🎉 恭喜通关！';
        statusEl.style.color = '#6bcb77';
    } else if (track.length >= maxTrack) {
        statusEl.textContent = '💀 轨道已满！';
        statusEl.style.color = '#ff6b6b';
    } else {
        statusEl.textContent = '进行中...';
        statusEl.style.color = '#ffd93d';
    }
}

function updateInsertMarkers() {
    const markers = document.querySelectorAll('.insert-marker');
    markers.forEach((m, i) => {
        if (i === selectedPosition) {
            m.classList.add('selected');
        } else {
            m.classList.remove('selected');
        }
    });
}

function createBallElement(color, isNext) {
    const ball = document.createElement('div');
    ball.className = 'ball ball-' + color + (isNext ? ' next-ball' : '');
    ball.textContent = color;
    return ball;
}

function checkGameOver(track, queue, maxTrack) {
    // 胜利条件：轨道和队列都为空
    if (track.length === 0 && queue.length === 0) {
        showGameOver('🎉 恭喜通关！', `最终得分: ${document.getElementById('score').textContent}`, true);
        return true;
    }
    // 失败条件：轨道已满
    if (track.length >= maxTrack) {
        showGameOver('💀 游戏结束', '轨道已满，挑战失败！', false);
        return true;
    }
    return false;
}

function showGameOver(title, msg, isWin) {
    document.getElementById('modal-title').textContent = title;
    document.getElementById('modal-msg').textContent = msg;
    document.getElementById('game-over-modal').classList.remove('hidden');

    if (isWin) {
        document.getElementById('modal-title').style.color = '#ffd93d';
    } else {
        document.getElementById('modal-title').style.color = '#ff6b6b';
    }
}