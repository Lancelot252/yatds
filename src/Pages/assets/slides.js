
(function () {
  function escapeHtml(text) {
    return text
      .replace(/&/g, '&amp;')
      .replace(/</g, '&lt;')
      .replace(/>/g, '&gt;');
  }

  function autoLinkPlainUrls() {
    const urlPattern = /(https?:\/\/[^\s<>"。)，）]+)/g;
    const walker = document.createTreeWalker(
      document.body,
      NodeFilter.SHOW_TEXT,
      {
        acceptNode(node) {
          const parent = node.parentElement;
          if (!parent || parent.closest('a, code, pre, script, style')) {
            return NodeFilter.FILTER_REJECT;
          }
          urlPattern.lastIndex = 0;
          return urlPattern.test(node.nodeValue) ? NodeFilter.FILTER_ACCEPT : NodeFilter.FILTER_REJECT;
        }
      }
    );
    const nodes = [];
    while (walker.nextNode()) nodes.push(walker.currentNode);
    nodes.forEach((node) => {
      urlPattern.lastIndex = 0;
      const fragment = document.createDocumentFragment();
      let lastIndex = 0;
      node.nodeValue.replace(urlPattern, (url, _capturedUrl, offset) => {
        fragment.append(document.createTextNode(node.nodeValue.slice(lastIndex, offset)));
        const link = document.createElement('a');
        link.href = url;
        link.textContent = url;
        link.target = '_blank';
        link.rel = 'noopener noreferrer';
        fragment.append(link);
        lastIndex = offset + url.length;
      });
      fragment.append(document.createTextNode(node.nodeValue.slice(lastIndex)));
      node.parentNode.replaceChild(fragment, node);
    });
  }

  function highlightCodeBlocks() {
    const keywords = new Set([
      'alignas', 'alignof', 'auto', 'break', 'case', 'catch', 'class', 'const', 'constexpr',
      'continue', 'delete', 'do', 'else', 'enum', 'explicit', 'extern', 'false', 'for', 'if',
      'inline', 'namespace', 'new', 'nullptr', 'operator', 'private', 'protected', 'public',
      'return', 'sizeof', 'static', 'struct', 'switch', 'template', 'this', 'throw', 'true',
      'try', 'typedef', 'typename', 'using', 'virtual', 'while'
    ]);
    const types = new Set([
      'bool', 'char', 'double', 'float', 'int', 'long', 'short', 'signed', 'string',
      'unsigned', 'vector', 'void'
    ]);
    const tokenPattern = /(\/\*[\s\S]*?\*\/|\/\/[^\n]*|"(?:\\.|[^"\\])*"|'(?:\\.|[^'\\])*'|#[^\n]*|\b\d+(?:\.\d+)?\b|\b[A-Za-z_]\w*\b|[{}()[\];,.*&<>!=+\-/:%|]+)/g;
    document.querySelectorAll('pre code').forEach((block) => {
      const source = block.textContent;
      let lastIndex = 0;
      let highlighted = '';
      source.replace(tokenPattern, (token, _capture, offset) => {
        highlighted += escapeHtml(source.slice(lastIndex, offset));
        const safeToken = escapeHtml(token);
        const nextText = source.slice(offset + token.length);
        if (/^\/\//.test(token) || /^\/\*/.test(token)) {
          highlighted += `<span class="tok-comment">${safeToken}</span>`;
        } else if (/^["']/.test(token)) {
          highlighted += `<span class="tok-string">${safeToken}</span>`;
        } else if (/^#/.test(token)) {
          highlighted += `<span class="tok-preproc">${safeToken}</span>`;
        } else if (/^\d/.test(token)) {
          highlighted += `<span class="tok-number">${safeToken}</span>`;
        } else if (keywords.has(token)) {
          highlighted += `<span class="tok-keyword">${safeToken}</span>`;
        } else if (types.has(token)) {
          highlighted += `<span class="tok-type">${safeToken}</span>`;
        } else if (/^[A-Za-z_]\w*$/.test(token) && /^\s*\(/.test(nextText)) {
          highlighted += `<span class="tok-function">${safeToken}</span>`;
        } else if (/^[{}()[\];,.*&<>!=+\-/:%|]+$/.test(token)) {
          highlighted += `<span class="tok-operator">${safeToken}</span>`;
        } else {
          highlighted += safeToken;
        }
        lastIndex = offset + token.length;
      });
      highlighted += escapeHtml(source.slice(lastIndex));
      block.innerHTML = highlighted;
    });
  }

  function initDeck(deck) {
    const slides = Array.from(deck.querySelectorAll('[data-slide-src]')).map((el) => ({
      src: el.getAttribute('data-slide-src'),
      title: el.getAttribute('data-slide-title') || ''
    }));
    if (!slides.length) return;
    let index = 0;
    const img = deck.querySelector('.slide-stage img');
    const title = deck.querySelector('.slide-stage .slide-title');
    const count = deck.querySelector('.slide-stage .slide-count');
    const label = deck.querySelector('#slideLabel') || deck.querySelector('.slide-meta > span:first-child');
    const dots = deck.querySelector('.slide-dots');
    const prev = deck.querySelector('[data-slide-prev]');
    const next = deck.querySelector('[data-slide-next]');

    slides.forEach((_, i) => {
      const dot = document.createElement('button');
      dot.type = 'button';
      dot.setAttribute('aria-label', `查看第 ${i + 1} 张`);
      dot.addEventListener('click', () => show(i));
      dots.appendChild(dot);
    });

    function show(nextIndex) {
      index = (nextIndex + slides.length) % slides.length;
      img.src = slides[index].src;
      img.alt = slides[index].title;
      if (title) title.textContent = slides[index].title;
      if (count) count.textContent = `${index + 1} / ${slides.length}`;
      if (label) label.textContent = `${slides[index].title} / ${slides.length}`;
      Array.from(dots.children).forEach((dot, i) => dot.classList.toggle('active', i === index));
    }

    prev.addEventListener('click', () => show(index - 1));
    next.addEventListener('click', () => show(index + 1));
    show(0);
  }

  function decodeHash(rawHash) {
    try {
      return decodeURIComponent(rawHash);
    } catch (_error) {
      return rawHash;
    }
  }

  function findTocLink(toc, rawHash) {
    if (!rawHash) return null;
    const decodedHash = decodeHash(rawHash);
    return Array.from(toc.querySelectorAll('a[href^="#"]')).find((link) => {
      const href = link.getAttribute('href');
      return href === rawHash || href === decodedHash;
    }) || null;
  }

  function setTocCurrent(toc, current) {
    if (!current) return;
    toc.querySelectorAll('[aria-current="page"]').forEach((item) => item.removeAttribute('aria-current'));
    toc.querySelectorAll('.toc-chapter-open').forEach((item) => item.classList.remove('toc-chapter-open'));

    current.setAttribute('aria-current', 'page');
    if (current.classList.contains('toc-subitem')) {
      let previous = current.parentElement?.previousElementSibling;
      while (previous && !previous.classList.contains('toc-chapter')) {
        previous = previous.previousElementSibling;
      }
      previous?.classList.add('toc-chapter-open');
    } else if (current.classList.contains('toc-chapter')) {
      current.classList.add('toc-chapter-open');
    }
  }

  function currentContentHash(toc) {
    const links = Array.from(toc.querySelectorAll('a[href^="#"]'));
    const targets = links
      .map((link) => {
        const href = link.getAttribute('href');
        let id = href.slice(1);
        try {
          id = decodeURIComponent(id);
        } catch (_error) {
          id = href.slice(1);
        }
        const target = document.getElementById(id);
        return target ? { href, target } : null;
      })
      .filter(Boolean);
    if (!targets.length) return '';

    const offset = 150;
    let current = targets[0];
    for (const item of targets) {
      if (item.target.getBoundingClientRect().top <= offset) {
        current = item;
      } else {
        break;
      }
    }
    return current.href;
  }

  function initTocTracking() {
    const toc = document.querySelector('.toc-chapters');
    if (!toc) return;

    const syncFromHash = () => {
      const current = findTocLink(toc, window.location.hash) || toc.querySelector('.toc-subitem, .toc-chapter');
      setTocCurrent(toc, current);
    };

    const syncFromScroll = () => {
      const hash = currentContentHash(toc);
      const current = findTocLink(toc, hash);
      setTocCurrent(toc, current);
    };

    let ticking = false;
    window.addEventListener('scroll', () => {
      if (ticking) return;
      ticking = true;
      window.requestAnimationFrame(() => {
        syncFromScroll();
        ticking = false;
      });
    }, { passive: true });

    window.addEventListener('hashchange', syncFromHash);
    window.setTimeout(syncFromHash, 0);
    window.setTimeout(syncFromScroll, 120);
  }

  function syncTocCurrent() {
    const toc = document.querySelector('.toc-chapters');
    if (!toc) return;

    let current = findTocLink(toc, window.location.hash);
    if (!current) current = toc.querySelector('.toc-subitem, .toc-chapter');
    if (!current) return;
    setTocCurrent(toc, current);
  }

  autoLinkPlainUrls();
  highlightCodeBlocks();
  initTocTracking();
  document.querySelectorAll('.slide-deck').forEach(initDeck);
})();
