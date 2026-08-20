const site = window.UE_LEARNING_SITE;

const state = {
  view: "home",
  moduleId: site.modules[0].id,
  query: "",
};

const els = {
  nav: document.querySelector(".main-nav"),
  moduleNav: document.querySelector("#moduleNav"),
  content: document.querySelector("#content"),
  title: document.querySelector("#viewTitle"),
  eyebrow: document.querySelector("#viewEyebrow"),
  search: document.querySelector("#searchInput"),
  results: document.querySelector("#searchResults"),
  focus: document.querySelector("#focusButton"),
  directoryToggle: document.querySelector("#directoryToggle"),
  pageToc: document.querySelector("#pageToc"),
};

function escapeHtml(value) {
  return String(value)
    .replaceAll("&", "&amp;")
    .replaceAll("<", "&lt;")
    .replaceAll(">", "&gt;")
    .replaceAll('"', "&quot;");
}

function list(items, ordered = false) {
  const tag = ordered ? "ol" : "ul";
  return `<${tag}>${items.map((item) => `<li>${escapeHtml(item)}</li>`).join("")}</${tag}>`;
}

function tags(values) {
  return `<div class="tag-row">${values.map((value) => `<span class="tag">${escapeHtml(value)}</span>`).join("")}</div>`;
}

function moduleById(id) {
  return site.modules.find((module) => module.id === id) || site.modules[0];
}

function setHash() {
  const next = `#/${state.view}/${state.moduleId}`;
  if (location.hash !== next) history.replaceState(null, "", next);
}

