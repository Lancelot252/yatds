// 祖玛游戏前端逻辑

const API_BASE = '/api';

// 颜色映射
const COLOR_MAP = {
    'R': { name: '红色', emoji: '🔴' },
    'G': { name: '绿色', emoji: '🟢' },
    'B': { name: '蓝色', emoji: '🔵' },
    'Y': { name: '黄色', emoji: '🟡' },
    'P': { name: '紫色', emoji: '🟣' },
    'O': { name: '橙色', emoji: '🟠' }
};

// 道具映射
const ITEM_MAP = {
    0: { name: '随机重排', icon: '🔀', cssClass: 'shuffle' },
    1: { name: '炸弹', icon: '💣', cssClass: 'bomb' },
    2: { name: '颜色炸弹', icon: '🎨', cssClass: 'colorBomb' },
    3: { name: '反转', icon: '🔄', cssClass: 'reverse' }
};

// 当前游戏状态
let gameState = null;
let selectedItemType = null;

// 初始化游戏
async function initGame() {
    try {
        const response = await fetch(`${API_BASE}/init`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({
                trackSize: 8,
                queueSize: 10,
                maxTrack: 15
            })
        });
        const data = await response.json();
        gameState = data;
        updateUI();
        hideGameStatus();
        hideItemDetail();
    } catch (error) {
        console.error('初始化游戏失败:', error);
    }
}

// 发射彩球
async function fireBall() {
    if (!gameState || gameState.gameOver || gameState.gameWon) return;

    const posInput = document.getElementById('fire-position');
    const pos = parseInt(posInput.value);

    if (isNaN(pos) || pos < 0 || pos > gameState.track.length) {
        alert(`请输入有效的位置 (0-${gameState.track.length})`);
        return;
    }

    try {
        const response = await fetch(`${API_BASE}/fire`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ pos: pos })
        });
        const data = await response.json();
        gameState = data.state;

        // 显示消除信息
        if (data.eliminated) {
            showEliminationInfo(data.eliminatedCount, data.chainCount);
        }

        updateUI();
        checkGameEnd();
    } catch (error) {
        console.error('发射彩球失败:', error);
    }
}

// 撤销操作
async function undoAction() {
    if (!gameState) return;

    try {
        const response = await fetch(`${API_BASE}/undo`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' }
        });
        const data = await response.json();
        gameState = data.state;
        updateUI();
        hideGameStatus();
    } catch (error) {
        console.error('撤销操作失败:', error);
    }
}

// 使用道具
async function useItem(type, pos, color) {
    if (!gameState || gameState.gameOver || gameState.gameWon) return;

    try {
        const response = await fetch(`${API_BASE}/item`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({
                type: type,
                pos: pos !== undefined ? pos : -1,
                color: color || 'R'
            })
        });
        const data = await response.json();
        gameState = data.state;
        updateUI();
        checkGameEnd();
        hideItemDetail();
    } catch (error) {
        console.error('使用道具失败:', error);
    }
}

// 点击轨道彩球位置来发射
async function fireAtPosition(pos) {
    if (!gameState || gameState.gameOver || gameState.gameWon) return;

    document.getElementById('fire-position').value = pos;

    try {
        const response = await fetch(`${API_BASE}/fire`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ pos: pos })
        });
        const data = await response.json();
        gameState = data.state;

        if (data.eliminated) {
            showEliminationInfo(data.eliminatedCount, data.chainCount);
        }

        updateUI();
        checkGameEnd();
    } catch (error) {
        console.error('发射彩球失败:', error);
    }
}

// 显示道具详情
function showItemDetail(type) {
    selectedItemType = type;
    const detailDiv = document.getElementById('item-detail');
    const contentDiv = document.getElementById('item-detail-content');
    detailDiv.classList.remove('hidden');

    let html = '';

    if (type === 0) { // 随机重排 - 无需额外参数
        html = `
            <p>随机重排轨道上的所有彩球</p>
            <button onclick="useItem(0)">确认使用</button>
        `;
    } else if (type === 1) { // 炸弹 - 需要位置
        html = `
            <label>选择炸弹作用位置:</label>
            <input type="number" id="bomb-pos" min="0" max="${gameState.track.length - 1}" value="0">
            <button onclick="useItem(1, parseInt(document.getElementById('bomb-pos').value))">确认使用</button>
        `;
    } else if (type === 2) { // 颜色炸弹 - 需要颜色
        html = `
            <label>选择要消除的颜色:</label>
            <select id="color-bomb-color">
                ${Object.entries(COLOR_MAP).map(([k, v]) =>
                    `<option value="${k}">${v.emoji} ${v.name}</option>`
                ).join('')}
            </select>
            <button onclick="useItem(2, -1, document.getElementById('color-bomb-color').value)">确认使用</button>
        `;
    } else if (type === 3) { // 反转 - 无需额外参数
        html = `
            <p>反转轨道彩球顺序</p>
            <button onclick="useItem(3)">确认使用</button>
        `;
    }

    contentDiv.innerHTML = html;
}

