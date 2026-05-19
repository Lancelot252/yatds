// API 基础 URL
const API_BASE = 'http://localhost:8000';

// 颜色映射
const colorMap = {
    'R': 'red',
    'B': 'blue',
    'G': 'green',
    'Y': 'yellow',
    'P': 'purple'
};

// 显示消息
function showMessage(message, type = 'info') {
    const messageEl = document.getElementById('message');
    messageEl.textContent = message;
    messageEl.className = 'message ' + type;
    
    // 3秒后自动隐藏
    setTimeout(() => {
        messageEl.className = 'message';
    }, 3000);
}

// 初始化游戏
function initGame() {
    const track = document.getElementById('trackInput').value.trim();
    const queue = document.getElementById('queueInput').value.trim();
    
    if (!track) {
        showMessage('请输入轨道初始状态', 'error');
        return;
    }
    
    // 模拟本地游戏初始化
    window.gameTrack = track.split(/\s+/);
    window.gameQueue = queue.split(/\s+/).filter(x => x);
    window.eliminations = 0;
    window.history = [{
        track: [...window.gameTrack],
        queue: [...window.gameQueue],
        eliminations: 0
    }];
    
    updateDisplay();
    showMessage('游戏初始化成功！', 'success');
}

// 插入球
function insertBall() {
    if (!window.gameTrack) {
        showMessage('请先初始化游戏', 'error');
        return;
    }
    
    const position = parseInt(document.getElementById('posInput').value);
    const color = document.getElementById('colorSelect').value;
    
    if (isNaN(position) || position < 0 || position > window.gameTrack.length) {
        showMessage('位置必须在 0-' + window.gameTrack.length + ' 之间', 'error');
        return;
    }
    
    // 保存状态用于撤销
    saveGameState();
    
    // 插入球
    window.gameTrack.splice(position, 0, color);
    
    // 执行消除
    eliminate();
    
    updateDisplay();
    
    // 检查游戏状态
    if (window.gameTrack.length === 0 && window.gameQueue.length === 0) {
        showMessage('🎉 恭喜你赢了！所有的球都被消除了！', 'success');
    } else if (window.gameTrack.length > 50) {
        showMessage('😢 游戏失败！轨道上的球过多了！', 'error');
    } else {
        showMessage('✓ 球已插入并进行了消除处理', 'success');
    }
}

// 执行消除操作
function eliminate() {
    let hasElimination = true;
    
    while (hasElimination) {
        hasElimination = eliminateSequence();
    }
}

// 消除连续3个或以上相同颜色的球
function eliminateSequence() {
    const track = window.gameTrack;
    let toRemove = new Array(track.length).fill(false);
    let found = false;
    
    for (let i = 0; i < track.length; ) {
        let j = i;
        
        // 找出连续相同颜色的球
        while (j < track.length && track[j] === track[i]) {
            j++;
        }
        
        // 如果连续3个或以上，标记为删除
        if (j - i >= 3) {
            for (let k = i; k < j; k++) {
                toRemove[k] = true;
            }
            found = true;
            window.eliminations++;
        }
        
        i = j;
    }
    
    // 删除标记的球
    if (found) {
        window.gameTrack = track.filter((_, index) => !toRemove[index]);
    }
    
    return found;
}

// 保存游戏状态
function saveGameState() {
    if (!window.history) {
        window.history = [];
    }
    
    window.history.push({
        track: [...window.gameTrack],
        queue: [...window.gameQueue],
        eliminations: window.eliminations
    });
}

// 撤销操作
function undoAction() {
    if (!window.history || window.history.length <= 1) {
        showMessage('无法撤销（已经是初始状态）', 'warning');
        return;
    }
    
    window.history.pop(); // 移除当前状态
    const prevState = window.history[window.history.length - 1];
    
    window.gameTrack = [...prevState.track];
    window.gameQueue = [...prevState.queue];
    window.eliminations = prevState.eliminations;
    
    updateDisplay();
    showMessage('已撤销上一步操作', 'info');
}

// 重置游戏
function resetGame() {
    window.gameTrack = undefined;
    window.gameQueue = undefined;
    window.eliminations = 0;
    window.history = undefined;
    
    document.getElementById('trackInput').value = 'R B B B G';
    document.getElementById('queueInput').value = 'R G B';
    document.getElementById('posInput').value = '2';
    
    updateDisplay();
    showMessage('游戏已重置', 'info');
}

// 更新显示
function updateDisplay() {
    if (!window.gameTrack) {
        document.getElementById('trackDisplay').innerHTML = '<span class="empty-state">(未初始化)</span>';
        document.getElementById('queueDisplay').innerHTML = '<span class="empty-state">(未初始化)</span>';
        document.getElementById('trackSize').textContent = '0';
        document.getElementById('queueSize').textContent = '0';
        document.getElementById('eliminations').textContent = '0';
        document.getElementById('gameState').textContent = '未初始化';
        return;
    }
    
    // 更新轨道显示
    const trackDisplay = document.getElementById('trackDisplay');
    if (window.gameTrack.length === 0) {
        trackDisplay.innerHTML = '<span class="empty-state">(轨道为空)</span>';
    } else {
        trackDisplay.innerHTML = window.gameTrack
            .map(ball => `<div class="ball ${colorMap[ball]}">${ball}</div>`)
            .join('');
    }
    
    // 更新队列显示
    const queueDisplay = document.getElementById('queueDisplay');
    if (window.gameQueue.length === 0) {
        queueDisplay.innerHTML = '<span class="empty-state">(队列为空)</span>';
    } else {
        queueDisplay.innerHTML = window.gameQueue
            .map(ball => `<div class="ball ${colorMap[ball]}">${ball}</div>`)
            .join('');
    }
    
    // 更新统计信息
    document.getElementById('trackSize').textContent = window.gameTrack.length;
    document.getElementById('queueSize').textContent = window.gameQueue.length;
    document.getElementById('eliminations').textContent = window.eliminations;
    
    // 更新游戏状态
    let state = '进行中';
    if (window.gameTrack.length === 0 && window.gameQueue.length === 0) {
        state = '✓ 胜利！';
        document.getElementById('gameState').style.color = '#28a745';
    } else if (window.gameTrack.length > 50) {
        state = '✗ 失败';
        document.getElementById('gameState').style.color = '#dc3545';
    } else {
        document.getElementById('gameState').style.color = '#667eea';
    }
    document.getElementById('gameState').textContent = state;
    
    // 更新位置输入框的最大值
    document.getElementById('posInput').max = window.gameTrack.length;
}

// 刷新状态（用于 API 模式）
function refreshStatus() {
    if (!window.gameTrack) {
        showMessage('请先初始化游戏', 'error');
        return;
    }
    updateDisplay();
}

// 页面加载时初始化
document.addEventListener('DOMContentLoaded', function() {
    updateDisplay();
    
    // 添加回车键支持
    document.getElementById('trackInput').addEventListener('keypress', function(e) {
        if (e.key === 'Enter') initGame();
    });
    
    document.getElementById('posInput').addEventListener('keypress', function(e) {
        if (e.key === 'Enter') insertBall();
    });
});
