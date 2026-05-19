// 前端与后端交互的占位脚本
// 实际交互可通过fetch与后端通信，或通过文件/命令行方式模拟

document.getElementById('fireBtn').onclick = function() {
    const pos = document.getElementById('insertPos').value;
    // 这里应调用后端接口，传递插入位置
    document.getElementById('message').innerText = '发射请求已发送（需后端实现）';
};
document.getElementById('undoBtn').onclick = function() {
    // 这里应调用后端撤销接口
    document.getElementById('message').innerText = '撤销请求已发送（需后端实现）';
};
// 轨道和队列的渲染逻辑应由后端数据驱动，这里仅为占位
function renderTrack(track) {
    const trackDiv = document.getElementById('track');
    trackDiv.innerHTML = track.map(c => `<span class="ball ${c}">${c}</span>`).join('');
}
function renderQueue(queue) {
    const queueDiv = document.getElementById('queue');
    queueDiv.innerHTML = '待发射：' + queue.map(c => `<span class="ball ${c}">${c}</span>`).join('');
}
// 示例初始化
renderTrack(['R','R','B','B','B','R','R']);
renderQueue(['G','Y','B']);