function parseHash() {
  const raw = decodeURIComponent(location.hash.replace(/^#\/?/, ""));
  const [view, moduleId] = raw.split("/");
  if (["home", "pipeline", "sources", "labs", "glossary", "module"].includes(view)) {
    state.view = view;
  }
  if (moduleId && site.modules.some((module) => module.id === moduleId)) {
    state.moduleId = moduleId;
  }
}

function switchView(view, moduleId = state.moduleId) {
  state.view = view;
  state.moduleId = moduleId;
  render();
  setHash();
  window.scrollTo({ top: 0, behavior: "smooth" });
}

function renderChrome() {
  els.nav.querySelectorAll(".nav-button").forEach((button) => {
    button.classList.toggle("active", button.dataset.view === state.view);
  });

  els.moduleNav.innerHTML = site.modules.map((module, index) => `
    <button class="module-link ${module.id === state.moduleId ? "active" : ""}" type="button" data-module="${escapeHtml(module.id)}">
      <span class="step">${String(index + 1).padStart(2, "0")}</span>
      <span><strong>${escapeHtml(module.title)}</strong><span>${escapeHtml(module.short)}</span></span>
    </button>
  `).join("");

  els.moduleNav.querySelectorAll("[data-module]").forEach((button) => {
    button.addEventListener("click", () => switchView("module", button.dataset.module));
  });
}

function renderHome() {
  els.eyebrow.textContent = "Learning Path";
  els.title.textContent = "学习路线";
  const metrics = [
    ["8", "学习模块"],
    ["6", "源码区域"],
    ["4", "验证实验"],
  ];

  els.content.innerHTML = `
    <section class="doc-intro">
      <p>探索 Unreal Engine 渲染源码的主要系统：Shader 编译、RDG、BasePass、GBuffer、光照后处理，以及 RHI 到 D3D12 的平台实现。</p>
      <div class="hero-banner">
        <img src="./assets/render-map.svg" alt="UE 渲染源码学习流程图">
      </div>
      <div class="on-this-page">
        <span>在这个页面上</span>
        <span>⌄</span>
      </div>
      <p><strong>Unreal Engine</strong> 的渲染源码很容易因为模块多、线程多、缓存多而读散。这个站点按官网文档的方式组织阅读入口：先建立源码地图，再用实验确认每个结论。</p>
      <div class="hero-actions">
        <button class="chip" type="button" data-view="pipeline">进入渲染管线</button>
        <button class="chip" type="button" data-view="sources">查看源码地图</button>
        <button class="chip" type="button" data-view="labs">开始验证实验</button>
      </div>
      <div class="scoreboard">
        ${metrics.map(([value, label]) => `<div class="metric"><strong>${value}</strong><span>${label}</span></div>`).join("")}
      </div>
    </section>

    <section class="section">
      <div class="section-head">
        <div>
          <h3>推荐路线</h3>
          <p>从目标反推学习顺序，不需要线性读完整套资料。</p>
        </div>
      </div>
      <div class="route-grid">
        ${site.routes.map((route) => `
          <article class="route-card">
            <h4>${escapeHtml(route.title)}</h4>
            <p>${escapeHtml(route.audience)}</p>
            ${list(route.steps, true)}
            <p><strong>完成标准：</strong>${escapeHtml(route.outcome)}</p>
          </article>
        `).join("")}
      </div>
    </section>

    <section class="section">
      <div class="section-head">
        <div>
          <h3>学习模块</h3>
          <p>每个模块都包含关键问题、源码入口、调用链、验证实验和常见误区。</p>
        </div>
      </div>
      <div class="module-grid">
        ${site.modules.map((module) => moduleCard(module)).join("")}
      </div>
    </section>
  `;

  els.content.querySelectorAll("[data-view]").forEach((button) => {
    button.addEventListener("click", () => switchView(button.dataset.view));
  });
  els.content.querySelectorAll("[data-module-card]").forEach((card) => {
    card.addEventListener("click", () => switchView("module", card.dataset.moduleCard));
  });
}

function moduleCard(module) {
  return `
    <article class="module-card" data-module-card="${escapeHtml(module.id)}">
      <h4>${escapeHtml(module.title)}</h4>
      <p>${escapeHtml(module.summary)}</p>
      ${tags([module.level, module.time, ...module.tags.slice(0, 2)])}
    </article>
  `;
}

function renderPipeline() {
  els.eyebrow.textContent = "Rendering Pipeline";
  els.title.textContent = "渲染管线";
  els.content.innerHTML = `
    <section class="section">
      <div class="section-head">
        <div>
          <h3>从资产到屏幕的主路径</h3>
          <p>点击任意节点进入对应学习模块。这个图是学习顺序，不是 UE 内部完整调用图。</p>
        </div>
      </div>
      <div class="pipeline">
        ${site.modules.filter((module) => module.id !== "debugging").map((module) => `
          <button class="pipe-node ${module.id === state.moduleId ? "active" : ""}" type="button" data-module="${escapeHtml(module.id)}">
            <strong>${escapeHtml(module.title)}</strong>
            <span>${escapeHtml(module.short)}</span>
          </button>
        `).join("")}
      </div>
    </section>
    ${renderModuleDetail(moduleById(state.moduleId), false)}
  `;
  els.content.querySelectorAll("[data-module]").forEach((button) => {
    button.addEventListener("click", () => switchView("pipeline", button.dataset.module));
  });
}

function renderModule() {
  const module = moduleById(state.moduleId);
  els.eyebrow.textContent = "Module";
  els.title.textContent = module.title;
  els.content.innerHTML = renderModuleDetail(module, true);
}

function renderModuleDetail(module, showHeader) {
  return `
    <section class="module-detail">
      ${showHeader ? `<h3>${escapeHtml(module.title)}</h3><p>${escapeHtml(module.summary)}</p>${tags([module.level, module.time, ...module.tags])}` : ""}
      <div class="detail-layout">
        <div class="detail-block">
          <h4>必须回答的问题</h4>
          ${list(module.questions)}
        </div>
        <div class="detail-block">
          <h4>推荐调用链</h4>
          ${list(module.callPath, true)}
        </div>
        <div class="detail-block">
          <h4>源码入口</h4>
          ${module.files.map((file) => `<code class="code-path">${escapeHtml(file)}</code>`).join("")}
        </div>
        <div class="detail-block">
          <h4>验证实验</h4>
          ${list(module.labs)}
        </div>
        <div class="detail-block">
          <h4>常见误区</h4>
          ${list(module.pitfalls)}
        </div>
      </div>
    </section>
  `;
}

function renderSources() {
  els.eyebrow.textContent = "Source Map";
  els.title.textContent = "源码地图";
  els.content.innerHTML = `
    <section class="section">
      <div class="section-head">
        <div>
          <h3>按职责找源码</h3>
          <p>先判断问题属于 Renderer、RenderCore、RHI 还是平台 RHI，再进入具体文件。</p>
        </div>
      </div>
      <div class="source-grid">
        ${site.sourceAreas.map((area) => `
          <article class="source-panel">
            <h4>${escapeHtml(area.area)}</h4>
            <p>${escapeHtml(area.role)}</p>
            <ul>${area.paths.map((path) => `<li><code>${escapeHtml(path)}</code></li>`).join("")}</ul>
          </article>
        `).join("")}
      </div>
    </section>
  `;
}

function renderLabs() {
  els.eyebrow.textContent = "Practice";
  els.title.textContent = "实战实验";
  els.content.innerHTML = `
    <section class="section">
      <div class="section-head">
        <div>
          <h3>用实验验证源码理解</h3>
          <p>每个实验都要求产生可观察证据，避免只停留在概念解释。</p>
        </div>
      </div>
      <div class="lab-grid">
        ${site.labs.map((lab) => `
          <article class="lab-card">
            <h4>${escapeHtml(lab.title)}</h4>
            <p>对应模块：${escapeHtml(lab.target)}</p>
            ${list(lab.checks)}
          </article>
        `).join("")}
      </div>
    </section>
  `;
}

function renderGlossary() {
  els.eyebrow.textContent = "Glossary";
  els.title.textContent = "术语速查";
  els.content.innerHTML = `
    <section class="section">
      <div class="section-head">
        <div>
          <h3>高频术语</h3>
          <p>只保留阅读 UE 渲染源码时最容易卡住的术语。</p>
        </div>
      </div>
      <div class="term-grid">
        ${site.terms.map(([term, desc]) => `
          <article class="term-card">
            <h4>${escapeHtml(term)}</h4>
            <p>${escapeHtml(desc)}</p>
          </article>
        `).join("")}
      </div>
    </section>
  `;
}

function searchIndex() {
  const rows = [];
  for (const module of site.modules) {
    rows.push({
      title: module.title,
      detail: module.summary,
      kind: "模块",
      action: () => switchView("module", module.id),
      text: [module.title, module.short, module.summary, module.tags.join(" "), module.files.join(" ")].join(" "),
    });
  }
  for (const area of site.sourceAreas) {
    rows.push({
      title: area.area,
      detail: area.role,
      kind: "源码区域",
      action: () => switchView("sources"),
      text: [area.area, area.role, area.paths.join(" ")].join(" "),
    });
  }
  for (const lab of site.labs) {
    rows.push({
      title: lab.title,
      detail: lab.checks.join("；"),
      kind: "实验",
      action: () => switchView("labs"),
      text: [lab.title, lab.target, lab.checks.join(" ")].join(" "),
    });
  }
  for (const [term, desc] of site.terms) {
    rows.push({
      title: term,
      detail: desc,
      kind: "术语",
      action: () => switchView("glossary"),
      text: `${term} ${desc}`,
    });
  }
  return rows;
}

function renderSearch() {
  const query = state.query.trim().toLowerCase();
  if (!query) {
    els.results.hidden = true;
    els.results.innerHTML = "";
    return;
  }

  const hits = searchIndex().filter((item) => item.text.toLowerCase().includes(query)).slice(0, 12);
  els.results.hidden = false;
  els.results.innerHTML = `
    <div class="search-results-inner">
      <h3>搜索结果：${escapeHtml(state.query)}</h3>
      <div class="search-list">
        ${hits.map((hit, index) => `
          <button class="search-card" type="button" data-hit="${index}">
            <strong>${escapeHtml(hit.title)} <span class="tag">${escapeHtml(hit.kind)}</span></strong>
            <span>${escapeHtml(hit.detail)}</span>
          </button>
        `).join("") || "<p>没有匹配结果。</p>"}
      </div>
    </div>
  `;
  els.results.querySelectorAll("[data-hit]").forEach((button) => {
    button.addEventListener("click", () => hits[Number(button.dataset.hit)].action());
  });
}

function renderPageToc() {
  const module = moduleById(state.moduleId);
  const itemsByView = {
    home: ["推荐路线", "学习模块"],
    pipeline: ["从资产到屏幕的主路径", module.title],
    sources: ["按职责找源码"],
    labs: ["用实验验证源码理解"],
    glossary: ["高频术语"],
    module: ["必须回答的问题", "源码入口", "验证实验", "常见误区"],
  };
  const items = itemsByView[state.view] || itemsByView.home;
  els.pageToc.innerHTML = items.map((item) => `<button type="button">${escapeHtml(item)}</button>`).join("");
}

function render() {
  renderChrome();
  renderSearch();
  if (state.view === "pipeline") renderPipeline();
  else if (state.view === "sources") renderSources();
  else if (state.view === "labs") renderLabs();
  else if (state.view === "glossary") renderGlossary();
  else if (state.view === "module") renderModule();
  else renderHome();
  renderPageToc();
}

els.nav.querySelectorAll("[data-view]").forEach((button) => {
  button.addEventListener("click", () => switchView(button.dataset.view));
});

els.search.addEventListener("input", (event) => {
  state.query = event.target.value;
  renderSearch();
});

els.focus.addEventListener("click", () => switchView("module", state.moduleId));

els.directoryToggle.addEventListener("click", () => {
  document.body.classList.toggle("nav-open");
});

document.querySelectorAll("[data-related]").forEach((card) => {
  card.addEventListener("click", () => switchView(card.dataset.related));
});

window.addEventListener("hashchange", () => {
  parseHash();
  render();
});

parseHash();
render();