// 隐藏道具详情
function hideItemDetail() {
    document.getElementById('item-detail').classList.add('hidden');
    selectedItemType = null;
}

// 更新UI
function updateUI() {
    if (!gameState) return;

    // 更新分数和信息
    document.getElementById('score-display').textContent = `分数: ${gameState.score}`;
    document.getElementById('undo-display').textContent = `可撤销: ${gameState.undoCount}`;
    document.getElementById('track-info').textContent = `轨道: ${gameState.track.length}/${gameState.maxTrack}`;

    // 更新轨道
    updateTrack();

    // 更新待发射彩球
    updateFireSection();

    // 更新道具
    updateItems();

    // 更新发射位置输入范围
    const posInput = document.getElementById('fire-position');
    posInput.max = gameState.track.length;
    if (parseInt(posInput.value) > gameState.track.length) {
        posInput.value = gameState.track.length;
    }
}

// 更新轨道显示
function updateTrack() {
    const container = document.getElementById('track-container');
    const positionsDiv = document.getElementById('track-positions');

    if (gameState.track.length === 0) {
        container.innerHTML = '<div class="empty-track">轨道已清空! 🎉</div>';
        positionsDiv.innerHTML = '';
        return;
    }

    // 生成轨道彩球
    let trackHtml = '';
    gameState.track.forEach((color, index) => {
        trackHtml += `<div class="ball ${color}" title="位置 ${index}: ${COLOR_MAP[color]?.name || color}">${COLOR_MAP[color]?.emoji || color}</div>`;
    });
    container.innerHTML = trackHtml;

    // 生成位置标记（可点击）
    let posHtml = '';
    // 在每个彩球前面和最后面添加插入位置标记
    for (let i = 0; i <= gameState.track.length; i++) {
        posHtml += `<div class="position-marker" onclick="fireAtPosition(${i})" title="插入到位置 ${i}">↑${i}</div>`;
    }
    positionsDiv.innerHTML = posHtml;
}

// 更新待发射区域
function updateFireSection() {
    // 当前彩球
    const currentBall = document.getElementById('current-ball');
    if (gameState.queue.length > 0) {
        const color = gameState.currentBall;
        currentBall.className = `ball ${color}`;
        currentBall.innerHTML = COLOR_MAP[color]?.emoji || color;
    } else {
        currentBall.className = 'ball';
        currentBall.innerHTML = '—';
        currentBall.style.background = 'rgba(255,255,255,0.1)';
    }

    // 队列彩球
    const queueContainer = document.getElementById('queue-container');
    let queueHtml = '';
    gameState.queue.forEach((color, index) => {
        const opacity = index === 0 ? '1' : '0.7';
        const sizeClass = index === 0 ? '' : 'queue-ball';
        queueHtml += `<div class="ball ${sizeClass} ${color}" style="opacity:${opacity}" title="队列第 ${index + 1} 个">${COLOR_MAP[color]?.emoji || color}</div>`;
    });
    queueContainer.innerHTML = queueHtml;
}

// 更新道具显示
function updateItems() {
    const container = document.getElementById('item-container');
    let html = '';

    gameState.items.forEach(item => {
        const info = ITEM_MAP[item.type];
        const disabledClass = item.count <= 0 ? 'disabled' : '';
        html += `
            <button class="item-btn ${info.cssClass} ${disabledClass}"
                    onclick="${item.count > 0 ? `showItemDetail(${item.type})` : ''}"
                    ${item.count <= 0 ? 'disabled' : ''}>
                <span>${info.icon} ${info.name}</span>
                <span class="item-count">剩余: ${item.count}</span>
            </button>
        `;
    });

    container.innerHTML = html;
}

// 显示消除信息
function showEliminationInfo(count, chainCount) {
    const infoDiv = document.getElementById('elimination-info');
    const textSpan = document.getElementById('elimination-text');

    let text = `消除 ${count} 个彩球!`;
    if (chainCount > 1) {
        text += ` 连锁消除 ${chainCount} 次! 🔥`;
    }

    textSpan.textContent = text;
    infoDiv.classList.remove('hidden');

    // 2秒后自动隐藏
    setTimeout(() => {
        infoDiv.classList.add('hidden');
    }, 2000);
}

// 检查游戏结束
function checkGameEnd() {
    if (!gameState) return;

    if (gameState.gameWon) {
        showGameStatus('🎉 游戏胜利! 🎉', 'win');
    } else if (gameState.gameOver) {
        showGameStatus('💥 游戏失败! 💥', 'lose');
    }
}

// 显示游戏状态
function showGameStatus(message, type) {
    const statusDiv = document.getElementById('game-status');
    const messageDiv = document.getElementById('status-message');

    statusDiv.classList.remove('hidden', 'win', 'lose');
    statusDiv.classList.add(type);
    messageDiv.textContent = message;
}

// 隐藏游戏状态
function hideGameStatus() {
    document.getElementById('game-status').classList.add('hidden');
}

// 页面加载完成后初始化游戏
document.addEventListener('DOMContentLoaded', () => {
    initGame();
});