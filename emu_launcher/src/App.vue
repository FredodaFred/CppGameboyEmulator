<script setup lang="ts">
import { open } from '@tauri-apps/plugin-dialog';
import { ref } from 'vue';
import { invoke } from "@tauri-apps/api/core";

const romPath = ref('');
const romSelected = ref(false);
const showConfig = ref(false);

async function launchGame() {
  try {
    await invoke('launch_emulator', { romPath: romPath.value });
    console.log('Emulator launched!');
  } catch (error) {
    console.error('Failed to launch:', error);
  }
}

async function selectFile() {
  const selected = await open({
    multiple: false,
    filters: [{ name: 'GameBoy ROM', extensions: ['gb', 'gbc'] }]
  });
  if (selected == null) return;
  romPath.value = selected;
  romSelected.value = true;
}
</script>

<template>
  <main>
    <nav class="top-menu">
      <span class="menu-title">Freddy's GB Emulator</span>
      <div class="menu-links">
        <button class="menu-link" @click="selectFile">Open ROM</button>
        <button class="menu-link" :class="{ active: showConfig }" @click="showConfig = !showConfig">Configuration</button>
      </div>
    </nav>

    <div v-if="showConfig" class="config-panel">
      <p>Configuration options go here.</p>
    </div>

    <div class="selection-card">
      <p class="rom-status"> Rom: {{ romSelected ? romPath : 'No ROM selected' }}</p>
    </div>

    <button :disabled="!romSelected" @click="launchGame" class="play-btn">
      PLAY
    </button>
  </main>
</template>

<style>
*, *::before, *::after { box-sizing: border-box; margin: 0; padding: 0; }

:root {
  font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', sans-serif;
  background: rgba(2,0,7,0.89);
}

main {
  display: flex;
  flex-direction: column;
  height: 100vh;
}

.top-menu {
  display: flex;
  align-items: center;
  justify-content: space-between;
  padding: 0 16px;
  height: 42px;
  background: rgba(255, 255, 255, 0.12);
  backdrop-filter: blur(12px);
  -webkit-backdrop-filter: blur(12px);
  border-bottom: 1px solid rgba(255, 255, 255, 0.15);
}

.menu-title {
  font-size: 13px;
  font-weight: 600;
  color: rgba(255, 255, 255, 0.9);
  letter-spacing: 0.2px;
}

.menu-links {
  display: flex;
  gap: 2px;
}

.menu-link {
  background: none;
  border: none;
  cursor: pointer;
  font-size: 13px;
  color: rgba(255, 255, 255, 0.75);
  padding: 4px 10px;
  border-radius: 5px;
  transition: background 0.15s, color 0.15s;
}

.menu-link:hover {
  background: rgba(255, 255, 255, 0.15);
  color: rgba(255, 255, 255, 1);
}

.menu-link.active {
  background: rgba(255, 255, 255, 0.2);
  color: white;
}

.config-panel {
  background: rgba(0, 0, 0, 0.2);
  border-bottom: 1px solid rgba(255, 255, 255, 0.1);
  padding: 12px 16px;
  font-size: 13px;
  color: rgba(255, 255, 255, 0.7);
}

.selection-card {
  display: flex;
  flex-direction: row;
  align-items: center;
  gap: 12px;
  padding: 20px 16px;
}

.btn-choose {
  padding: 6px 14px;
  border-radius: 6px;
  border: none;
  background: rgba(255, 255, 255, 0.2);
  color: white;
  font-size: 13px;
  cursor: pointer;
}

.btn-choose:hover {
  background: rgba(255, 255, 255, 0.3);
}

.rom-status {
  font-size: 12px;
  color: rgba(255, 255, 255, 0.6);
  overflow: hidden;
  text-overflow: ellipsis;
  white-space: nowrap;
}

.play-btn {
  margin: 0 16px;
  padding: 10px;
  border-radius: 8px;
  border: none;
  background: crimson;
  color: white;
  font-size: 14px;
  font-weight: 600;
  cursor: pointer;
  transition: background 0.15s;
  width: 10%;
}

.play-btn:hover:not(:disabled) {
  background: #357abd;
}

.play-btn:disabled {
  opacity: 0.4;
  cursor: not-allowed;
}
</style>